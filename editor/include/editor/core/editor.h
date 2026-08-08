#ifndef EDITOR_CORE_EDITOR_H
#define EDITOR_CORE_EDITOR_H

#include "editor/core/editor_state.h"
#include "editor/core/window.h"

/**
 * @brief Main entry point of the editor
 * Also manages instantiation of all the windows and the loop and stuff.
 */
class Editor {
public:
    Editor();
private:
    Window window;
    EditorState globalStateData;
};

#endif // EDITOR_CORE_EDITOR_H