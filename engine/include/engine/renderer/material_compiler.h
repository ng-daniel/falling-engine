#pragma once
   
#include "engine/assets/asset_data.h"
#include "engine/renderer/renderer_structures.h"

/**
 * @brief Builds runtime shader programs and attaches them to materials.
 *
 * This service only changes in-memory asset data. The runtime shader link is not
 * represented in asset metadata or serialization.
 */
class MaterialShaderService {
public:
    /**
     * @brief Creates a runtime shader program description and assigns its UUID
     * to materialAsset.runtimeShader.
     */
    static ShaderProgramData CompileMaterial(
        MaterialAsset& materialAsset,
        const ShaderAsset& vertexShader,
        const ShaderAsset& fragmentShader
    );
};
