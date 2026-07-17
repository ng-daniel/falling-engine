#ifndef ENGINE_SERIALIZATION_JSONARCHIVE_H
#define ENGINE_SERIALIZATION_JSONARCHIVE_H

#include <cstddef>
#include <utility>

#include "json.hpp"
#include "archive.h"

class JsonArchive : public Archive
{
public:
    enum class Mode
    {
        Reading,
        Writing
    };

    JsonArchive(Mode mode);

    bool Open(const std::filesystem::path& path) override;
    bool Save(const std::filesystem::path& path) override;

    bool IsReading() const override;
    bool IsWriting() const override;

    bool Contains(std::string_view key) const override;

    void BeginObject(std::string_view name = "") override;
    void EndObject() override;
    void BeginArray(std::string_view name);
    void EndArray();

    std::size_t Size() const;

    void Read(std::string_view key, int& value) override;
    void Read(std::string_view key, uint32_t& value) override;
    void Read(std::string_view key, float& value) override;
    void Read(std::string_view key, bool& value) override;
    void Read(std::string_view key, std::string& value) override;

    template<typename List, typename SerializeElement>
    void ReadList(std::string_view key, List& values, SerializeElement&& serializeElement)
    {
        BeginArray(key);

        const std::size_t count = Size();
        values.clear();

        for (std::size_t index = 0; index < count; ++index)
        {
            typename List::value_type value {};
            BeginObject();
            std::forward<SerializeElement>(serializeElement)(*this, value);
            EndObject();
            values.push_back(std::move(value));
        }

        EndArray();
    }

    void Write(std::string_view key, int value) override;
    void Write(std::string_view key, uint32_t value) override;
    void Write(std::string_view key, float value) override;
    void Write(std::string_view key, bool value) override;
    void Write(std::string_view key, const std::string& value) override;

    template<typename List, typename SerializeElement>
    void WriteList(std::string_view key, const List& values, SerializeElement&& serializeElement)
    {
        BeginArray(key);

        for (const auto& value : values)
        {
            BeginObject();
            std::forward<SerializeElement>(serializeElement)(*this, value);
            EndObject();
        }

        EndArray();
    }

private:
    std::size_t& CurrentArrayIndex();

    Mode m_Mode;
    nlohmann::json m_Root;
    std::vector<nlohmann::json*> m_Stack;
    std::vector<std::size_t> m_ArrayIndices;
};

#endif // ENGINE_SERIALIZATION_JSONARCHIVE_H