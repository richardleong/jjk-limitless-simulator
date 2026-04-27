#include "Utils.h"

std::mt19937& prng()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return gen;
}

float randFloat(float min, float max)
{
    std::uniform_real_distribution<float> dist(min, max);
    return dist(prng());
}