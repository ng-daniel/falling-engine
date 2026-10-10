#include "engine/renderer/opengl/opengl_device.h"

#include "engine/renderer/renderer_structures.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "engine/debug/logger.h"

#include <limits>
#include <stdexcept>
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

    bool GetTextureFormats(int channelCount, GLint& internalFormat, GLenum& sourceFormat) {
        if (channelCount == 1) {
            internalFormat = GL_R8;
            sourceFormat = GL_RED;
            return true;
        } else if (channelCount == 2) {
            internalFormat = GL_RG8;
            sourceFormat = GL_RG;
            return true;
        } else if (channelCount == 3) {
            internalFormat = GL_RGB8;
            sourceFormat = GL_RGB;
            return true;
        } else if (channelCount == 4) {
            internalFormat = GL_RGBA8;
            sourceFormat = GL_RGBA;
            return true;
        } else {
            return false;
        }
    }

    GLint GetMagnificationFilter(TextureAsset::FilterType filter) {
        return filter == TextureAsset::FilterType::Nearest ? GL_NEAREST : GL_LINEAR;
    }

    GLint GetMinificationFilter(TextureAsset::FilterType filter) {
        switch (filter) {
            case TextureAsset::FilterType::Nearest:
            case TextureAsset::FilterType::Linear:
            case TextureAsset::FilterType::NearestMipmapNearest:
            case TextureAsset::FilterType::LinearMipmapNearest:
            case TextureAsset::FilterType::NearestMipmapLinear:
            case TextureAsset::FilterType::LinearMipmapLinear:
                return static_cast<GLint>(filter);
            case TextureAsset::FilterType::Undefined:
                return GL_LINEAR_MIPMAP_LINEAR;
        }
        return GL_LINEAR_MIPMAP_LINEAR;
    }

    bool UsesMipmaps(GLint minificationFilter) {
        return minificationFilter != GL_NEAREST && minificationFilter != GL_LINEAR;
    }

    /**
     * @brief Initializes openGL buffers by
     * 1. Initializing device data
     * 2. pulling vertex and index data
     * 3. configuring, binding, and uploading data to GPU buffers
     * 
     * @param renderData 
     */
    void InitializeBuffersForPrimitive(PrimitiveRenderData& renderData, const MeshAsset * meshAsset) {
        std::unique_ptr<OpenGLDeviceData> deviceData = std::make_unique<OpenGLDeviceData>();
        const PrimitiveData * primitive = &meshAsset->primitives[renderData.pIdx];

        auto& vertices = primitive->vertices;
        auto& indices = primitive->indices;

        if (indices.size() > static_cast<std::size_t>(std::numeric_limits<GLsizei>::max())) {
            throw std::runtime_error("Primitive has too many indices for the graphics device.");
        }
        deviceData->indexCount = static_cast<std::uint32_t>(indices.size());

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

        Logger::Info("OpenGLDevice", "Initialized buffers for mesh " + std::to_string(meshAsset->id) + ", primitive " + std::to_string(renderData.pIdx+1) + "/" + std::to_string(meshAsset->primitives.size()));
        Logger::Info("OpenGLDevice", "BufferValues: VAO=" + std::to_string(deviceData->VAO) +
            ", VBO=" + std::to_string(deviceData->VBO) +
            ", EBO=" + std::to_string(deviceData->EBO)
        );

        // move initialized device data into primitive render data
        renderData.graphicsDeviceData = std::move(deviceData);
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
    glClearColor(0.5f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLDevice::Render(const RenderData& renderData) {
    for (const DrawCommand& command : renderData.commands) {
        if (!command.primitive || !command.primitive->graphicsDeviceData) {
            continue;
        }

        // bind shader program
        const auto shaderIt = shaderPrograms.find(command.shaderProgram);
        if (shaderIt == shaderPrograms.end()) {
            Logger::Error("OpenGLDevice", "Cannot render with shader program " + std::to_string(command.shaderProgram) + " because it does not exist.");
            continue;
        }
        const OpenGLShaderProgram* shaderProgram = shaderIt->second.get();
        glUseProgram(shaderProgram->programId);

        // set base color texture
        if (command.material.HasTexture(MaterialTextureSlot::BaseColor)) {
            const TDEVICE_RID textureId = command.material.textures[static_cast<std::size_t>(MaterialTextureSlot::BaseColor)];
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textureId);
        }

        // set orientation/3d space matrix values
        glUniformMatrix4fv(shaderProgram->modelUniform, 1, GL_FALSE, command.model.Data());
        glUniformMatrix4fv(shaderProgram->viewUniform, 1, GL_FALSE, renderData.frame.view.Data());
        glUniformMatrix4fv(shaderProgram->projectionUniform, 1, GL_FALSE, renderData.frame.projection.Data());
        // glUniformMatrix4fv(shaderProgram->projectionUniform, 1, GL_FALSE, Matrix4(0.0f).Data());
        
        // retrieve and bind vertex buffers
        const OpenGLDeviceData* deviceData = static_cast<const OpenGLDeviceData*>(command.primitive->graphicsDeviceData.get());
        glBindVertexArray(deviceData->VAO);
        glDrawElements(GL_TRIANGLES, deviceData->indexCount, GL_UNSIGNED_INT, 0);
    }
}

void OpenGLDevice::EndFrame() {
}

void OpenGLDevice::Close() {
    for (const auto& [handle, shaderProgram] : shaderPrograms) {
        glDeleteProgram(shaderProgram->programId);
    }
    shaderPrograms.clear();

    for (const auto& textureId : textures) {
        glDeleteTextures(1, &textureId);
    }
    textures.clear();
}

/**
 * @brief Initiailizes OpenGL GPU buffer objects for the mesh
 */
void OpenGLDevice::InitializeGPUBuffersForMesh(MeshRenderData& meshRenderData, const MeshAsset * meshAsset) {
    for (int i = 0; i < meshRenderData.primitives.size(); ++i) {
        InitializeBuffersForPrimitive(meshRenderData.primitives[i], meshAsset);
    }
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

/**
 * @brief Runs the OpenGL procedure for generating textures
 * on the GPU
 * 
 * @param texture 
 * @param image 
 * @return TDEVICE_RID the openGL texture ID 
 */
TDEVICE_RID OpenGLDevice::CreateTexture(
    const TextureAsset& texture,
    const ImageAsset& image
) {
    // error checking
    GLint internalFormat = 0;
    GLenum sourceFormat = 0;
    if (image.width <= 0 ||
        image.height <= 0 ||
        !GetTextureFormats(image.numChannels, internalFormat, sourceFormat))
    {
        Logger::Error("OpenGLDevice", "Cannot create a texture from invalid image dimensions or channels.");
        return INVALID_TDEVICE_RID;
    }
    const std::size_t expectedDataSize = static_cast<std::size_t>(image.width)
        * static_cast<std::size_t>(image.height)
        * static_cast<std::size_t>(image.numChannels);
    if (image.data.size() < expectedDataSize) {
        Logger::Error("OpenGLDevice", "Cannot create a texture from incomplete image data.");
        return INVALID_TDEVICE_RID;
    }

    // generate and bind texture
    GLuint textureId = 0;
    glGenTextures(1, &textureId);
    if (textureId == 0) {
        Logger::Error("OpenGLDevice", "OpenGL failed to allocate a texture.");
        return INVALID_TDEVICE_RID;
    }
    glBindTexture(GL_TEXTURE_2D, textureId);

    // set texture parameters
    const GLint minificationFilter = GetMinificationFilter(texture.min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minificationFilter);
    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MAG_FILTER,
        GetMagnificationFilter(texture.mag_filter)
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(texture.wrap_s));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(texture.wrap_t));

    GLint previousUnpackAlignment = 4;
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &previousUnpackAlignment);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        internalFormat,
        image.width,
        image.height,
        0,
        sourceFormat,
        GL_UNSIGNED_BYTE,
        image.data.data()
    );
    glPixelStorei(GL_UNPACK_ALIGNMENT, previousUnpackAlignment);

    if (UsesMipmaps(minificationFilter)) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    // unbind texture and store in 
    glBindTexture(GL_TEXTURE_2D, 0);
    textures.emplace(textureId);
    return textureId;
}

void OpenGLDevice::DestroyTexture(TDEVICE_RID textureId) {
    const auto texture = textures.find(textureId);
    if (texture == textures.end()) {
        return;
    }

    glDeleteTextures(1, &(*texture));
    textures.erase(texture);
}