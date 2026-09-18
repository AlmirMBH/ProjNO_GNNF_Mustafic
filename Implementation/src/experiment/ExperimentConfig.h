#pragma once

#include "data/Vec.h"

#include <string>

enum class SolverMethod {
    GaussNewton,
    Newton
};

struct ExperimentConfig {
    std::string modelName;
    int pointCount;
    double noiseSigma;
    double thetaOffset;
    Vec initialParameters;
    SolverMethod method;
    int maxIterationCount;
    double tolerance;
    unsigned int seed;
};
