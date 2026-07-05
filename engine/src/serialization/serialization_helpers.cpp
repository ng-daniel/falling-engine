#include "engine/serialization/serialization_helpers.h"

std::string ToString(UUID uuid) {
    return std::to_string(uuid);
}

UUID UUIDFromString(std::string_view str) {
    return static_cast<UUID>(std::stoull(std::string(str)));
}