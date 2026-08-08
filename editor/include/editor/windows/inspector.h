#ifndef EDITOR_WINDOWS_INSPECTOR_H
#define EDITOR_WINDOWS_INSPECTOR_H

struct EditorState;

/**
 * @brief Asset inspector: displays metadata, type, path, runtime sub-assets/dependencies,
 * and importer info for the source asset currently selected in the Asset Browser. Offers a
 * reimport control. Reads everything through the public AssetManager API.
 */
class InspectorWindow {
public:
    void Draw(EditorState& state);
};

#endif // EDITOR_WINDOWS_INSPECTOR_H