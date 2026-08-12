#include "editor/windows/asset_browser.h"
#include "editor/core/editor_state.h"
#include "engine/assets/asset_manager.h"

#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#include <imgui.h>

namespace {
    struct AssetTreeNode {
        std::string label;
        std::filesystem::path path;
        std::vector<AssetTreeNode> children;
        std::optional<AssetInfo> asset; // no for nodes (directories), yes for leafs (assets)

        bool IsDirectory() const {
            return !asset.has_value();
        }


        /**
         * @brief Convert the file path into a list of all the parts of the path ie. partA/partB/partC
         * 
         * @param path 
         * @return std::vector<std::string> 
         */
        static std::vector<std::string> GetPathParts(const std::filesystem::path& path) {
            std::vector<std::string> parts;
            for (const std::filesystem::path& part : path.lexically_normal()) {
                const std::string partString = part.string();
                if (partString.empty() || partString == ".") {
                    continue;
                }
                parts.push_back(partString);
            }
            return parts;
        }

        /**
         * @brief Given a list of source assets, find the longest shared path prefix between them.
         * Used only once to determine the root node of the asset tree.
         * 
         * @param sourceAssets 
         * @return std::vector<std::string> 
         */
        static std::vector<std::string> GetSharedPathPrefix(const std::vector<AssetInfo>& sourceAssets) {
            if (sourceAssets.empty()) {
                return {};
            }

            std::vector<std::string> sharedPrefix = GetPathParts(sourceAssets.front().path);
            for (size_t index = 1; index < sourceAssets.size() && !sharedPrefix.empty(); ++index) {
                const std::vector<std::string> currentParts = GetPathParts(sourceAssets[index].path);
                const size_t matchingCount = std::min(sharedPrefix.size(), currentParts.size());

                size_t prefixLength = 0;
                while (prefixLength < matchingCount && sharedPrefix[prefixLength] == currentParts[prefixLength]) {
                    ++prefixLength;
                }

                sharedPrefix.resize(prefixLength);
            }

            return sharedPrefix;
        }

        /**
         * @brief For a given path and parent, find the directory node in that parent
         * that matches the given path. Create new node if no existing node matches.
         * 
         * @param parent 
         * @param path 
         * @param label 
         * @return AssetTreeNode& 
         */
        static AssetTreeNode& FindOrCreateDirectoryNode(
            AssetTreeNode& parent,
            const std::filesystem::path& path,
            const std::string& label
        ) {
            for (AssetTreeNode& child : parent.children) {
                if (child.IsDirectory() && child.path == path) {
                    return child;
                }
            }

            // no existing matches, create new
            parent.children.push_back(
                    AssetTreeNode{
                        label,
                        path,
                        {},
                        std::nullopt // no asset for directories
                    }
                );
            return parent.children.back();
        }

        /**
         * @brief Recursively sorts the asset tree according to type (dir or asset) and label (alphabetically).
         * 
         * @param node typically root
         */
        static void SortAssetTree(AssetTreeNode& node) {
            // primary sort directories before assets
            // secondary sort alphabetically by label
            std::sort(
                node.children.begin(),
                node.children.end(),
                [](const AssetTreeNode& left, const AssetTreeNode& right) {
                    if (left.IsDirectory() != right.IsDirectory()) {
                        return left.IsDirectory() && !right.IsDirectory();
                    }
                    return left.label < right.label;
                }
            );

            for (AssetTreeNode& child : node.children) {
                if (child.IsDirectory()) {
                    SortAssetTree(child);
                }
            }
        }

