#ifndef UUID_H
#define UUID_H

#include <cstdint>
#include <random>

using UUID = std::uint64_t;

class UUIDGenerator {
public:
    UUIDGenerator();
    UUID GenerateUUID();

private:
    std::random_device rd;
    std::mt19937_64 gen;
    std::uniform_int_distribution<UUID> dist;
};
#endif // UUID_H