#include "generator.h"

#include "engine/debug/logger.h"

int main() {
    try {
        AssetHeaderGenerator::Generate("./game/assets", "./game/include/generated");
    } catch (const std::exception& e) {
        Logger::Error("asset_header_generator", e.what());
        return 1;
    }
    return 0;
}