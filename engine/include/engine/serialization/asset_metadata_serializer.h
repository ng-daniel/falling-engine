#ifndef ENGINE_SERIALIZATION_ASSET_METADATA_SERIALIZER_H
#define ENGINE_SERIALIZATION_ASSET_METADATA_SERIALIZER_H

#include "engine/assets/asset_data.h"
#include "serializer.h"

class AssetMetadataSerializer : public Serializer<SourceAssetMetadata> {
public:
    void Read(SourceAssetMetadata& metadata, const std::filesystem::path& metaPath) override;
    void Write(const SourceAssetMetadata& metadata, const std::filesystem::path& metaPath) override;
};

#endif // ENGINE_SERIALIZATION_ASSET_METADATA_SERIALIZER_H