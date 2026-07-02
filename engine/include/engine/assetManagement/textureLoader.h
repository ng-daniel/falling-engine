#include <vector>
#include <string>

enum class TextureType {
    Diffuse,
    Specular
};

struct TextureData {
    int width;
    int height;
    int channels;
    TextureType type;
    unsigned char * data;
    bool isLoaded;
};

class TextureLoader {
public:
    TextureData LoadTexture(const std::string & filePath);
};