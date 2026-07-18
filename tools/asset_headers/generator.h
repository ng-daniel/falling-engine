#ifndef TOOLS_ASSET_HEADERS_GENERATOR_H
#define TOOLS_ASSET_HEADERS_GENERATOR_H

#include <string>

class AssetHeaderGenerator {
public:
    static void Generate(const std::string& assetRootDir, const std::string& outputDir);
};

#endif // TOOLS_ASSET_HEADERS_GENERATOR_H