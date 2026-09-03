#pragma once

#include "editor/core/editor_state.h"
#include "editor/core/editor_window.h"
#include "editor/windows/asset_browser.h"
#include "editor/windows/inspector.h"

/**
 * @brief Main entry point of the editor
 * Also manages instantiation of all the windows and the loop and stuff.
 */
class Editor {
public:
    Editor();

    /// @brief Runs the main editor loop until the window is closed.
    void Run();

private:
    EditorWindow window;
    EditorState globalStateData;
    AssetBrowserWindow assetBrowserWindow;
    InspectorWindow inspectorWindow;
};
