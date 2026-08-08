#ifndef EDITOR_WINDOWS_ASSET_BROWSER_H
#define EDITOR_WINDOWS_ASSET_BROWSER_H

#include <optional>

#include "engine/utils/uuid.h"

struct EditorState;

/**
 * @brief Minimal asset browser: lists source assets from the public AssetManager API and
 * supports importing, selecting, renaming (primary export name), moving, and deleting.
 * Never touches internal asset-management data directly.
 */
class AssetBrowserWindow {
public:
    void Draw(EditorState& state);

private:
    // fixed size buffers for imgui fields
    char importPathBuffer[512] = "";
    char movePathBuffer[512] = "";
    char renameBuffer[256] = "";

    // Tracks which selection the move/rename buffers were last filled from, so they refresh
    // when the selection changes but keep in-progress edits otherwise.
    std::optional<UUID> buffersSyncedFor;
};

#endif // EDITOR_WINDOWS_ASSET_BROWSER_H