#pragma once

#include "data/Vec.h"

#include <string>

struct ExperimentMetrics {
    std::string modelName;
    std::string methodName;
    int pointCount;
    double noiseSigma;
    double thetaOffset;
    int iterationCount;
    double wallTimeSeconds;
    double finalResidualNorm;
    double maxParameterError;
    bool failed;
    Vec parameterErrors;
};

struct ComparisonSummaryRow {
    std::string modelName;
    double noiseSigma;
    double thetaOffset;
    int gaussNewtonIterationCount;
    int newtonIterationCount;
    double gaussNewtonMaxParameterError;
    double newtonMaxParameterError;
    bool gaussNewtonFailed;
    bool newtonFailed;
    bool methodsDiffer;
};
