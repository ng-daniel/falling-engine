#include "engine/renderer/opengl/opengl_device.h"

#include "engine/renderer/renderer_structures.h"
#include "glm/glm.hpp"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

namespace {

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
            &vertices[0],
            GL_STATIC_DRAW
        );
        
        // bind index data to EBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, deviceData->EBO);
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            indices.size() * sizeof(unsigned int), 
            &indices[0],
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
    for (auto& [meshId, meshRenderData] : renderData.meshRenderData) {
        if (!meshRenderData.initialized) {
            InitializeBuffersForMesh(meshRenderData);
            meshRenderData.initialized = true;
        }
    }
}

void OpenGLDevice::EndFrame() {
}

void OpenGLDevice::Close() {
}
