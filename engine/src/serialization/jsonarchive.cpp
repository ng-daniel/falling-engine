#include <fstream>
#include <vector>

#include "jsonarchive.h"

JsonArchive::JsonArchive()
    : m_Mode(Mode::Writing),
      m_Root(nlohmann::json::object()) {
    m_Stack.push_back(&m_Root);
}

bool JsonArchive::Open(const std::filesystem::path& path) {
    std::ifstream file(path);

    if (!file)
        return false;

    file >> m_Root;

    m_Mode = Mode::Reading;

    m_Stack.clear();
    m_Stack.push_back(&m_Root);

    return true;
}

bool JsonArchive::Save(const std::filesystem::path& path) {
    std::ofstream file(path);

    if (!file)
        return false;

    file << m_Root.dump(4);

    return true;
}

bool JsonArchive::IsReading() const {
    return m_Mode == Mode::Reading;
}

bool JsonArchive::IsWriting() const {
    return m_Mode == Mode::Writing;
}

bool JsonArchive::Contains(std::string_view key) const {
    return m_Stack.back()->contains(key);
}

/*
Read Methods
-----------------
*/

void JsonArchive::Read(std::string_view key, int& value) {
    value = (*m_Stack.back())[key];
}

void JsonArchive::Read(std::string_view key, uint32_t& value) {
    value = (*m_Stack.back())[key];
}

void JsonArchive::Read(std::string_view key, float& value) {
    value = (*m_Stack.back())[key];
}

void JsonArchive::Read(std::string_view key, bool& value) {
    value = (*m_Stack.back())[key];
}

void JsonArchive::Read(std::string_view key, std::string& value) {
    value = (*m_Stack.back())[key];
}

/*
Write Methods
-----------------
*/

void JsonArchive::Write(std::string_view key, int value) {
    (*m_Stack.back())[key] = value;
}

void JsonArchive::Write(std::string_view key, uint32_t value) {
    (*m_Stack.back())[key] = value;
}

void JsonArchive::Write(std::string_view key, float value) {
    (*m_Stack.back())[key] = value;
}

void JsonArchive::Write(std::string_view key, bool value) {
    (*m_Stack.back())[key] = value;
}

void JsonArchive::Write(
    std::string_view key,
    const std::string& value) {
    (*m_Stack.back())[key] = value;
}

void JsonArchive::BeginObject(std::string_view name) {
    if (name.empty())
    {
        return;
    }

    if (IsWriting())
    {
        (*m_Stack.back())[std::string(name)] = nlohmann::json::object();
    }

    m_Stack.push_back(&(*m_Stack.back())[std::string(name)]);
}

void JsonArchive::EndObject() {
    if (m_Stack.size() > 1)
        m_Stack.pop_back();
}