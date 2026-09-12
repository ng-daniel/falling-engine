#include "engine/renderer/renderer.h"
#include "engine/renderer/opengl/opengl_device.h"
#include "engine/renderer/renderer_structures.h"

#include "engine/debug/logger.h"
#include "engine/input/input_manager.h"

#include <array>
#include <stdexcept>
#include <utility>

namespace {
    PrimitiveRenderData BuildPrimitiveRenderData(const MeshAsset * parentMesh, int pIdx) {
        PrimitiveRenderData primitiveRenderData;
        primitiveRenderData.meshId = parentMesh->id;
        primitiveRenderData.pIdx = pIdx;
        primitiveRenderData.materialId = parentMesh->primitives[pIdx].material;
        primitiveRenderData.graphicsDeviceData = nullptr;
        return primitiveRenderData;
    }
    
    MeshRenderData BuildMeshRenderData(const MeshAsset * meshAsset) {
        MeshRenderData meshRenderData;
        meshRenderData.meshId = meshAsset->id;
        meshRenderData.meshId = meshAsset->id;
        return meshRenderData;
    }

    GraphicsDeviceMaterialData BuildGraphicsDeviceMaterialData(const MaterialAsset * materialAsset) {
        GraphicsDeviceMaterialData materialData;
        materialData.materialId = materialAsset->id;
        materialData.baseColorFactor = materialAsset->baseColorFactor;
        materialData.metallicFactor = materialAsset->metallicFactor;
        materialData.roughnessFactor = materialAsset->roughnessFactor;
        materialData.occlusionFactor = materialAsset->occlusionFactor;
        materialData.normalFactor = materialAsset->normalFactor;
        materialData.emissiveFactor = materialAsset->emissiveFactor;
        return materialData;
    }

    DrawCommand BuildDrawCommand(
        PrimitiveRenderData* primitive,
        SPDEVICE_RID shaderProgramId,
        const Matrix4& modelTransform,
        const GraphicsDeviceMaterialData& material
    ) {
        DrawCommand command;
        command.primitive = primitive;
        command.shaderProgram = shaderProgramId;
        command.model = modelTransform;
        command.material = material;
        return command;
    }

    using MaterialShaderBinding = std::tuple<SPDEVICE_RID, GraphicsDeviceMaterialData>;
    const int SHADER_SLOT = 0;
    const int MATERIAL_SLOT = 1;
}


Renderer::Renderer(AssetManager& assetManager)
    : assetManagerRef(assetManager)
    , device(std::make_unique<OpenGLDevice>()) {
    // auto select OPENGL for now
}

Renderer::~Renderer() {
    if (device) {
        for (auto& [shaderProgramId, shaderProgram] : shaderCache) {
            if (shaderProgram.deviceProgramId != INVALID_SPDEVICE_RID) {
                device->DestroyShaderProgram(shaderProgram.deviceProgramId);
            }
        }
        for (const auto& [textureId, deviceTextureId] : textureCache) {
            if (deviceTextureId != INVALID_TDEVICE_RID) {
                device->DestroyTexture(deviceTextureId);
            }
        }
        device->Close();
    }
}

void Renderer::ConfigureWindow() {
    device->ConfigureWindow();
}

bool Renderer::Init(WindowManager& window) {
    if (!device->Init(window)) {
        return false;
    }
    this->window = &window;
    return true;
}

void Renderer::BeginFrame(const InputManager& inputManager) {
    frameSubmissions.clear();
    frameData.commands.clear();
    if (window) {
        frameData.frame = debugCamera.Update(*window, inputManager);
    }
    device->BeginFrame();
}

void Renderer::SubmitMesh(UUID meshId, const Matrix4& worldTransform) {
    MeshRenderData* meshRenderData = GetOrCreateMeshRenderData(meshId);
    if (!meshRenderData) {
        return;
    }
    if (!meshRenderData->initialized) {
        device->InitializeGPUBuffersForMesh(*meshRenderData, assetManagerRef.RequestAssetReadOnly<MeshAsset>(meshId));
        meshRenderData->initialized = true;
    }
    frameSubmissions.push_back({ meshRenderData, worldTransform });
}

void Renderer::Render() {
    BuildDrawCommands();
    device->Render(frameData);
}

void Renderer::EndFrame() {
    device->EndFrame();
}

