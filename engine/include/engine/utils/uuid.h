#ifndef UUID_H
#define UUID_H

#include <cstdint>
#include <random>

using UUID = std::uint64_t;

class UUIDGenerator {
public:
    static UUID GenerateUUID();

private:
    static std::random_device rd;
    static std::mt19937_64 gen;
    static std::uniform_int_distribution<UUID> dist;
};
#endif // UUID_H