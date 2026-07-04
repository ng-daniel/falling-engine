#ifndef ENGINE_SERIALIZATION_SERIALIZATION_HELPERS_H
#define ENGINE_SERIALIZATION_SERIALIZATION_HELPERS_H

#include <string>

#include "engine/utils/uuid.h"

std::string ToString(
    UUID);

UUID UUIDFromString(
    std::string_view);

#endif // ENGINE_SERIALIZATION_SERIALIZATION_HELPERS_H