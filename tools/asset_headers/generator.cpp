#include "engine/assets/asset_warehouse_service.h"

#include <fstream>
#include <algorithm>
#include <string>
#include <vector>

#include "generator.h"

namespace {
    const std::string HEADER_FILE_NAME = "asset_ids.h";
    
    void TransformExportNames(std::vector<std::string>& exportNames) {
        for (std::string& name : exportNames) {
            std::transform(name.begin(), name.end(), name.begin(), ::toupper);
        }
    }
    
    void WriteHeaderFile(const std::string& outputDir, const std::vector<std::string>& exportNames, const std::vector<UUID>& uuids) {
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
    
    // extract names and uuids into separate vectors
    std::vector<std::string> keys;
    std::vector<UUID> values;
    keys.reserve(exportNameUUIDMap.size());
    values.reserve(exportNameUUIDMap.size());
    std::transform(exportNameUUIDMap.begin(), exportNameUUIDMap.end(), std::back_inserter(keys),
                   [](const auto& pair) { return pair.first; });
    std::transform(exportNameUUIDMap.begin(), exportNameUUIDMap.end(), std::back_inserter(values),
                   [](const auto& pair) { return pair.second; });
    
    TransformExportNames(keys);
    WriteHeaderFile(outputDir, keys, values);
}