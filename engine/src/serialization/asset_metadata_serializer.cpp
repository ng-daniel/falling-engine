#include "engine/serialization/jsonarchive.h"
#include "engine/serialization/asset_metadata_serializer.h"
#include "engine/serialization/serialization_helpers.h"

// shared names
const std::string UUID_FIELD_NAME = "UUID";
const std::string ASSET_TYPE_FIELD_NAME = "assetType";
const std::string PATH_FIELD_NAME = "path";

// source asset metadata specific names
const std::string RUNTIME_ASSETS_FIELD_NAME = "runtimeAssets";

// runtime asset metadata specific names
const std::string SOURCE_ID_FIELD_NAME = "sourceUUID";

void ReadRuntimeMetadata(JsonArchive& archive, RuntimeAssetMetadata& metadata);
void WriteRuntimeMetadata(JsonArchive& archive, const RuntimeAssetMetadata& metadata);

void AssetMetadataSerializer::Read(SourceAssetMetadata& metadata, const std::filesystem::path& metaPath) {
    JsonArchive archive = JsonArchive(JsonArchive::Mode::Reading);
    if (!archive.Open(metaPath)) {
        throw std::runtime_error("Failed to open asset metadata file: " + metaPath.string());
    }

    std::string id;
    std::string type;
    std::string assetPath;

    archive.Read(UUID_FIELD_NAME, id);
    archive.Read(ASSET_TYPE_FIELD_NAME, type);
    archive.Read(PATH_FIELD_NAME, assetPath);
    
    metadata.id = UUIDFromString(id);
    metadata.type = type;
    metadata.path = assetPath;

    // read runtime asset array

    archive.BeginArray(RUNTIME_ASSETS_FIELD_NAME);
    for (std::size_t index = 0; index < archive.GetSizeWhenInArray(); index++) {
        archive.BeginObject();

        RuntimeAssetMetadata runtimeAssetMetadata;
        ReadRuntimeMetadata(archive, runtimeAssetMetadata);
        metadata.assetMetadatas.push_back(runtimeAssetMetadata);
        
        archive.EndObject();
    }
    archive.EndArray();
}

void AssetMetadataSerializer::Write(const SourceAssetMetadata& metadata, const std::filesystem::path& metaPath) {
    JsonArchive archive = JsonArchive(JsonArchive::Mode::Writing);

    archive.Write(UUID_FIELD_NAME, ToString(metadata.id));
    archive.Write(ASSET_TYPE_FIELD_NAME, metadata.type);
    archive.Write(PATH_FIELD_NAME, metadata.path);

    // write runtime asset array

    archive.BeginArray(RUNTIME_ASSETS_FIELD_NAME);
    for (const auto& runtimeAssetMetadata : metadata.assetMetadatas) {
        archive.BeginObject();
        WriteRuntimeMetadata(archive, runtimeAssetMetadata);
        archive.EndObject();
    }
    archive.EndArray();

    archive.Save(metaPath);
}

void ReadRuntimeMetadata(JsonArchive& archive, RuntimeAssetMetadata& metadata) {
    std::string id;
    std::string type;
    std::string assetPath;
    std::string sourceId;

    archive.Read(UUID_FIELD_NAME, id);
    archive.Read(ASSET_TYPE_FIELD_NAME, type);
    archive.Read(PATH_FIELD_NAME, assetPath);
    archive.Read(SOURCE_ID_FIELD_NAME, sourceId);

    metadata.id = UUIDFromString(id);
    metadata.type = type;
    metadata.path = assetPath;
    metadata.sourceId = UUIDFromString(sourceId);
}

void WriteRuntimeMetadata(JsonArchive& archive, const RuntimeAssetMetadata& metadata) {
    archive.Write(UUID_FIELD_NAME, ToString(metadata.id));
    archive.Write(ASSET_TYPE_FIELD_NAME, metadata.type);
    archive.Write(PATH_FIELD_NAME, metadata.path);
    archive.Write(SOURCE_ID_FIELD_NAME, ToString(metadata.sourceId));
}