#include <vector>
#include <string>

/// @brief Struct to hold texture data loaded from a file.
struct TextureData {
    std::string name; // format of "texture_<type>"
    int width;        // width of the texture
    int height;       // height of the texture
    int channels;     // number of color channels
    char * data;      // raw texture data, is temporary until the texture is uploaded to the GPU, then it is deleted
    bool isLoaded;    // flag to indicate if the texture has been successfully uploaded to GPU
};

class TextureLoader {
public:
    TextureData LoadTexture(const std::string & filePath);
};