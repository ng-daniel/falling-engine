#include <string>

struct ShaderData {
    std::string vertexShaderSource;
    std::string fragmentShaderSource;
    std::string geometryShaderSource;
};

class ShaderLoader {
public:
    ShaderData LoadShader(const std::string & vertexShaderPath, const std::string & fragmentShaderPath, const std::string & geometryShaderPath = "");
private:
    std::string ReadShaderFile(const std::string & filePath);
};