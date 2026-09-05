#include "engine/renderer/opengl/opengl_device.h"

#include "engine/renderer/renderer_structures.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "engine/debug/logger.h"
#include <limits>
#include <string>
#include <vector>

namespace {

    unsigned int CompileShader(unsigned int shaderType, std::string_view source, const char* stageName) {
        const unsigned int shaderId = glCreateShader(shaderType);
        const char* sourceData = source.data();
        const int sourceLength = static_cast<int>(source.size());
        glShaderSource(shaderId, 1, &sourceData, &sourceLength);
        glCompileShader(shaderId);

        int compileSucceeded = GL_FALSE;
        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileSucceeded);
        if (compileSucceeded == GL_TRUE) {
            return shaderId;
        }

        int logLength = 0;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> log(static_cast<std::size_t>(logLength > 1 ? logLength : 1));
        glGetShaderInfoLog(shaderId, logLength, nullptr, log.data());
        Logger::Error(
            "OpenGLDevice",
            std::string("Failed to compile ") + stageName + " shader: " + log.data()
        );
        glDeleteShader(shaderId);
        return 0;
    }

    /**
     * @brief Initializes openGL buffers by
     * 1. Initializing device data
     * 2. pulling vertex and index data
     * 3. configuring, binding, and uploading data to GPU buffers
     * 
     * @param renderData 
     */
    void InitializeBuffersForPrimitive(PrimitiveRenderData& renderData) {
        std::unique_ptr<OpenGLDeviceData> deviceData = std::make_unique<OpenGLDeviceData>();
        const PrimitiveData * primitive = &renderData.mesh->primitives[renderData.pIdx];

        auto& vertices = primitive->vertices;
        auto& indices = primitive->indices;

        // generate buffers
        glGenVertexArrays(1, &deviceData->VAO);
        glGenBuffers(1, &deviceData->VBO);
        glGenBuffers(1, &deviceData->EBO);

        // bind VAO containing VBO and EBO
        glBindVertexArray(deviceData->VAO);
        
        // bind vertex data to VAO
        glBindBuffer(GL_ARRAY_BUFFER, deviceData->VBO);
        glBufferData(
            GL_ARRAY_BUFFER,
            vertices.size() * sizeof(Vertex),
            vertices.data(),
            GL_STATIC_DRAW
        );
        
        // bind index data to EBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, deviceData->EBO);
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            indices.size() * sizeof(unsigned int), 
            indices.data(),
            GL_STATIC_DRAW
        );
        
        // configure vertex attribute pointers
        // skip tangent for now even though we have the data
        // because we don't do anything with that yet
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(1);	
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(2);	
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

        // unbind VAO
        glBindVertexArray(0);

        Logger::Info("OpenGLDevice", "Initialized buffers for mesh " + std::to_string(renderData.mesh->id) + ", primitive " + std::to_string(renderData.pIdx+1) + "/" + std::to_string(renderData.mesh->primitives.size()));
        Logger::Info("OpenGLDevice", "BufferValues: VAO=" + std::to_string(deviceData->VAO) +
            ", VBO=" + std::to_string(deviceData->VBO) +
            ", EBO=" + std::to_string(deviceData->EBO)
        );

        // move initialized device data into primitive render data
        renderData.graphicsDeviceData = std::move(deviceData);
    }

    /**
     * @brief Initiailizes OpenGL GPU buffer objects for the mesh
     */
    void InitializeBuffersForMesh(MeshRenderData& meshRenderData) {
        for (int i = 0; i < meshRenderData.primitives.size(); ++i) {
            InitializeBuffersForPrimitive(meshRenderData.primitives[i]);
        }
    }
}

void OpenGLDevice::ConfigureWindow() {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

/**
 * @brief Initializes OpenGL
 */
bool OpenGLDevice::Init(WindowManager& window) {
    this->window = &window;
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        // failed to initialize GLAD
        this->window->ForceClose();
        return false;
    }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    return true;
}

void OpenGLDevice::BeginFrame() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.1f, 0.1f, 0.5f, 1.0f);
}

void OpenGLDevice::Render(RenderData& renderData) {
    for (RenderSubmission& submission : renderData.submissions) {
        if (!submission.mesh) {
            continue;
        }

        if (!submission.mesh->initialized) {
            InitializeBuffersForMesh(*submission.mesh);
            submission.mesh->initialized = true;
        }

        for (PrimitiveRenderData& primitive : submission.mesh->primitives) {
            // Bind VAO and draw elements for each primitive
            // OpenGLDeviceData * deviceData = static_cast<OpenGLDeviceData*>(primitive.graphicsDeviceData.get());
            // if (deviceData) {
            //     glBindVertexArray(deviceData->VAO);
            //     glDrawElements(
            //         GL_TRIANGLES,
            //         primitive,
            //         GL_UNSIGNED_INT, 0);
            //     glBindVertexArray(0);
            // }
            Logger::Info("OpenGLDevice", "Rendering mesh " + std::to_string(submission.mesh->meshId) + " primitive " + std::to_string(primitive.pIdx));
        }
    }
}

void OpenGLDevice::EndFrame() {
}

SPDEVICE_RID OpenGLDevice::CreateShaderProgram(
        const std::string& vertexSource,
        const std::string& fragmentSource
    ) {
    
    const unsigned int vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource, "vertex");
    if (vertexShader == 0) {
        return {};
    }
    const unsigned int fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource, "fragment");
    if (fragmentShader == 0) {
        glDeleteShader(vertexShader);
        return {};
    }

    uint programId = glCreateProgram();
    glAttachShader(programId, vertexShader);
    glAttachShader(programId, fragmentShader);
    glLinkProgram(programId);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    int linkSucceeded = GL_FALSE;
    glGetProgramiv(programId, GL_LINK_STATUS, &linkSucceeded);
    if (linkSucceeded != GL_TRUE) {
        int logLength = 0;
        glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> log(static_cast<std::size_t>(logLength > 1 ? logLength : 1));
        glGetProgramInfoLog(programId, logLength, nullptr, log.data());
        Logger::Error("OpenGLDevice", std::string("Failed to link shader program: ") + log.data());
        glDeleteProgram(programId);
        return {};
    }

    auto shaderProgram = std::make_unique<OpenGLShaderProgram>();
    shaderProgram->programId = programId;
    shaderProgram->modelUniform = glGetUniformLocation(programId, "model");
    shaderProgram->viewUniform = glGetUniformLocation(programId, "view");
    shaderProgram->projectionUniform = glGetUniformLocation(programId, "projection");
    shaderPrograms.emplace(programId, std::move(shaderProgram));
    Logger::Info("OpenGLDevice", "Created shader program " + std::to_string(programId));
    return programId;
}

void OpenGLDevice::DestroyShaderProgram(uint programId) {
    const auto shaderIt = shaderPrograms.find(programId);
    if (shaderIt == shaderPrograms.end()) {
        return;
    }
    glDeleteProgram(shaderIt->second->programId);
    shaderPrograms.erase(shaderIt);
}

OpenGLShaderProgram * OpenGLDevice::FindShaderProgram(uint programId) {
    const auto shaderIt = shaderPrograms.find(programId);
    return shaderIt == shaderPrograms.end() ? nullptr : shaderIt->second.get();
}

void OpenGLDevice::Close() {
    for (const auto& [handle, shaderProgram] : shaderPrograms) {
        glDeleteProgram(shaderProgram->programId);
    }
    shaderPrograms.clear();
}
