#ifndef ENGINE_SERIALIZATION_ARCHIVE_H
#define ENGINE_SERIALIZATION_ARCHIVE_H

#include <string>
#include <filesystem>

/**
 * @brief Abstract interface for serialization archivers.
 */
class Archive
{
public:

    virtual ~Archive() = default;

    virtual bool Open(const std::filesystem::path& path) = 0;
    virtual bool Save(const std::filesystem::path& path) = 0;

    virtual bool IsReading() const = 0;
    virtual bool IsWriting() const = 0;

    virtual bool Contains(std::string_view key) const = 0;

    virtual void BeginObject(std::string_view name = "") = 0;
    virtual void EndObject() = 0;

    virtual void Read(std::string_view key, int& value) = 0;
    virtual void Read(std::string_view key, uint32_t& value) = 0;
    virtual void Read(std::string_view key, float& value) = 0;
    virtual void Read(std::string_view key, bool& value) = 0;
    virtual void Read(std::string_view key, std::string& value) = 0;

    virtual void Write(std::string_view key, int value) = 0;
    virtual void Write(std::string_view key, uint32_t value) = 0;
    virtual void Write(std::string_view key, float value) = 0;
    virtual void Write(std::string_view key, bool value) = 0;
    virtual void Write(std::string_view key, const std::string& value) = 0;
};

#endif // ENGINE_SERIALIZATION_ARCHIVE_H