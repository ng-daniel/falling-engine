#pragma once

#include "engine/utils/uuid.h"
#include "engine/serialization/jsonarchive.h"

class UUIDSerializer {
public:
    static void Serialize(JsonArchive& archive, UUID uuid, const std::string& name) {
        archive.Write(name, static_cast<uint64_t>(uuid));
    }

    static void Deserialize(JsonArchive& archive, UUID& uuid, const std::string& name) {
        uint64_t uuidValue;
        archive.Read(name, uuidValue);
        uuid = static_cast<UUID>(uuidValue);
        // no return needed for void function
    }
};