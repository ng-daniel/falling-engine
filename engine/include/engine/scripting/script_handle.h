#pragma once

#include <cstdint>

class ScriptManager;

/**
 * @brief Identifies a script instance owned by a ScriptManager.
 * @details This is just a reference, it does not own the script.
 * IDs belong to the manager that created them. Zero means no script was assigned.
 * A nonzero ID does not guarantee that the script is still alive.
 */
class ScriptHandle {
public:
    ScriptHandle() = default;

    uint64_t GetId() const { return id; }
    bool operator==(const ScriptHandle&) const = default;

private:
    friend class ScriptManager;

    explicit ScriptHandle(uint64_t id) : id(id) {}

    uint64_t id = 0;
};
