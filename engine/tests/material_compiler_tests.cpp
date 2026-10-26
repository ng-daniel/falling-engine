#include "engine/renderer/material_compiler.h"
#include "engine/assets/asset_helpers.h"
#include "engine/debug/logger.h"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>

namespace {
    // Runtime asset UUIDs that the importers assigned to the files in TestAssetRoot().
    struct TestAssetIds {
        UUID material = INVALID_UUID;
        UUID vertexShader = INVALID_UUID;
        UUID fragmentShader = INVALID_UUID;
    };

    void WriteFile(const std::filesystem::path& path, const std::string& contents) {
        std::ofstream file(path);
        file << contents;
    }

    // AssetManager only loads assets from disk, so each run starts from a fresh root
    // containing a minimal glTF (one material, one empty scene) and a shader per stage.
    std::filesystem::path TestAssetRoot() {
        const std::filesystem::path root = std::filesystem::temp_directory_path() / "falling_engine_material_compiler_tests";
        std::filesystem::remove_all(root);
        std::filesystem::create_directories(root);

        WriteFile(root / "lit.vert", "#version 330 core\nvoid main() {}\n");
        WriteFile(root / "lit_surface.frag", "#version 330 core\nvoid main() {}\n");
        WriteFile(
            root / "model.gltf",
            R"({"asset":{"version":"2.0"},"scene":0,"scenes":[{}],"materials":[{"name":"surface"}]})"
        );
        return root;
    }

    UUID FindRuntimeAssetId(AssetManager& assets, const std::string& sourceFileName, Asset::AssetType type) {
        for (const AssetInfo& source : assets.GetAllSourceAssets()) {
            if (source.path.filename() != sourceFileName) {
                continue;
            }
            // Runtime metadata is only registered once the source has been imported.
            assets.ReimportAsset(source.id);
            for (const AssetInfo& runtime : assets.GetRuntimeAssetsForSource(source.id)) {
                if (runtime.type == GetStringFromAssetType(type)) {
                    return runtime.id;
                }
            }
        }
        return INVALID_UUID;
    }

    TestAssetIds ImportTestAssets(AssetManager& assets) {
        TestAssetIds ids;
        ids.material = FindRuntimeAssetId(assets, "model.gltf", Asset::AssetType::Material);
        ids.vertexShader = FindRuntimeAssetId(assets, "lit.vert", Asset::AssetType::Shader);
        ids.fragmentShader = FindRuntimeAssetId(assets, "lit_surface.frag", Asset::AssetType::Shader);
        assert(ids.material != INVALID_UUID);
        assert(ids.vertexShader != INVALID_UUID);
        assert(ids.fragmentShader != INVALID_UUID);
        return ids;
    }

    void TestCompileAttachesRuntimeProgram() {
        AssetManager assets(TestAssetRoot());
        const TestAssetIds ids = ImportTestAssets(assets);

        const ShaderProgramData program = MaterialShaderService::CompileMaterial(
            ids.material,
            ids.vertexShader,
            ids.fragmentShader,
            assets
        );

        assert(program.id != 0);
        assert(program.vertexShaderId == ids.vertexShader);
        assert(program.fragmentShaderId == ids.fragmentShader);
        assert(program.deviceProgramId == INVALID_SPDEVICE_RID);
        assert(assets.RequestAsset<MaterialAsset>(ids.material)->runtimeShader == program.id);
    }

    void TestCompileRejectsIncorrectStagesWithoutChangingMaterial() {
        AssetManager assets(TestAssetRoot());
        const TestAssetIds ids = ImportTestAssets(assets);
        assets.RequestAsset<MaterialAsset>(ids.material)->runtimeShader = 42;

        bool threw = false;
        try {
            MaterialShaderService::CompileMaterial(ids.material, ids.fragmentShader, ids.fragmentShader, assets);
        } catch (const std::invalid_argument&) {
            threw = true;
        }

        assert(threw);
        assert(assets.RequestAsset<MaterialAsset>(ids.material)->runtimeShader == 42);
    }

    void TestCompileRejectsMissingOrMistypedAssets() {
        AssetManager assets(TestAssetRoot());
        const TestAssetIds ids = ImportTestAssets(assets);
        assets.RequestAsset<MaterialAsset>(ids.material)->runtimeShader = 42;

        const auto throwsRuntimeError = [&](UUID material, UUID vertexShader, UUID fragmentShader) {
            try {
                MaterialShaderService::CompileMaterial(material, vertexShader, fragmentShader, assets);
            } catch (const std::runtime_error&) {
                return true;
            }
            return false;
        };

        assert(throwsRuntimeError(INVALID_UUID, ids.vertexShader, ids.fragmentShader));
        assert(throwsRuntimeError(ids.material, INVALID_UUID, ids.fragmentShader));
        assert(throwsRuntimeError(ids.material, ids.vertexShader, INVALID_UUID));
        // A shader UUID passed as the material is rejected by the asset type check.
        assert(throwsRuntimeError(ids.vertexShader, ids.vertexShader, ids.fragmentShader));
        assert(assets.RequestAsset<MaterialAsset>(ids.material)->runtimeShader == 42);
    }
}

int main() {
    Logger::Info("MaterialCompilerTests", "Running material compiler tests...");
    Logger::Info("MaterialCompilerTests", "Running Compile attaches runtime program to material test");
    TestCompileAttachesRuntimeProgram();
    Logger::Info("MaterialCompilerTests", "Running Compile rejects incorrect shader stages without changing material test");
    TestCompileRejectsIncorrectStagesWithoutChangingMaterial();
    Logger::Info("MaterialCompilerTests", "Running Compile rejects missing or mistyped assets test");
    TestCompileRejectsMissingOrMistypedAssets();
    Logger::Info("MaterialCompilerTests", "All tests passed.");
}
