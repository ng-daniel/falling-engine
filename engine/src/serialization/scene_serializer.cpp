#include "engine/serialization/scene_serializer.h"

#include "engine/assets/asset_data.h"
#include "engine/serialization/uuid_serializer.h"

namespace {

    // component field names
    const char * COMPONENT_TYPE_NAME = "componentType";
    const char * COMPONENT_DATA_NAME = "componentData";

    // entity field names
    const char * ENTITY_ID_NAME = "entityId";
    const char * COMPONENTS_ARRAY_NAME = "components";

    // scene field names
    const char * ROOT_ENTITY_NAME = "rootEntity";
    const char * ENTITY_ARRAY_NAME = "entities";

    void SerializeComponentData(JsonArchive& archive, const ComponentData& componentData) {
        archive.BeginObject();
        archive.Write(COMPONENT_TYPE_NAME, componentData.componentType);
        archive.Write(COMPONENT_DATA_NAME, componentData.componentData.dump());
        archive.EndObject();
    }

    ComponentData DeserializeComponentData(JsonArchive& archive) {
        ComponentData componentData;
        archive.Read(COMPONENT_TYPE_NAME, componentData.componentType);

        std::string tempComponentData;
        archive.Read(COMPONENT_DATA_NAME, tempComponentData);
        componentData.componentData = nlohmann::json::parse(tempComponentData);

        return componentData;
    }

    void SerializeEntityData(JsonArchive& archive, const EntityData& entityData) {
        archive.BeginObject();
        UUIDSerializer::Serialize(archive, entityData.entityId, ENTITY_ID_NAME);
        archive.BeginArray(COMPONENTS_ARRAY_NAME);
        for (const auto& componentData : entityData.components) {
            SerializeComponentData(archive, componentData);
        }
        archive.EndArray();
        archive.EndObject();
    }

    EntityData DeserializeEntityData(JsonArchive& archive) {
        EntityData entityData;
        entityData.entityId = UUIDSerializer::Deserialize(archive, ENTITY_ID_NAME);
        for (const auto& componentDataJson : archive.GetArray(COMPONENTS_ARRAY_NAME)) {
            entityData.components.push_back(DeserializeComponentData(archive));
        }
        return entityData;
    }
}

void SceneSerializer::Serialize(JsonArchive& archive, const SceneAsset& scene, std::string_view name) {
    UUIDSerializer::Serialize(archive, scene.rootEntity, ROOT_ENTITY_NAME);
    archive.BeginArray(ENTITY_ARRAY_NAME);
    for (const auto& entityData : scene.entities) {
        archive.BeginObject();
        UUIDSerializer::Serialize(archive, entityData.entityId, ENTITY_ID_NAME);
        archive.BeginArray(COMPONENTS_ARRAY_NAME);
        for (const auto& componentData : entityData.components) {
            SerializeComponentData(archive, componentData);
        }
        archive.EndArray();
        archive.EndObject();
    }
    archive.EndArray();
}

SceneAsset SceneSerializer::Deserialize(JsonArchive& archive, std::string_view name) {
    SceneAsset scene;

    scene.rootEntity = UUIDSerializer::Deserialize(archive, ROOT_ENTITY_NAME);
    for (const auto& entityDataJson : archive.GetArray(ENTITY_ARRAY_NAME)) {
        
        // build temporary archive
        JsonArchive entityDataJsonArchive(JsonArchive::Mode::Reading);
        entityDataJsonArchive.OpenFromMemory(entityDataJson);
        scene.entities.push_back(DeserializeEntityData(entityDataJsonArchive));
    }
    return scene;
}