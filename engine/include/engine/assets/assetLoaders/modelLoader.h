#include <engine/assets/assetLoaders/meshLoader.h>

struct ModelData {
    std::vector<MeshData> meshes;
};

class ModelLoader {
public:
    ModelData LoadModel(const std::string & filePath);
};