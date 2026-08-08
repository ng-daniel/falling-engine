#include "editor/core/editor.h"
#include "engine/debug/logger.h"

#include "editor/core/window.h"
#include "editor/core/editor_state.h"

Editor::Editor()
    : window(), globalStateData() {
    Logger::Info("Editor", "Editor initialized");
}