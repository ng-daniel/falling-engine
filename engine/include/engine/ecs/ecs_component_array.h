#pragma once

#include <cstdint>
#include <limits>
#include <algorithm>
#include <string>
#include <vector>
#include <memory>

/**
 * @brief A paged array implementation.
 * 
 * @tparam T 
 * @tparam PAGE_SIZE 
 */
template<typename T, size_t PAGE_SIZE>
class PagedArray {
public:
    explicit PagedArray(T defaultValue = T()) : defaultValue(defaultValue) {}

    /**
     * @brief Primary accessor
     * 
     * @param index 
     * @return T* 
     */
    T* TryGet(size_t index)
    {
        const size_t page = GetPageIndex(index);
        if (page >= pages.size() || !pages[page]) {
            return nullptr;
        }

        return &pages[page][GetPageOffset(index)];
    }
    const T* TryGetReadOnly(size_t index) const
    {
        const size_t page = GetPageIndex(index);
        if (page >= pages.size() || !pages[page]) {
            return nullptr;
        }

        return &pages[page][GetPageOffset(index)];
    }
    /**
     * @brief Manually create a new page, return the index of the new page
     * 
     * @return size_t 
     */
    size_t NewPage()
    {
        size_t newPageIndex = pages.size();
        pages.resize(newPageIndex + 1);
        pages[newPageIndex] = std::make_unique<T[]>(PAGE_SIZE);
        std::fill_n(pages[newPageIndex].get(), PAGE_SIZE, defaultValue);
        return newPageIndex;
    }

    /**
     * @brief Raw accessor, fails if page DNE
     * 
     * @param index 
     * @return T& 
     */
    T& operator[](size_t index)
    {
        const size_t page = GetPageIndex(index);
        const size_t offset = GetPageOffset(index);
        return pages[page][offset];
    }
private:
    T defaultValue;
    // MAIN DATA STORE
    std::vector<std::unique_ptr<T[]>> pages;
    
    size_t GetPageIndex(size_t index) const
    {
        return index / PAGE_SIZE;
    }
    size_t GetPageOffset(size_t index) const
    {
        return index % PAGE_SIZE;
    }
    T* GetOrCreatePage(size_t pageIndex)
    {
        if (pageIndex >= pages.size())
            pages.resize(pageIndex + 1);

        if (!pages[pageIndex]) {
            pages[pageIndex] = std::make_unique<T[]>(PAGE_SIZE);
            std::fill_n(pages[pageIndex].get(), PAGE_SIZE, defaultValue);
        }
        return pages[pageIndex].get();
    }
};

class IEcsComponentArray {
public:
    virtual ~IEcsComponentArray() = default;
    virtual std::string GetComponentName() = 0;
    virtual void DeleteComponent(uint32_t entityIndex) = 0;
};

template <typename T>
class EcsComponentArray : public IEcsComponentArray {
public:
    EcsComponentArray() : entityToDenseMap(TOMBSTONE) {}
    ~EcsComponentArray() = default;

    /**
     * @brief Returns a pointer to the component associated with the given runtimeIdx, or nullptr if it doesn't exist.
     * 
     * @param entityIndex 
     * @return T* 
     */
    T * GetComponent(uint32_t entityRuntimeIdx) {
        const uint32_t * denseIdx = entityToDenseMap.TryGet(entityRuntimeIdx);
        if (!denseIdx) {
            return nullptr;
        }

        if (*denseIdx == TOMBSTONE) {
            return nullptr;
        }

        return &denseArray[*denseIdx];
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
        uint32_t * targetIdx = entityToDenseMap.TryGet(entityRuntimeIdx);
        if (!targetIdx) {
            return;
        }
        if (*targetIdx == TOMBSTONE) {
            return;
        }

        // swap the target idx with the last idx
        uint32_t lastIdx = denseArray.size() - 1;
        if (*targetIdx != lastIdx) {
            denseArray[*targetIdx] = denseArray[lastIdx];

            uint32_t swappedEntityRuntimeIdx = denseToEntityMap[lastIdx];
            denseToEntityMap[*targetIdx] = swappedEntityRuntimeIdx;
            entityToDenseMap[swappedEntityRuntimeIdx] = *targetIdx;
        }

        denseArray.pop_back(); // delete the last idx, which is now our target
        denseToEntityMap.pop_back();
        *targetIdx = TOMBSTONE;
    }

private:
    static constexpr uint32_t PAGE_SIZE = 1024;

    // reserve highest value of uint32_t as a tombstone marker
    static constexpr uint32_t MAX_ENTITIES = std::numeric_limits<uint32_t>::max() - 1;
    
    // reserve highest value of uint32_t as a tombstone marker
    static constexpr uint32_t TOMBSTONE = std::numeric_limits<uint32_t>::max(); 
    
    PagedArray<uint32_t, PAGE_SIZE> entityToDenseMap; // runtimeIdx -> denseIdx
    std::vector<T> denseArray; // denseIdx -> component
    std::vector<uint32_t> denseToEntityMap; // denseIdx -> runtimeIdx
    // need this for the deletion swapping step
};
