#include "engine/serialization/serializer.h"
#include "engine/serialization/archive.h"
#include "engine/asset-management/asset_structures.h"

class AssetMetadataSerializer : public Serializer<AssetMetadata> {
public:
    void Serialize(Archive& archive, AssetMetadata& metadata) override;
};