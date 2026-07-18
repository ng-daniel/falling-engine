#ifndef ENGINE_ASSETS_ASSET_HANDLE_H
#define ENGINE_ASSETS_ASSET_HANDLE_H

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

#endif // ENGINE_ASSETS_ASSET_HANDLE_H