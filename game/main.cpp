#include <cmath>
#include <stdexcept>

#include "engine/ecs/components/transform.h"
#include "include/generated/asset_ids.h"
#include "engine/core/application.h"
#include "engine/renderer/material_compiler.h"

#include "engine/tools/ecs_extras.h"

#include "engine/debug/logger.h"
#include "engine/utils/random.h"

#include "engine/utils/quaternion.h"

#include "assets/scripts/rotator.h"
#include "assets/scripts/initializer.h"

void PrintModelNodeTree(ModelNode node, int depth = 0) {
    std::string indent(depth * 2, ' ');
    Logger::Info("main", indent + "Node name: " + node.name + ", Mesh ID: " + std::to_string(node.meshId));
    Logger::Info("main", "Position: (" +
        std::to_string(node.relativePosition.x) + ", " +
        std::to_string(node.relativePosition.y) + ", " +
        std::to_string(node.relativePosition.z) + ")");
    Logger::Info("main", indent + "Rotation: (" +
        std::to_string(node.relativeRotation.x) + ", " +
        std::to_string(node.relativeRotation.y) + ", " +
        std::to_string(node.relativeRotation.z) + ")");
    Logger::Info("main", indent + "Scale: (" +
        std::to_string(node.relativeScale.x) + ", " +
        std::to_string(node.relativeScale.y) + ", " +
        std::to_string(node.relativeScale.z) + ")");
    for (auto& child : node.children) {
        PrintModelNodeTree(child, depth + 1);
    }
}

int main() {
    Application app = Application("./game/assets");

    EcsManager& ecsManager = app.GetECSManager();
    Entity * rootEntity = ecsManager.GetEntity(ecsManager.CreateEntity()->entityId);
    ecsManager.SetScene(*rootEntity);
    
    AssetManager& assetManager = app.GetAssetManager();
    ScriptManager& scriptManager = app.GetScriptSystem();


    // float halfXSize = xSize / 2.0f;
    // float halfYSize = ySize / 2.0f;
    // float halfZSize = zSize / 2.0f;
    // for (int x = 0; x < xSize; ++x) {
    //     for (int z = 0; z < zSize; ++z) {
    //         for (int y = 0; y < ySize; ++y) {
    //             Entity * modelRootEntity = ECSExtras::BuildEntityFromModel(app.GetAssetManager(), ecsManager, GameAssets::RYUJIN7_M_MODEL.GetUUID());
    //             ecsManager.Parent(*rootEntity, *modelRootEntity);
    //             Transform  * modelRootTransform = ecsManager.GetComponent<Transform>(*modelRootEntity);
    //             Transform::ChangePosition(
    //                 *modelRootTransform,
    //                 Vector3(static_cast<float>((x - halfXSize) * distance), static_cast<float>((y - halfYSize) * distance), static_cast<float>((z - halfZSize) * distance))
    //             );
    //             Rotator * rotator = scriptManager.AddScript<Rotator>(*modelRootEntity);
    //             Rotator::SetSpeed(*rotator, Random::RandFloat(10.0f, 20.0f));
    //             Rotator::SetHoverOscillationSpeed(*rotator, 1.0f);
    //             Rotator::SetHoverOscillationAmplitude(*rotator, 0.5f);
    //             Rotator::SetRotationAxis(*rotator, Vector3(0.0f, 1.0f, 0.0f));
    //             Rotator::SetInitialTimer(*rotator, float(x + z + y) / 2.0f);
    //         }
    //     }
    // }

    float distance = 1.0f;
    int xSize = 22;
    int ySize = 22;
    int zSize = 21;
    const Entity * newEntity = ecsManager.CreateEntity();
    Initializer::Initialize(
        *scriptManager.AddScript<Initializer>(*newEntity),
        xSize, ySize, zSize, distance,
        GameAssets::RYUJIN7_M_MODEL.GetUUID()
    );
    
    MaterialAsset* modelMaterial = assetManager.RequestAsset<MaterialAsset>(
        GameAssets::RYUJIN7_M_MATERIAL_MATERIAL
    );
    const ShaderAsset* basicVertexShader = assetManager.RequestAssetReadOnly<ShaderAsset>(
        GameAssets::BASICVERT_SHADER
    );
    const ShaderAsset* basicFragmentShader = assetManager.RequestAssetReadOnly<ShaderAsset>(
        GameAssets::BASICFRAG_SHADER
    );
    if (!modelMaterial || !basicVertexShader || !basicFragmentShader) {
        throw std::runtime_error("Failed to load material shader inputs.");
    }
    const ShaderProgramData basicShaderProgram = MaterialShaderService::CompileMaterial(
        *modelMaterial,
        *basicVertexShader,
        *basicFragmentShader
    );
    app.GetRenderer().RegisterShaderProgram(basicShaderProgram);
    Logger::Info("main", "Finished compiling shader program with ID: " + std::to_string(basicShaderProgram.id));


    app.Run();

    return 0;
}
