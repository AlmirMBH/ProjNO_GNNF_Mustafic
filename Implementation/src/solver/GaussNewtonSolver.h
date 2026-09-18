#pragma once

#include "data/Dataset.h"
#include "data/Vec.h"
#include "models/Model.h"

#include <vector>

struct GaussNewtonConfig {
    int maxIterationCount;
    double parameterTolerance;
    double residualTolerance;
    Vec initialParameters;
};

struct GaussNewtonResult {
    Vec finalParameters;
    int iterationCount;
    bool failed;
    std::vector<double> residualNormHistory;
    std::vector<Vec> parameterHistory;
};

GaussNewtonResult runGaussNewton(const Model& model, const Dataset& dataset, const GaussNewtonConfig& config);
