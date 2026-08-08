#include "editor/windows/inspector.h"
#include "editor/core/editor_state.h"
#include "engine/assets/asset_manager.h"
#include "engine/assets/asset_helpers.h"

#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include <imgui.h>

namespace {
    /**
     * @brief Draws a single dependency with a label and UUID.
     * 
     * @param label The label to display for the dependency.
     * @param id The UUID of the dependency.
     */
    void DrawDependency(const char* label, UUID id) {
        if (id == 0) {
            return;
        }
        ImGui::BulletText("%s: %llu", label, static_cast<unsigned long long>(id));
    }

    // Dependencies aren't part of AssetInfo, so for the asset types that reference other
    // assets we load them through the public API (by type) and display their UUIDs directly.
    void DrawDependencies(AssetManager& assetManager, const AssetInfo& runtimeAsset) {
        Asset::AssetType type;
        try {
            type = GetAssetTypeFromString(runtimeAsset.type);
        } catch (const std::runtime_error&) {
            return;
        }

        switch (type) {
            case Asset::AssetType::Model: {
                const ModelAsset* model = assetManager.RequestAssetReadOnly<ModelAsset>(runtimeAsset.id);
                if (!model) {
                    return;
                }
                ImGui::Text("Meshes (%zu):", model->meshes.size());
                for (UUID meshId : model->meshes) {
                    DrawDependency("Mesh", meshId);
                }
                ImGui::Text("Materials (%zu):", model->materials.size());
                for (UUID materialId : model->materials) {
                    DrawDependency("Material", materialId);
                }
                break;
            }
            case Asset::AssetType::Material: {
                const MaterialAsset* material = assetManager.RequestAssetReadOnly<MaterialAsset>(runtimeAsset.id);
                if (!material) {
                    return;
                }
                DrawDependency("Base Color Texture", material->baseColorTexture);
                DrawDependency("Metallic/Roughness Texture", material->metallicRoughnessTexture);
                DrawDependency("Normal Texture", material->normalTexture);
                DrawDependency("Occlusion Texture", material->occlusionTexture);
                DrawDependency("Emissive Texture", material->emissiveTexture);
                break;
            }
            case Asset::AssetType::Texture: {
                const TextureAsset* texture = assetManager.RequestAssetReadOnly<TextureAsset>(runtimeAsset.id);
                if (!texture) {
                    return;
                }
                DrawDependency("Image", texture->image);
                break;
            }
            default:
                break;
        }
    }
}

void InspectorWindow::Draw(EditorState& state) {
    ImGui::Begin("Inspector");

    if (!state.selectedSourceAssetId.has_value()) {
        ImGui::TextDisabled("No asset selected. Select one in the Asset Browser.");
        ImGui::End();
        return;
    }

    std::optional<AssetInfo> source = state.assetManager.GetSourceAssetInfo(*state.selectedSourceAssetId);
    if (!source.has_value()) {
        ImGui::TextDisabled("Selected asset no longer exists.");
        ImGui::End();
        return;
    }

    ImGui::Text("Name: %s", source->name.c_str());
    ImGui::Text("Type: %s", source->type.c_str());
    ImGui::Text("Source ID: %llu", static_cast<unsigned long long>(source->id));
    ImGui::TextWrapped("Path: %s", source->path.string().c_str());
    ImGui::Text("Loaded: %s", source->loaded ? "yes" : "no");

    ImGui::Separator();
    if (ImGui::Button("Reimport")) {
        state.assetManager.ReimportAsset(source->sourceId);
    }
    ImGui::SameLine();
    ImGui::TextDisabled("(re-runs the importer)");

    ImGui::Separator();
    ImGui::TextUnformatted("Runtime sub-assets:");

    std::vector<AssetInfo> runtimeAssets = state.assetManager.GetRuntimeAssetsForSource(source->sourceId);
    if (runtimeAssets.empty()) {
        ImGui::TextDisabled("None yet. Click Reimport to generate them.");
    }
    for (const AssetInfo& runtimeAsset : runtimeAssets) {
        ImGui::PushID(static_cast<int>(runtimeAsset.id));
        std::string header = runtimeAsset.name + "  [" + runtimeAsset.type + "]" +
            (runtimeAsset.loaded ? "" : "  (unloaded)");
        if (ImGui::TreeNode("RuntimeAssetNode", "%s", header.c_str())) {
            ImGui::Text("ID: %llu", static_cast<unsigned long long>(runtimeAsset.id));
            DrawDependencies(state.assetManager, runtimeAsset);
            ImGui::TreePop();
        }
        ImGui::PopID();
    }

    ImGui::End();
}