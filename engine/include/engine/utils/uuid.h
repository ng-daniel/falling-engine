#ifndef UUID_H
#define UUID_H

#include <cstdint>
#include <random>

using UUID = std::uint64_t;

inline std::string UUIDToString(UUID uuid) {
    return std::to_string(uuid);
}

inline UUID StringToUUID(const std::string& uuidStr) {
    return static_cast<UUID>(std::stoull(uuidStr));
}

class UUIDGenerator {
public:
    static UUID GenerateUUID();

private:
    static std::random_device rd;
    static std::mt19937_64 gen;
    static std::uniform_int_distribution<UUID> dist;
};
#endif // UUID_H