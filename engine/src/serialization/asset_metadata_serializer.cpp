#include "engine/serialization/asset_metadata_serializer.h"
#include "engine/serialization/serialization_helpers.h"

void AssetMetadataSerializer::Serialize(Archive& archive, AssetMetadata& metadata) {
    if (archive.IsReading()) {
        std::string uid;
        std::string type;
        std::string importer;
        std::string path;

        archive.Read("uid", uid);
        archive.Read("assetType", type);
        archive.Read("importer", importer);
        archive.Read("path", path);

        metadata.id = UUIDFromString(uid);
        metadata.type = type;
        metadata.importer = importer;
        metadata.path = path;
    }
    else {
        archive.Write("uid", ToString(metadata.id));
        archive.Write("assetType", metadata.type);
        archive.Write("importer", metadata.importer);
        archive.Write("path", metadata.path);
    }
}