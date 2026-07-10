#include "engine/serialization/jsonarchive.h"
#include "engine/serialization/asset_uuid_serializer.h"
#include "engine/serialization/serialization_helpers.h"

void AssetUUIDSerializer::Read(UUID& uuid, const std::filesystem::path& uidPath) {
    JsonArchive archive = JsonArchive(JsonArchive::Mode::Reading);
    if (!archive.Open(uidPath)) {
        throw std::runtime_error("Failed to open asset uuid file: " + uidPath.string());
    }

    std::string uid;
    archive.Read("uid", uid);
    uuid = UUIDFromString(uid);
}

void AssetUUIDSerializer::Write(const UUID& uuid, const std::filesystem::path& uidPath) {
    JsonArchive archive = JsonArchive(JsonArchive::Mode::Writing);

    archive.Write("uid", ToString(uuid));
    archive.Save(uidPath); 
}