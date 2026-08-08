#ifndef EDITOR_CORE_EDITOR_H
#define EDITOR_CORE_EDITOR_H

/**
 * @brief Main entry point of the editor
 * Also manages instantiation of all the windows and the loop and stuff.
 */
class Editor {
public:
    Editor();
    ~Editor();
    void Run();
};

#endif // EDITOR_CORE_EDITOR_H