#include "editor/windows/asset_browser.h"
#include "editor/core/editor_state.h"
#include "engine/assets/asset_manager.h"

#include <cstdio>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#include <imgui.h>

namespace {
    
    /**
     * @brief Utility function to copy a string into a fixed-size buffer,
     truncating if necessary and ensuring null-termination.
     * 
     * @param buffer 
     * @param bufferSize 
     * @param value 
     */
    void CopyToBuffer(char* buffer, size_t bufferSize, const std::string& value) {
        std::snprintf(buffer, bufferSize, "%s", value.c_str());
    }

    // The "primary" runtime sub-asset shares the file's stem as its export name by default
    // (see SourceAssetMetadata::GetPrimaryRuntimeMetadata); it's the one users think of as
    // "the asset" when renaming from the browser.
    std::optional<AssetInfo> FindPrimaryRuntimeAsset(AssetManager& assetManager, const AssetInfo& source) {
        for (const AssetInfo& runtimeAsset : assetManager.GetRuntimeAssetsForSource(source.sourceId)) {
            if (runtimeAsset.name == source.name) {
                return runtimeAsset;
            }
        }
        return std::nullopt;
    }
}

void AssetBrowserWindow::Draw(EditorState& state) {
    ImGui::Begin("Asset Browser");

    /// Import New File Fields
    /// ---------------------------------------------

    ImGui::TextUnformatted("Import external file:");
    ImGui::SetNextItemWidth(-90.0f);
    ImGui::InputText("##ImportPath", importPathBuffer, sizeof(importPathBuffer));
    ImGui::SameLine();
    if (ImGui::Button("Import", ImVec2(80.0f, 0.0f)) && importPathBuffer[0] != '\0') {
        state.assetManager.ImportAsset(importPathBuffer);
        importPathBuffer[0] = '\0';
    }

    ImGui::Separator();

    /// Asset List
    /// ---------------------------------------------

    std::vector<AssetInfo> sourceAssets = state.assetManager.GetAllSourceAssets();
    ImGui::BeginChild("AssetList", ImVec2(0.0f, -160.0f), true);
    for (const AssetInfo& asset : sourceAssets) {
        bool isSelected = state.selectedSourceAssetId.has_value() && *state.selectedSourceAssetId == asset.id;
        std::string label = asset.name + "  [" + asset.type + "]";

        ImGui::PushID(static_cast<int>(asset.id));
        if (ImGui::Selectable(label.c_str(), isSelected)) {
            state.selectedSourceAssetId = asset.id;
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s", asset.path.string().c_str());
        }
        ImGui::PopID();
    }
    ImGui::EndChild();

    ImGui::Separator();

    /// Rename/Move/Delete Fields
    /// ---------------------------------------------

    if (!state.selectedSourceAssetId.has_value()) {
        ImGui::TextDisabled("Select an asset to rename, move, or delete it.");
        ImGui::End();
        return;
    }

    std::optional<AssetInfo> selected = state.assetManager.GetSourceAssetInfo(*state.selectedSourceAssetId);
    if (!selected.has_value()) {
        // Selection no longer exists (e.g. deleted elsewhere).
        state.selectedSourceAssetId.reset();
        ImGui::End();
        return;
    }

    if (buffersSyncedFor != selected->id) {
        CopyToBuffer(movePathBuffer, sizeof(movePathBuffer), selected->path.string());
        std::optional<AssetInfo> primary = FindPrimaryRuntimeAsset(state.assetManager, *selected);
        CopyToBuffer(renameBuffer, sizeof(renameBuffer), primary.has_value() ? primary->name : selected->name);
        buffersSyncedFor = selected->id;
    }

    ImGui::Text("Selected: %s [%s]", selected->name.c_str(), selected->type.c_str());

    // rename
    std::optional<AssetInfo> primaryRuntimeAsset = FindPrimaryRuntimeAsset(state.assetManager, *selected);
    ImGui::SetNextItemWidth(-90.0f);
    ImGui::InputText("##RenamePath", renameBuffer, sizeof(renameBuffer));
    ImGui::SameLine();
    ImGui::BeginDisabled(!primaryRuntimeAsset.has_value());
    if (ImGui::Button("Rename", ImVec2(80.0f, 0.0f)) && renameBuffer[0] != '\0') {
        state.assetManager.RenameAsset(primaryRuntimeAsset->id, renameBuffer);
    }
    ImGui::EndDisabled();
    if (!primaryRuntimeAsset.has_value()) {
        ImGui::TextDisabled("Reimport (see Inspector) to populate rename data.");
    }

    // move
    ImGui::SetNextItemWidth(-90.0f);
    ImGui::InputText("##MovePath", movePathBuffer, sizeof(movePathBuffer));
    ImGui::SameLine();
    if (ImGui::Button("Move", ImVec2(80.0f, 0.0f)) && movePathBuffer[0] != '\0') {
        if (state.assetManager.MoveAsset(selected->sourceId, std::filesystem::path(movePathBuffer))) {
            buffersSyncedFor.reset();
        }
    }

    // delete
    if (ImGui::Button("Delete Asset")) {
        ImGui::OpenPopup("Delete Asset?");
    }
    if (ImGui::BeginPopupModal("Delete Asset?", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Delete '%s' and its underlying file?", selected->name.c_str());
        ImGui::TextDisabled("This cannot be undone.");
        ImGui::Separator();
        if (ImGui::Button("Delete", ImVec2(120.0f, 0.0f))) {
            state.assetManager.DeleteAsset(selected->sourceId);
            state.selectedSourceAssetId.reset();
            buffersSyncedFor.reset();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120.0f, 0.0f))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::End();
}