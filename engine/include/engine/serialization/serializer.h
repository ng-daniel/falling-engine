#pragma once

#include <filesystem>

template<typename T>
class Serializer
{
public:
    virtual ~Serializer() = default;
    virtual void Read(T& object, const std::filesystem::path& path) = 0;
    virtual void Write(const T& object, const std::filesystem::path& path) = 0;
};
