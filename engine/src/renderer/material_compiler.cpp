#include "engine/renderer/material_compiler.h"

#include "engine/assets/asset_manager.h"
#include "engine/utils/uuid.h"

#include <stdexcept>

ShaderProgramData MaterialShaderService::CompileMaterial(
    UUID materialId,
    UUID vertexShaderId,
    UUID fragmentShaderId,
    AssetManager& assetManager
) {
    MaterialAsset* modelMaterial = assetManager.RequestAsset<MaterialAsset>(materialId);
    const ShaderAsset* basicVertexShader = assetManager.RequestAssetReadOnly<ShaderAsset>(vertexShaderId);
    const ShaderAsset* basicFragmentShader = assetManager.RequestAssetReadOnly<ShaderAsset>(fragmentShaderId);
    if (!modelMaterial || !basicVertexShader || !basicFragmentShader) {
        throw std::runtime_error("Failed to load material shader inputs.");
    }

    if (basicVertexShader->id == INVALID_UUID || basicFragmentShader->id == INVALID_UUID) {
        throw std::invalid_argument("A shader program requires both shader assets to have UUIDs.");
    }
    if (basicVertexShader->shaderType != ShaderAsset::ShaderType::Vertex) {
        throw std::invalid_argument("The vertex shader asset has the wrong shader type.");
    }
    if (basicFragmentShader->shaderType != ShaderAsset::ShaderType::Fragment) {
        throw std::invalid_argument("The fragment shader asset has the wrong shader type.");
    }

    ShaderProgramData shaderProgram;
    shaderProgram.id = UUIDGenerator::GenerateUUID();
    shaderProgram.vertexShaderId = basicVertexShader->id;
    shaderProgram.fragmentShaderId = basicFragmentShader->id;

    modelMaterial->runtimeShader = shaderProgram.id;
    return shaderProgram;
}
