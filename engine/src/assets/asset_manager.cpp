#include "engine/assets/asset_manager.h"

/*
Asset Manager Implementation
--------------------------------

On startup, initializes the asset warehouse by scanning the asset directory.
Determines the appropriate AssetImporter for each asset based on file extension.

Lazily imports assets when requested by external code and stores them in the asset warehouse.
*/

/**
 * @brief Constructs an AssetManager instance.
 * 
 * @param root The root directory where assets are located.
 * 
 * @details
 * Initializes the asset warehouse from the asset root.
 */
AssetManager::AssetManager(std::filesystem::path root)
    : assetWarehouseService(root) {}