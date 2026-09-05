#include "engine/renderer/material_compiler.h"
#include "engine/debug/logger.h"

#include <cassert>
#include <stdexcept>

namespace {
    MaterialAsset BuildMaterial() {
        MaterialAsset material{};
        material.id = 1;
        material.type = Asset::AssetType::Material;
        return material;
    }

    ShaderAsset BuildShader(UUID id, ShaderAsset::ShaderType type) {
        ShaderAsset shader{};
        shader.id = id;
        shader.type = Asset::AssetType::Shader;
        shader.shaderType = type;
        return shader;
    }

    void TestCompileAttachesRuntimeProgram() {
        MaterialAsset material = BuildMaterial();
        const ShaderAsset vertexShader = BuildShader(2, ShaderAsset::ShaderType::Vertex);
        const ShaderAsset fragmentShader = BuildShader(3, ShaderAsset::ShaderType::Fragment);

        const ShaderProgramData program = MaterialShaderService::CompileMaterial(
            material,
            vertexShader,
            fragmentShader
        );

        assert(program.id != 0);
        assert(program.vertexShaderId == vertexShader.id);
        assert(program.fragmentShaderId == fragmentShader.id);
        assert(program.deviceProgramId == INVALID_SPDEVICE_RID);
        assert(material.runtimeShader == program.id);
    }

    void TestCompileRejectsIncorrectStagesWithoutChangingMaterial() {
        MaterialAsset material = BuildMaterial();
        material.runtimeShader = 42;
        const ShaderAsset fragmentShader = BuildShader(2, ShaderAsset::ShaderType::Fragment);

        bool threw = false;
        try {
            MaterialShaderService::CompileMaterial(material, fragmentShader, fragmentShader);
        } catch (const std::invalid_argument&) {
            threw = true;
        }

        assert(threw);
        assert(material.runtimeShader == 42);
    }
}

int main() {
    Logger::Info("MaterialCompilerTests", "Running material compiler tests...");
    Logger::Info("MaterialCompilerTests", "Running Compile attaches runtime program to material test");
    TestCompileAttachesRuntimeProgram();
    Logger::Info("MaterialCompilerTests", "Running Compile rejects incorrect shader stages without changing material test");
    TestCompileRejectsIncorrectStagesWithoutChangingMaterial();
    Logger::Info("MaterialCompilerTests", "All tests passed.");
}
