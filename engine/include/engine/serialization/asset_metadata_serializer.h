#ifndef ENGINE_SERIALIZATION_ASSET_METADATA_SERIALIZER_H
#define ENGINE_SERIALIZATION_ASSET_METADATA_SERIALIZER_H

#include "engine/asset-management/asset_structures.h"

#include "engine/serialization/serializer.h"

class AssetMetadataSerializer : public Serializer<AssetMetadata> {
public:
    void Read(AssetMetadata& metadata, const std::filesystem::path& metaPath) override;
    void Write(const AssetMetadata& metadata, const std::filesystem::path& metaPath) override;
};

#endif // ENGINE_SERIALIZATION_ASSET_METADATA_SERIALIZER_H