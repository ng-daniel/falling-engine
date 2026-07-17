#include "engine/serialization/jsonarchive.h"
#include "engine/serialization/asset_metadata_serializer.h"
#include "engine/serialization/serialization_helpers.h"

void AssetMetadataSerializer::Read(SourceAssetMetadata& metadata, const std::filesystem::path& metaPath) {
    JsonArchive archive = JsonArchive(JsonArchive::Mode::Reading);
    if (!archive.Open(metaPath)) {
        throw std::runtime_error("Failed to open asset metadata file: " + metaPath.string());
    }
    std::string id;
    std::string type;
    std::string assetPath;

    archive.Read("uuid", id);
    archive.Read("assetType", type);
    archive.Read("path", assetPath);

    metadata.id = UUIDFromString(id);
    metadata.type = type;
    metadata.path = assetPath;
}

void AssetMetadataSerializer::Write(const SourceAssetMetadata& metadata, const std::filesystem::path& metaPath) {
    JsonArchive archive = JsonArchive(JsonArchive::Mode::Writing);

    archive.Write("uuid", ToString(metadata.id));
    archive.Write("assetType", metadata.type);
    archive.Write("path", metadata.path);

    archive.Save(metaPath);
}