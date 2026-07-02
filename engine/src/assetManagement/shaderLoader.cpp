#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "shaderLoader.h"

/// @brief Loads the shader files and returns the shader data.
/// Group them together because vertex and fragment need to be together to make shader program.
/// @param vertexShaderPath Path to the vertex shader file.
/// @param fragmentShaderPath Path to the fragment shader file.
/// @param geometryShaderPath Path to the geometry shader file (optional).
/// @return ShaderData containing the loaded shader sources.
ShaderData ShaderLoader::LoadShader(const std::string & vertexShaderPath, const std::string & fragmentShaderPath, const std::string & geometryShaderPath = "") {
    ShaderData shaderData;
    shaderData.vertexShaderSource = ReadShaderFile(vertexShaderPath);
    shaderData.fragmentShaderSource = ReadShaderFile(fragmentShaderPath);
    if (!geometryShaderPath.empty()) {
        shaderData.geometryShaderSource = ReadShaderFile(geometryShaderPath);
    }
    return shaderData;
}

/// @brief Reads the contents of a shader file.
/// @param filePath Path to the shader file.
/// @return The contents of the shader file as a string.
std::string ShaderLoader::ReadShaderFile(const std::string & filePath) {
    std::ifstream shaderFile(filePath);
    if (!shaderFile.is_open()) {
        std::cerr << "Failed to open shader file: " << filePath << std::endl;
        return "";
    }
    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    return shaderStream.str();
}