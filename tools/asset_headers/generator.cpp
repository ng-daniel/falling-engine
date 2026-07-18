#include "engine/assets/asset_warehouse_service.h"

#include <fstream>
#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>

#include "generator.h"

namespace {
    const std::string HEADER_FILE_NAME = "asset_ids.h";
    
    std::vector<std::string> TransformExportNames(
        const std::vector<std::string>& exportNames,
        const std::vector<std::string>& types
    ) {
        std::vector<std::string> transformedNames;

        size_t numAssets = exportNames.size();
        for (size_t i = 0; i < numAssets; i++) {
            
            std::string name = exportNames[i];
            std::transform(name.begin(), name.end(), name.begin(), ::toupper);
            
            std::string type = types[i];
            std::transform(type.begin(), type.end(), type.begin(), ::toupper);
            
            name += "_" + type;
            transformedNames.push_back(name);
        }
        return transformedNames;
    }
    
    void WriteHeaderFile(const std::string& outputDir, 
        const std::vector<std::string>& exportNames,
        const std::vector<UUID>& uuids) {
        std::ofstream out(outputDir + "/" + HEADER_FILE_NAME);

        out << "#ifndef ASSET_IDS_H\n";
        out << "#define ASSET_IDS_H\n\n";

        out << "#include <engine/assets/asset_handle.h>\n";
        out << "#include <engine/utils/uuid.h>\n\n";

        out << "namespace GameAssets\n{\n";

        size_t numAssets = exportNames.size();
        for (size_t i = 0; i < numAssets; i++)
        {
            out << "    constexpr AssetHandle "
                << exportNames[i]
                << "{ "
                << uuids[i]
                << " };\n";
        }

        out << "}\n";
        out << "#endif // ASSET_IDS_H\n";
    }
}



/**
 * @brief Generates a header file containing human-readable constants that map to UUIDs of assets in the asset warehouse. 
 * 
 * @param assetRootDir 
 * @param outputDir 
 */
void AssetHeaderGenerator::Generate(const std::string& assetRootDir, const std::string& outputDir) {
    AssetWarehouseService assetWarehouseService(assetRootDir);
    std::unordered_map<std::string, UUID> exportNameUUIDMap = assetWarehouseService.GetAllExportNameUUIDMappings();
    std::unordered_map<UUID, RuntimeAssetMetadata> runtimeMetadatas = assetWarehouseService.GetAllRuntimeMetadatas();
    
    // extract names and uuids into separate vectors
    std::vector<std::string> keys;
    std::vector<UUID> values;
    std::vector<std::string> types;
    keys.reserve(exportNameUUIDMap.size());
    values.reserve(exportNameUUIDMap.size());
    types.reserve(exportNameUUIDMap.size());
    std::transform(exportNameUUIDMap.begin(), exportNameUUIDMap.end(), std::back_inserter(keys),
                   [](const auto& pair) { return pair.first; });
    std::transform(exportNameUUIDMap.begin(), exportNameUUIDMap.end(), std::back_inserter(values),
                   [](const auto& pair) { return pair.second; });
    for (const auto& uuid : values) {
        types.push_back(runtimeMetadatas[uuid].type);
    }
    
    std::vector<std::string> transformedKeys = TransformExportNames(keys, types);
    WriteHeaderFile(outputDir, transformedKeys, values);
}