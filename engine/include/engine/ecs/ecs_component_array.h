#pragma once

#include "engine/ecs/components/component.h"
#include "engine/ecs/ecs_structures.h"
#include <cstddef>
#include <cstdint>
#include <limits>
#include <algorithm>
#include <vector>
#include <memory>
#include <utility>

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
        pageCount++;
        return newPageIndex;
    }
    size_t GetPageCount() const {
        return pageCount;
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
        
        // required because PagedArray technically requires random access,
        // because if an older entity gets destroyed it's runtime id is reused
        // so it's not all just push sequentially, even though most of the time it will be
        return GetOrCreatePage(page)[offset];
    }
private:
    T defaultValue;
    size_t pageCount = 0;

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
        if (pageIndex >= pages.size()) {
            pages.resize(pageIndex + 1);
        }
        if (pageIndex >= pageCount) {
            pageCount = pageIndex + 1;
        }
        if (!pages[pageIndex]) {
            pages[pageIndex] = std::make_unique<T[]>(PAGE_SIZE);
            std::fill_n(pages[pageIndex].get(), PAGE_SIZE, defaultValue);
        }
        return pages[pageIndex].get();
    }
};

/**
 * @brief Packed paged storage with stable addresses across additions.
 * Each page is a vector reserved to a fixed capacity, so appending cannot
 * move elements in existing pages.
 */
template <typename T, size_t PAGE_SIZE>
class PagedDenseArray {
public:
    static_assert(PAGE_SIZE > 0);

    template <typename... Args>
    T& EmplaceBack(Args&&... args) {
        const size_t pageIndex = count / PAGE_SIZE;
        if (pageIndex == pages.size()) {
            std::vector<T> page;
            page.reserve(PAGE_SIZE);
            pages.push_back(std::move(page));
        }
        std::vector<T>& page = pages[pageIndex];
        page.emplace_back(std::forward<Args>(args)...);
        ++count;
        return page.back();
    }

    void PopBack() {
        const size_t pageIndex = (count - 1) / PAGE_SIZE;
        pages[pageIndex].pop_back();
        --count;
        if (pages[pageIndex].empty()) {
            pages.pop_back();
        }
    }

    T& operator[](size_t index) {
        return pages[index / PAGE_SIZE][index % PAGE_SIZE];
    }

    const T& operator[](size_t index) const {
        return pages[index / PAGE_SIZE][index % PAGE_SIZE];
    }

    size_t Size() const { return count; }

private:
    size_t count = 0;
    std::vector<std::vector<T>> pages;
};

class IEcsComponentArray {
public:
    virtual ~IEcsComponentArray() = default;
    virtual Component * GetComponent(ECS_RID entityIndex) = 0;
    virtual const Component * GetComponentReadOnly(ECS_RID entityIndex) const = 0;
    virtual Component * CreateComponent(ECS_RID entityIndex) = 0;
    virtual void DeleteComponent(ECS_RID entityIndex) = 0;
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
    T * GetComponent(ECS_RID entityRuntimeIdx) override {
        const uint32_t * denseIdx = entityToDenseMap.TryGet(entityRuntimeIdx);
        if (!denseIdx) {
            return nullptr;
        }

        if (*denseIdx == TOMBSTONE) {
            return nullptr;
        }

        return &denseArray[*denseIdx];
    }
    const T * GetComponentReadOnly(ECS_RID entityRuntimeIdx) const override {
        const uint32_t * denseIdx = entityToDenseMap.TryGetReadOnly(entityRuntimeIdx);
        if (!denseIdx) {
            return nullptr;
        }

        if (*denseIdx == TOMBSTONE) {
            return nullptr;
        }

        return &denseArray[*denseIdx];
    }

    /**
     * @brief Creates a new component associated with the given entityRuntimeIdx.
     * 
     * @param entityRuntimeIdx 
     * @return T* 
     */
    T * CreateComponent(ECS_RID entityRuntimeIdx) override {
        if (entityToDenseMap[entityRuntimeIdx] != TOMBSTONE) {
            return nullptr; // component already exists for this entityRuntimeIdx
        }

        const uint32_t newIdx = static_cast<uint32_t>(denseArray.Size());
        T& component = denseArray.EmplaceBack();
        try {
            denseEntityArray.EmplaceBack(entityRuntimeIdx);
        } catch (...) {
            denseArray.PopBack();
            throw;
        }
        entityToDenseMap[entityRuntimeIdx] = newIdx;
        return &component;
    }

    /**
     * @brief Deletes the target component by removing 
     * 
     * @param entityRuntimeIdx 
     */
    void DeleteComponent(ECS_RID entityRuntimeIdx) override {
        uint32_t * targetIdx = entityToDenseMap.TryGet(entityRuntimeIdx);
        if (!targetIdx) {
            return;
        }
        if (*targetIdx == TOMBSTONE) {
            return;
        }

        // swap the target idx with the last idx
        uint32_t lastIdx = denseArray.Size() - 1;
        if (*targetIdx != lastIdx) {
            denseArray[*targetIdx] = denseArray[lastIdx];

            ECS_RID swappedEntityRuntimeIdx = denseEntityArray[lastIdx];
            denseEntityArray[*targetIdx] = swappedEntityRuntimeIdx;
            entityToDenseMap[swappedEntityRuntimeIdx] = *targetIdx;
        }

        denseArray.PopBack(); // delete the last idx, which is now our target
        denseEntityArray.PopBack();
        *targetIdx = TOMBSTONE;
    }

    T& GetComponentAt(size_t index) {
        return denseArray[index];
    }

    ECS_RID GetEntityRuntimeIdAt(size_t index) const {
        return denseEntityArray[index];
    }

    EntityComponentView<T> GetEntityComponentView() {
        return {
            .array = this,
            .count = denseArray.Size()
        };
    }

private:
    static constexpr uint32_t PAGE_SIZE = 1024;
    static constexpr size_t DENSE_PAGE_SIZE = 256;

    // reserve highest value of uint32_t as a tombstone marker
    static constexpr ECS_RID MAX_ENTITIES = std::numeric_limits<ECS_RID>::max() - 1;

    // reserve highest value of uint32_t as a tombstone marker
    static constexpr uint32_t TOMBSTONE = std::numeric_limits<uint32_t>::max();

    PagedArray<uint32_t, PAGE_SIZE> entityToDenseMap; // runtimeIdx -> denseIdx
    PagedDenseArray<T, DENSE_PAGE_SIZE> denseArray; // denseIdx -> component

    // parallel arrays to map denseArray to entity runtime indices
    PagedDenseArray<ECS_RID, DENSE_PAGE_SIZE> denseEntityArray; // denseIdx -> runtimeIdx
                                            // need this for the deletion swapping step
};
