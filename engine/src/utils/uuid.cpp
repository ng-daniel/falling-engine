#include "engine/utils/uuid.h"

std::random_device UUIDGenerator::rd;
std::mt19937_64 UUIDGenerator::gen(rd());
std::uniform_int_distribution<UUID> UUIDGenerator::dist;

UUID UUIDGenerator::GenerateUUID() {
    UUID newUUID;
    do {
        newUUID = dist(gen);
    } while (newUUID == INVALID_UUID);
    return newUUID;
}