#include <iostream>
#include "generator.h"

int main() {
    try {
        AssetHeaderGenerator::Generate("./game/assets", "./game/include");
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}