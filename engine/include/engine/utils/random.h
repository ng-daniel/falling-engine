#pragma once

#include <random>

class Random {
public:
    static void SetSeed(unsigned int seed);
    static int RandInt(int min, int max);
    static float RandFloat(float min, float max);
private:
    static std::random_device rd;
    static std::mt19937 rng;
};