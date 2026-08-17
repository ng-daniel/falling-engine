#ifndef ENGINE_ECS_ECS_COMPONENT_ARRAY_H
#define ENGINE_ECS_ECS_COMPONENT_ARRAY_H

#include <cstdint>
#include <limits>
#include <string>
#include <vector>

class IEcsComponentArray {
public:
    virtual ~IEcsComponentArray() = default;
    virtual std::string GetComponentName() = 0;
    virtual void DeleteComponent(uint32_t entityIndex) = 0;
};

template <typename T>
class EcsComponentArray : public IEcsComponentArray {
public:
    EcsComponentArray() {
        entityToDenseMap.resize(MAX_SIZE, TOMBSTONE);
        denseArray.reserve(MAX_SIZE);
        denseToEntityMap.reserve(MAX_SIZE);
    }
    ~EcsComponentArray() = default;

    /**
     * @brief Returns a pointer to the component associated with the given runtimeIdx, or nullptr if it doesn't exist.
     * 
     * @param entityIndex 
     * @return T* 
     */
    T * GetComponent(uint32_t entityRuntimeIdx) {
        if (entityRuntimeIdx >= entityToDenseMap.size()) {
            return nullptr;
        }

        uint32_t denseIdx = entityToDenseMap[entityRuntimeIdx];
        if (denseIdx == TOMBSTONE) {
            return nullptr;
        }

        return &denseArray[denseIdx];
    }
    const T * GetComponentReadOnly(uint32_t entityRuntimeIdx) {
        return GetComponent(entityRuntimeIdx);
    }

    /**
     * @brief Creates a new component associated with the given entityRuntimeIdx.
     * 
     * @param entityRuntimeIdx 
     * @return T* 
     */
    T * NewComponent(uint32_t entityRuntimeIdx) {
        if (entityRuntimeIdx >= MAX_SIZE) {
            return nullptr;
        }
        if (entityToDenseMap[entityRuntimeIdx] != TOMBSTONE) {
            return nullptr; // component already exists for this entityRuntimeIdx
        }

        // add component
        denseArray.push_back(T());
        uint32_t newIdx = denseArray.size() - 1;
        
        // update mappings
        entityToDenseMap[entityRuntimeIdx] = newIdx;
        denseToEntityMap.push_back(entityRuntimeIdx);

        return &denseArray[newIdx];
    }

    /**
     * @brief Deletes the target component by removing 
     * 
     * @param entityRuntimeIdx 
     */
    void DeleteComponent(uint32_t entityRuntimeIdx) {
        if (entityRuntimeIdx >= entityToDenseMap.size()) {
            return;
        }
        uint32_t targetIdx = entityToDenseMap[entityRuntimeIdx];
        if (targetIdx == TOMBSTONE) {
            return;
        }

        // swap the target idx with the last idx
        uint32_t lastIdx = denseArray.size() - 1;
        if (targetIdx != lastIdx) {
            denseArray[targetIdx] = denseArray[lastIdx];

            uint32_t swappedEntityRuntimeIdx = denseToEntityMap[lastIdx];
            denseToEntityMap[targetIdx] = swappedEntityRuntimeIdx;
            entityToDenseMap[swappedEntityRuntimeIdx] = targetIdx;
        }

        denseArray.pop_back(); // delete the last idx, which is now our target
        denseToEntityMap.pop_back();
        entityToDenseMap[entityRuntimeIdx] = TOMBSTONE;
    }

private:
    static constexpr uint32_t MAX_SIZE = 1024;

    // reserve highest value of uint32_t as a tombstone marker
    static constexpr uint32_t TOMBSTONE = std::numeric_limits<uint32_t>::max(); 
    
    std::vector<uint32_t> entityToDenseMap; // runtimeIdx -> denseIdx 
    std::vector<T> denseArray; // denseIdx -> component
    std::vector<uint32_t> denseToEntityMap; // denseIdx -> runtimeIdx
    // need this for the deletion swapping step
};

#endif // ENGINE_ECS_ECS_COMPONENT_ARRAY_H