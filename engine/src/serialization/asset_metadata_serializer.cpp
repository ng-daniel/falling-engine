#include "engine/serialization/jsonarchive.h"
#include "engine/serialization/asset_metadata_serializer.h"

void AssetMetadataSerializer::Read(AssetMetadata& metadata, const std::filesystem::path& metaPath) {
    JsonArchive archive = JsonArchive(JsonArchive::Mode::Reading);
    if (!archive.Open(metaPath)) {
        throw std::runtime_error("Failed to open asset metadata file: " + metaPath.string());
    }

    std::string type;
    std::string importer;
    std::string assetPath;

    archive.Read("assetType", type);
    archive.Read("importer", importer);
    archive.Read("path", assetPath);

    metadata.type = type;
    metadata.importer = importer;
    metadata.path = assetPath;
}

void AssetMetadataSerializer::Write(const AssetMetadata& metadata, const std::filesystem::path& metaPath) {
    JsonArchive archive = JsonArchive(JsonArchive::Mode::Writing);

    archive.Write("assetType", metadata.type);
    archive.Write("importer", metadata.importer);
    archive.Write("path", metadata.path);

    archive.Save(metaPath);
}