#pragma once

#include "engine/ecs/ecs_runtime_id.h"
#include "engine/serialization/jsonarchive.h"
#include "engine/utils/uuid.h"
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <tuple>

/**
 * @brief Just a marker for components
 * 
 */
struct IComponent {
    static void Serialize(JsonArchive& archive, const IComponent& component);
    static IComponent Deserialize(JsonArchive& archive, IComponent& component);
    virtual std::string GetType() const = 0;
};

/**
 * @brief Represents an entity in a scene
 */
struct Entity {
    UUID entityId;
    ECS_RID entityRuntimeIdx; // used for runtime indexing into ECS component arrays
    std::string name;

    void SetName(const std::string& newName) {
        name = newName;
    }
};

/**
 * @brief Return structure for iterating over entity-component view requests
 * @tparam T
 */
template <typename T>
struct EntityComponentView {
    struct Iterator {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = std::tuple<ECS_RID&, T&>;

        const EntityComponentView* view;
        size_t index;

        value_type operator*() const {
            return {view->entityRuntimeIds[index], view->components[index]};
        }

        Iterator& operator++() {
            ++index;
            return *this;
        }

        Iterator operator++(int) {
            Iterator previous = *this;
            ++(*this);
            return previous;
        }

        bool operator==(const Iterator& other) const {
            return view == other.view && index == other.index;
        }

        bool operator!=(const Iterator& other) const {
            return !(*this == other);
        }
    };

    Iterator begin() const { return {this, 0}; }
    Iterator end() const { return {this, count}; }
    size_t size() const { return count; }
    bool empty() const { return count == 0; }

    ECS_RID* entityRuntimeIds = nullptr;
    T* components = nullptr;
    size_t count = 0;
};
