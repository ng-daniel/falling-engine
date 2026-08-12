#include "editor/core/editor.h"
#include "engine/debug/logger.h"

#include "editor/core/window.h"
#include "editor/core/editor_state.h"

namespace {
    // Editor operates directly on the game's asset library, same convention as game/main.cpp.
    const char* ASSET_ROOT = "./game/assets";
}

Editor::Editor()
    : window(), globalStateData(ASSET_ROOT) {
    Logger::Info("Editor", "Editor initialized");
}

void Editor::Run() {
    while (!window.ShouldClose()) {
        window.BeginFrame();

        assetBrowserWindow.Draw(globalStateData);
        inspectorWindow.Draw(globalStateData);

        window.EndFrame();
    }
}