        /**
         * @brief Builds an asset tree from a list of source assets.
         * 
         * @param sourceAssets The list of source assets to include in the tree.
         * @return AssetTreeNode The root node of the constructed asset tree.
         */
        static AssetTreeNode BuildAssetTree(const std::filesystem::path& rootPath, const std::vector<AssetInfo>& sourceAssets) {
            
            // initialize root node
            
            AssetTreeNode root;
            root.path = rootPath;
            
            // build tree by iterating through each source asset and creating nodes for each part of its path
            // existing nodes don't get duplicated

            std::vector<std::string> rootParts = GetPathParts(rootPath);
            for (const AssetInfo& asset : sourceAssets) {
                const std::vector<std::string> parts = GetPathParts(asset.path);
                if (parts.empty()) {
                    continue;
                }

                AssetTreeNode* currentNode = &root;
                std::filesystem::path currentPath = rootPath;
                for (size_t index = rootParts.size(); index + 1 < parts.size(); ++index) {
                    currentPath /= parts[index];
                    currentNode = &FindOrCreateDirectoryNode(*currentNode, currentPath, parts[index]);
                }

                std::string label = parts.back() + "  [" + asset.type + "]";
                currentNode->children.push_back(AssetTreeNode{label, asset.path.lexically_normal(), {}, asset});
            }

            SortAssetTree(root);
            return root;
        }

        static void DrawAssetTreeNode(const AssetTreeNode& node, std::optional<UUID>& selectedSourceAssetId) {
            if (!node.IsDirectory()) {
                const AssetInfo& asset = *node.asset;
                const bool isSelected = selectedSourceAssetId.has_value() && *selectedSourceAssetId == asset.id;

                ImGui::PushID(static_cast<int>(asset.id));
                if (ImGui::Selectable(node.label.c_str(), isSelected)) {
                    selectedSourceAssetId = asset.id;
                }
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("%s", asset.path.string().c_str());
                }
                ImGui::PopID();
                return;
            }

            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
            if (node.path == node.path.root_path()) {
                flags |= ImGuiTreeNodeFlags_DefaultOpen;
            }

            const std::string nodeId = node.path.empty() ? std::string("AssetTreeRoot") : node.path.string();
            if (ImGui::TreeNodeEx(nodeId.c_str(), flags, "%s", node.label.c_str())) {
                for (const AssetTreeNode& child : node.children) {
                    DrawAssetTreeNode(child, selectedSourceAssetId);
                }
                ImGui::TreePop();
            }
        }
    };

    
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

    /**
     * @brief Finds the primary runtime sub-asset for a given source asset.
     * 
     * @param assetManager The asset manager to query.
     * @param source The source asset whose primary runtime asset is to be found.
     * @return std::optional<AssetInfo> The primary runtime asset if found, std::nullopt otherwise.
     */
    std::optional<AssetInfo> FindPrimaryRuntimeAsset(AssetManager& assetManager, const AssetInfo& source) {
        // The "primary" runtime sub-asset shares the file's stem as its export name by default
        // (see SourceAssetMetadata::GetPrimaryRuntimeMetadata); it's the one users think of as
        // "the asset" when renaming from the browser.
        
        return assetManager.GetPrimaryRuntimeAssetForSource(source.sourceId);
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

    /// Asset Hierarchy
    /// ---------------------------------------------

    ImGui::TextUnformatted(state.assetManager.GetAssetRoot().string().c_str());

    std::vector<AssetInfo> sourceAssets = state.assetManager.GetAllSourceAssets();
    AssetTreeNode assetTree = AssetTreeNode::BuildAssetTree(state.assetManager.GetAssetRoot(), sourceAssets);
    ImGui::BeginChild("AssetList", ImVec2(0.0f, -160.0f), true);
    for (const AssetTreeNode& child : assetTree.children) {
        AssetTreeNode::DrawAssetTreeNode(child, state.selectedSourceAssetId);
    }
    if (assetTree.children.empty()) {
        ImGui::TextDisabled("No source assets found.");
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
    ImGui::Text("Modify export name:");
    ImGui::SetNextItemWidth(-90.0f);
    ImGui::InputText("##RenamePath", renameBuffer, sizeof(renameBuffer));
    ImGui::SameLine();
    ImGui::BeginDisabled(!primaryRuntimeAsset.has_value());
    if (ImGui::Button("Update", ImVec2(80.0f, 0.0f)) && renameBuffer[0] != '\0') {
        state.assetManager.RenameAsset(primaryRuntimeAsset->id, renameBuffer);
    }
    ImGui::EndDisabled();
    if (!primaryRuntimeAsset.has_value()) {
        ImGui::TextDisabled("Reimport (see Inspector) to populate rename data.");
    }

    // move
    ImGui::Text("Move asset files:");
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