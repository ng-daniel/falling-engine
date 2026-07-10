#ifndef ENGINE_SERIALIZATION_ASSET_UUID_SERIALIZER_H
#define ENGINE_SERIALIZATION_ASSET_UUID_SERIALIZER_H

#include "engine/utils/uuid.h"
#include "serializer.h"

class AssetUUIDSerializer : public Serializer<UUID> {
public:
    void Read(UUID& uuid, const std::filesystem::path& metaPath) override;
    void Write(const UUID& uuid, const std::filesystem::path& metaPath) override;
};

#endif // ENGINE_SERIALIZATION_ASSET_UUID_SERIALIZER_H