UUID Renderer::RegisterShaderProgram(const ShaderProgramData& shaderProgram) {
    if (shaderProgram.id == INVALID_UUID
        || shaderProgram.vertexShaderId == INVALID_UUID
        || shaderProgram.fragmentShaderId == INVALID_UUID) {
        throw std::invalid_argument("A shader program requires an ID and both shader assets.");
    }

    // check if the shader program is already cached
    const auto cachedProgram = shaderCache.find(shaderProgram.id);
    if (cachedProgram != shaderCache.end()) {
        if (cachedProgram->second.vertexShaderId != shaderProgram.vertexShaderId
            || cachedProgram->second.fragmentShaderId != shaderProgram.fragmentShaderId) {
            throw std::invalid_argument("A different shader program already uses this UUID.");
        }
        return cachedProgram->first;
    }

    // load the shader assets
    const ShaderAsset* vertexShader = assetManagerRef.RequestAssetReadOnly<ShaderAsset>(
        shaderProgram.vertexShaderId
    );
    const ShaderAsset* fragmentShader = assetManagerRef.RequestAssetReadOnly<ShaderAsset>(
        shaderProgram.fragmentShaderId
    );
    if (!vertexShader || !fragmentShader) {
        throw std::runtime_error("Failed to load vertex or fragment shader asset.");
    }
    if (vertexShader->shaderType != ShaderAsset::ShaderType::Vertex
        || fragmentShader->shaderType != ShaderAsset::ShaderType::Fragment) {
        throw std::invalid_argument("Shader assets do not match their requested stages.");
    }

    // create the shader program on the graphics device
    ShaderProgramData runtimeProgram = shaderProgram;
    runtimeProgram.deviceProgramId = device->CreateShaderProgram(
        vertexShader->shaderSource,
        fragmentShader->shaderSource
    );
    if (runtimeProgram.deviceProgramId == INVALID_SPDEVICE_RID) {
        throw std::runtime_error("The graphics device failed to create the shader program.");
    }

    // put generated program into cache
    shaderCache.emplace(runtimeProgram.id, runtimeProgram);
    return runtimeProgram.id;
}

void Renderer::BuildDrawCommands() {
    frameData.commands.clear();

    std::unordered_map<UUID, MaterialShaderBinding> resolvedMaterials;
    std::unordered_set<UUID> failedMaterials;

    // reserve space for draw commands to avoid reallocations
    std::size_t commandCount = 0;
    for (const RenderSubmission& submission : frameSubmissions) {
        if (submission.mesh) {
            commandCount += submission.mesh->primitives.size();
        }
    }
    frameData.commands.reserve(commandCount);
    
    // iterate through all mesh submissions and their primitives
    for (const RenderSubmission& submission : frameSubmissions) {
        if (!submission.mesh) {
            Logger::Warning(
                "Renderer",
                "Skipping submission with null mesh pointer."
            );
            continue;
        }
        if (!submission.mesh->initialized) {
            Logger::Warning(
                "Renderer",
                "Skipping uninitialized mesh " + std::to_string(submission.mesh->meshId) + "."
            );
            continue;
        }
        for (PrimitiveRenderData& primitive : submission.mesh->primitives) {
            if (failedMaterials.contains(primitive.materialId)) {
                Logger::Warning(
                    "Renderer",
                    "Skipping primitive with material " + std::to_string(primitive.materialId)
                        + " because it failed to resolve earlier."
                );
                continue;
            }

            // create and store new material-shader binding if it doesn't exist
            if (resolvedMaterials.find(primitive.materialId) == resolvedMaterials.end()) {
                MaterialShaderBinding binding;
                bool result = BuildMaterialShaderData(
                    primitive.materialId,
                    std::get<SHADER_SLOT>(binding),
                    std::get<MATERIAL_SLOT>(binding)
                );
                if (!result) {
                    failedMaterials.insert(primitive.materialId);
                    continue;
                }
                resolvedMaterials.emplace(primitive.materialId, std::move(binding));
            }

            // build draw command for this primitive using
            // 1. primitive data
            // 2. shader program
            // 3. world transform
            // 4. material data
            const auto resolvedBinding = resolvedMaterials.find(primitive.materialId);
            frameData.commands.push_back(
                BuildDrawCommand(
                    &primitive,
                    std::get<SHADER_SLOT>(resolvedBinding->second),
                    submission.worldTransform,
                    std::get<MATERIAL_SLOT>(resolvedBinding->second)
                )
            );
        }
    }
}

/**
 * @brief Given material ID, fetches all assets the material ID depends on,
 * builds a GraphicsDeviceMaterialData, and returns the shader program ID and material data
 * in place at the provided references
 * 
 * @param materialId 
 * @param shaderProgramId 
 * @param resolvedMaterial 
 * @return true 
 * @return false 
 */
