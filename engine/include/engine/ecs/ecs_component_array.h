#ifndef ENGINE_ECS_ECS_COMPONENT_ARRAY_H
#define ENGINE_ECS_ECS_COMPONENT_ARRAY_H

#include <cstdint>
#include <string>
#include <limits>
#include <vector>

// just a flag
class IEcsComponentArray {
public:
    virtual std::string GetComponentName() = 0;
};

template <typename T>
class EcsComponentArray : public IEcsComponentArray {
public:
    EcsComponentArray() {
        entityToDenseMap.reserve(MAX_SIZE);
        denseToEntityMap.reserve(MAX_SIZE);
        std::fill(entityToDenseMap.begin(), entityToDenseMap.end(), TOMBSTONE);
        std::fill(denseToEntityMap.begin(), denseToEntityMap.end(), TOMBSTONE);
    }
    ~EcsComponentArray() = default;

    /**
     * @brief Returns a pointer to the component associated with the given runtimeIdx, or nullptr if it doesn't exist.
     * 
     * @param entityIndex 
     * @return T* 
     */
    T * GetComponent(uint32_t entityIndex) {
        if (entityIndex >= entityToDenseMap.size()) {
            return nullptr;
        }

        uint32_t denseIdx = entityToDenseMap[entityIndex];
        if (denseIdx == TOMBSTONE) {
            return nullptr;
        }

        return &denseArray[denseIdx];
    }
    const T * GetComponentReadOnly(uint32_t entityIndex) {
        return GetComponent(entityIndex);
    }

    /**
     * @brief Creates a new component associated with the given entityIndex.
     * 
     * @param entityIndex 
     * @return T* 
     */
    T * NewComponent(uint32_t entityIndex) {
        if (entityIndex >= entityToDenseMap.size()) {
            return nullptr;
        }
        if (entityToDenseMap[entityIndex] != TOMBSTONE) {
            return nullptr; // component already exists for this entityIndex
        }

        // add component
        denseArray.push_back(T());
        uint32_t newIdx = denseArray.size() - 1;
        
        // update mappings
        entityToDenseMap[entityIndex] = newIdx;
        denseToEntityMap.push_back(entityIndex);

        return &denseArray[newIdx];
    }

    /**
     * @brief Deletes the target component by removing 
     * 
     * @param entityIndex 
     */
    void DeleteComponent(uint32_t entityIndex) {
        if (entityIndex >= entityToDenseMap.size()) {
            return;
        }
        uint32_t targetIdx = entityToDenseMap[entityIndex];
        if (targetIdx == TOMBSTONE) {
            return;
        }

        // swap the target idx with the last idx
        uint32_t lastIdx = denseArray.size() - 1;
        denseArray[targetIdx] = denseArray[lastIdx];
        denseArray.pop_back(); // delete the last idx, which is now our target

        // don't forget to also swap the denseToEntityMap
        denseToEntityMap[targetIdx] = denseToEntityMap[lastIdx];
        denseToEntityMap.pop_back();

        // update the entityToDenseMap for the swapped entity
        uint32_t swappedEntityIndex = denseToEntityMap[targetIdx];
        entityToDenseMap[swappedEntityIndex] = targetIdx;
        entityToDenseMap[entityIndex] = TOMBSTONE;
    }

    /**
     * @brief Gets the name of the component based on the component type T
     * @return std::string 
     */
    std::string GetComponentName() {
        
    }

private:
    static constexpr uint32_t MAX_SIZE = std::numeric_limits<uint32_t>::max();
    
    // reserve highest value of uint32_t as a tombstone marker
    static constexpr uint32_t TOMBSTONE = std::numeric_limits<uint32_t>::max(); 
    
    std::vector<uint32_t> entityToDenseMap; // runtimeIdx -> denseIdx 
    std::vector<T> denseArray; // denseIdx -> component
    std::vector<uint32_t> denseToEntityMap; // denseIdx -> runtimeIdx
    // need this for the deletion swapping step
};

#endif // ENGINE_ECS_ECS_COMPONENT_ARRAY_H