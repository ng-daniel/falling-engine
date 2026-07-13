#include <fstream>
#include <stdexcept>

#include "engine/assets/importers/shader_importer.h"
#include "engine/assets/asset_data.h"
#include "engine/assets/asset_helpers.h"
/**
 * @brief Loads a shader asset from the given metadata.
 * @param metadata The metadata of the shader asset.
 * @return std::vector<std::unique_ptr<Asset>> The loaded shader asset.
 */
std::vector<std::unique_ptr<Asset>> ShaderImporter::LoadAsset(AssetMetadata& metadata) {
    
    // load source from file
    std::ifstream shaderFile(metadata.path);
    if (!shaderFile.is_open()) {
        throw std::runtime_error("Failed to open shader file: " + metadata.path.string());
    }
    std::string shaderSource((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
    shaderFile.close();

    auto shaderAsset = std::make_unique<ShaderAsset>();
    shaderAsset->shaderSource = std::move(shaderSource);
    
    // get type from file extension
    if (metadata.path.extension() == ".vert") {
        shaderAsset->shaderType = ShaderAsset::ShaderType::Vertex;
    } else if (metadata.path.extension() == ".frag") {
        shaderAsset->shaderType = ShaderAsset::ShaderType::Fragment;
    } else {
        throw std::runtime_error("Unknown shader file extension: " + metadata.path.string());
    }

    ApplyMetadataToAsset(
        metadata.id, 
        metadata.path.stem().string(), 
        "Shader", 
        *shaderAsset
    );

    std::vector<std::unique_ptr<Asset>> assets;
    assets.push_back(std::move(shaderAsset));

    return assets;
}