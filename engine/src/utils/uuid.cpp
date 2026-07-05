#include "engine/utils/uuid.h"

std::random_device UUIDGenerator::rd;
std::mt19937_64 UUIDGenerator::gen(rd());
std::uniform_int_distribution<UUID> UUIDGenerator::dist;

UUID UUIDGenerator::GenerateUUID() {
    return dist(gen);
}