#include "engine/renderer/material_compiler.h"

#include "engine/utils/uuid.h"

#include <stdexcept>

ShaderProgramData MaterialShaderService::CompileMaterial(
    MaterialAsset& materialAsset,
    const ShaderAsset& vertexShader,
    const ShaderAsset& fragmentShader
) {
    if (vertexShader.id == INVALID_UUID || fragmentShader.id == INVALID_UUID) {
        throw std::invalid_argument("A shader program requires both shader assets to have UUIDs.");
    }
    if (vertexShader.shaderType != ShaderAsset::ShaderType::Vertex) {
        throw std::invalid_argument("The vertex shader asset has the wrong shader type.");
    }
    if (fragmentShader.shaderType != ShaderAsset::ShaderType::Fragment) {
        throw std::invalid_argument("The fragment shader asset has the wrong shader type.");
    }

    ShaderProgramData shaderProgram;
    shaderProgram.id = UUIDGenerator::GenerateUUID();
    shaderProgram.vertexShaderId = vertexShader.id;
    shaderProgram.fragmentShaderId = fragmentShader.id;

    materialAsset.runtimeShader = shaderProgram.id;
    return shaderProgram;
}
