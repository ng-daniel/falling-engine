#pragma once

#include "engine/utils/uuid.h"

class AssetHandle
{
public:
    constexpr explicit AssetHandle(UUID uuid)
        : assetUUID(uuid)
    {}
    constexpr UUID GetUUID() const { return assetUUID; }
private:
    UUID assetUUID;
};
