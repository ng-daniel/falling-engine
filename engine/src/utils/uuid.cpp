#include "engine/utils/uuid.h"

UUIDGenerator::UUIDGenerator() : gen(rd()) {}

UUID UUIDGenerator::GenerateUUID() {
    return dist(gen);
}