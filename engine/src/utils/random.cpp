#include "engine/utils/random.h"

std::random_device Random::rd;
std::mt19937 Random::rng(Random::rd());

/**
 * @brief Sets the seed for the random number generator.
 * 
 * @param seed 
 */
void Random::SetSeed(unsigned int seed) {
    Random::rng.seed(seed);
}

/**
 * @brief Returns random int within [min, max]
 * 
 * @param min 
 * @param max 
 * @return int 
 */
int Random::RandInt(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(Random::rng);
}

/**
 * @brief Returns random float within [min, max]
 * 
 * @param min 
 * @param max 
 * @return float 
 */
float Random::RandFloat(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(Random::rng);
}