#pragma once

#include "engine/serialization/jsonarchive.h"
#include "engine/assets/asset_data.h"

class SceneSerializer {
public:
    static void Serialize(JsonArchive& archive, const SceneAsset& scene, std::string_view name = "");
    static SceneAsset Deserialize(JsonArchive& archive, std::string_view name = "");
};