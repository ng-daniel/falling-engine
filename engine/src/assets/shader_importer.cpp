#include <fstream>
#include <stdexcept>

#include "engine/assets/shader_importer.h"
#include "engine/assets/asset_data.h"

/**
 * @brief Loads a shader asset from the given file path.
 * @param path The file path to the shader asset.
 * @return Asset ptr to the loaded shader asset.
 */
std::unique_ptr<Asset> ShaderImporter::LoadAsset(const std::filesystem::path& path) {
    
    // load source from file
    std::ifstream shaderFile(path);
    if (!shaderFile.is_open()) {
        throw std::runtime_error("Failed to open shader file: " + path.string());
    }
    std::string shaderSource((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
    shaderFile.close();

    auto shaderAsset = std::make_unique<ShaderAsset>();
    shaderAsset->shaderSource = std::move(shaderSource);
    
    // get type from file extension
    if (path.extension() == ".vert") {
        shaderAsset->shaderType = ShaderAsset::ShaderType::Vertex;
    } else if (path.extension() == ".frag") {
        shaderAsset->shaderType = ShaderAsset::ShaderType::Fragment;
    } else {
        throw std::runtime_error("Unknown shader file extension: " + path.string());
    }

    return shaderAsset;
}