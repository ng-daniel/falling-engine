#include <vector>
#include <engine/assets/assetLoaders/textureLoader.h>

struct VertexData {
    float position[3];
    float normal[3];
    float texCoords[2];
};

struct MeshData {
    std::vector<VertexData> vertices;
    std::vector<unsigned int> indices;
    std::vector<TextureData> textures;
};