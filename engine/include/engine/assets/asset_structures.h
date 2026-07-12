#ifndef ENGINE_ASSETS_ASSET_STRUCTURES_H
#define ENGINE_ASSETS_ASSET_STRUCTURES_H

#include <filesystem>
#include <string>

#include "engine/utils/uuid.h"

struct AssetMetadata {
    UUID id;
    std::string type;
    std::string importer;
    std::filesystem::path path;
    bool loaded; // in CPU memory or not
};

#endif // ENGINE_ASSETS_ASSET_STRUCTURES_H