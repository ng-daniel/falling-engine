#ifndef ENGINE_SERIALIZATION_JSONARCHIVE_H
#define ENGINE_SERIALIZATION_JSONARCHIVE_H

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

    void Read(std::string_view key, int& value) override;
    void Read(std::string_view key, uint32_t& value) override;
    void Read(std::string_view key, float& value) override;
    void Read(std::string_view key, bool& value) override;
    void Read(std::string_view key, std::string& value) override;

    void Write(std::string_view key, int value) override;
    void Write(std::string_view key, uint32_t value) override;
    void Write(std::string_view key, float value) override;
    void Write(std::string_view key, bool value) override;
    void Write(std::string_view key, const std::string& value) override;

private:
    Mode m_Mode;
    nlohmann::json m_Root;
    std::vector<nlohmann::json*> m_Stack;
};

#endif // ENGINE_SERIALIZATION_JSONARCHIVE_H