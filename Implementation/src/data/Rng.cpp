#include "Rng.h"

#include <random>

static std::mt19937 rngEngine;
static std::normal_distribution<double> normalDistribution(0.0, 1.0);

void seedRng(unsigned int seed) {
    rngEngine.seed(seed);
}

double gaussianNoise(double sigma) {
    return normalDistribution(rngEngine) * sigma;
}
