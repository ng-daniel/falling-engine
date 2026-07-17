#include <fstream>
#include <vector>

#include "engine/serialization/jsonarchive.h"

JsonArchive::JsonArchive(Mode mode)
    : m_Mode(mode),
    m_Root(nlohmann::json::object()) {
    m_Stack.push_back(&m_Root);
}

bool JsonArchive::Open(const std::filesystem::path& path) {
    std::ifstream file(path);

    if (!file)
        return false;

    file >> m_Root;

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

std::size_t JsonArchive::GetSizeWhenInArray() const {
    return m_Stack.back()->size();
}

std::size_t& JsonArchive::CurrentArrayIndex() {
    return m_ArrayIndices.back();
}

/*
Read Methods
-----------------
*/

void JsonArchive::Read(std::string_view key, int& value) {
    m_Stack.back()->at(std::string(key)).get_to(value);
}

void JsonArchive::Read(std::string_view key, uint32_t& value) {
    m_Stack.back()->at(std::string(key)).get_to(value);
}

void JsonArchive::Read(std::string_view key, float& value) {
    m_Stack.back()->at(std::string(key)).get_to(value);
}

void JsonArchive::Read(std::string_view key, bool& value) {
    m_Stack.back()->at(std::string(key)).get_to(value);
}

void JsonArchive::Read(std::string_view key, std::string& value) {
    m_Stack.back()->at(std::string(key)).get_to(value);
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
        if (!m_Stack.back()->is_array())
        {
            return;
        }

        if (IsWriting())
        {
            m_Stack.back()->push_back(nlohmann::json::object());
            m_Stack.push_back(&m_Stack.back()->back());
            return;
        }

        nlohmann::json& value = (*m_Stack.back())[CurrentArrayIndex()++];
        m_Stack.push_back(&value);
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

void JsonArchive::BeginArray(std::string_view name) {
    if (name.empty())
    {
        return;
    }

    if (IsWriting())
    {
        (*m_Stack.back())[std::string(name)] = nlohmann::json::array();
    }

    m_Stack.push_back(&(*m_Stack.back())[std::string(name)]);
    m_ArrayIndices.push_back(0);
}

void JsonArchive::EndArray() {
    if (m_ArrayIndices.empty())
    {
        return;
    }

    m_ArrayIndices.pop_back();

    if (m_Stack.size() > 1)
    {
        m_Stack.pop_back();
    }
}