bool Renderer::BuildMaterialShaderData(
    UUID materialId,
    SPDEVICE_RID& shaderProgramId,
    GraphicsDeviceMaterialData& resolvedMaterial
) {
    if (materialId == INVALID_UUID) {
        return false;
    }

    const MaterialAsset* material = assetManagerRef.RequestAssetReadOnly<MaterialAsset>(materialId);
    if (!material) {
        Logger::Error(
            "Renderer",
            "Failed to resolve material asset " + std::to_string(materialId) + "."
        );
        return false;
    }

    const auto shaderProgram = shaderCache.find(material->runtimeShader);
    if (material->runtimeShader == INVALID_UUID || shaderProgram == shaderCache.end()) {
        Logger::Error(
            "Renderer",
            "Material " + std::to_string(materialId)
                + " has no registered runtime shader program."
        );
        return false;
    }

    resolvedMaterial = BuildGraphicsDeviceMaterialData(material);

    const std::array<UUID, MATERIAL_TEXTURE_SLOT_COUNT> materialTextures = {
        material->baseColorTexture,
        material->metallicRoughnessTexture,
        material->normalTexture,
        material->occlusionTexture,
        material->emissiveTexture
    };
    for (std::size_t slotIndex = 0; slotIndex < materialTextures.size(); ++slotIndex) {
        const TDEVICE_RID deviceTextureId = GetOrCreateTexture(materialTextures[slotIndex]);
        resolvedMaterial.textures[slotIndex] = deviceTextureId;
        if (deviceTextureId != INVALID_TDEVICE_RID) {
            resolvedMaterial.textureMask |= MaterialTextureBit(
                static_cast<MaterialTextureSlot>(slotIndex)
            );
        }
    }

    shaderProgramId = shaderProgram->second.deviceProgramId;
    return shaderProgramId != INVALID_SPDEVICE_RID;
}

/**
 * @brief Pulls required assets from the database and
 * creates a texture on the graphics device if it doesn't already exist.
 * 
 * @param textureId 
 * @return TDEVICE_RID 
 */
TDEVICE_RID Renderer::GetOrCreateTexture(UUID textureId) {
    if (textureId == INVALID_UUID) {
        return INVALID_TDEVICE_RID;
    }

    // search texture cache for texture
    const auto cachedTexture = textureCache.find(textureId);
    if (cachedTexture != textureCache.end()) {
        return cachedTexture->second;
    }

    // otherwise pull required info from asset database
    const TextureAsset* texture = assetManagerRef.RequestAssetReadOnly<TextureAsset>(textureId);
    if (!texture || texture->image == INVALID_UUID) {
        Logger::Error(
            "Renderer",
            "Failed to resolve texture asset " + std::to_string(textureId) + "."
        );
        return INVALID_TDEVICE_RID;
    }
    const ImageAsset* image = assetManagerRef.RequestAssetReadOnly<ImageAsset>(texture->image);
    if (!image) {
        Logger::Error(
            "Renderer",
            "Failed to resolve image for texture asset " + std::to_string(textureId) + "."
        );
        return INVALID_TDEVICE_RID;
    }

    // create texture on graphics device
    const TDEVICE_RID deviceTextureId = device->CreateTexture(*texture, *image);
    if (deviceTextureId == INVALID_TDEVICE_RID) {
        Logger::Error(
            "Renderer",
            "Graphics device failed to create texture " + std::to_string(textureId) + "."
        );
        return INVALID_TDEVICE_RID;
    }

    // push texture into cache and return
    textureCache.emplace(textureId, deviceTextureId);
    return deviceTextureId;
}

MeshRenderData* Renderer::GetOrCreateMeshRenderData(UUID meshId) {
    const auto cachedMeshIt = meshCache.find(meshId);
    if (cachedMeshIt != meshCache.end()) {
        return &cachedMeshIt->second;
    }

    const MeshAsset* meshAsset = assetManagerRef.RequestAssetReadOnly<MeshAsset>(meshId);
    if (!meshAsset) {
        Logger::Error("Renderer", "Failed to load mesh asset " + std::to_string(meshId) + ".");
        return nullptr;
    }

    MeshRenderData meshRenderData = BuildMeshRenderData(meshAsset);
    meshRenderData.primitives.reserve(meshAsset->primitives.size());
    for (std::size_t primitiveIndex = 0; primitiveIndex < meshAsset->primitives.size(); ++primitiveIndex) {
        meshRenderData.primitives.push_back(
            BuildPrimitiveRenderData(meshAsset, static_cast<int>(primitiveIndex))
        );
    }

    const auto insertionResult = meshCache.emplace(meshId, std::move(meshRenderData));
    return &insertionResult.first->second;
}
