#pragma once

#include <filesystem>
#include <optional>

#include "engine/assets/asset_manager.h"
#include "engine/utils/uuid.h"

/**
 * @brief Only for data that needs to be globally accessible throughout the editor.
 */
struct EditorState {
    explicit EditorState(std::filesystem::path assetRoot)
        : assetManager(std::move(assetRoot)) {}

    AssetManager assetManager;

    /// Source asset currently selected in the Asset Browser, shown by the Inspector.
    std::optional<UUID> selectedSourceAssetId;
};
