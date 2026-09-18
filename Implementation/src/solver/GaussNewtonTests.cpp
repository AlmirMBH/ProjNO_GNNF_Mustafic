#include "GaussNewtonTests.h"

#include "GaussNewtonSolver.h"
#include "data/ModelGenerators.h"
#include "models/ExponentialModel.h"
#include "models/SinusoidalModel.h"

#include <cmath>
#include <iostream>

static bool parametersWithinTolerance(const Vec& fittedParameters, const Vec& trueParameters, double tolerance) {
    for (size_t parameterIndex = 0; parameterIndex < trueParameters.size(); parameterIndex++) {
        if (std::fabs(fittedParameters[parameterIndex] - trueParameters[parameterIndex]) > tolerance) {
            return false;
        }
    }
    return true;
}

bool runGaussNewtonTests() {
    bool allTestsPassed = true;

    Dataset exponentialDataset;
    generateExponentialDataset(exponentialDataset, 2.0, 0.3, 0.0, 2.0, 3, 0.0, 42);
    ExponentialModel exponentialModel;

    GaussNewtonConfig exponentialConfig;
    exponentialConfig.maxIterationCount = 3;
    exponentialConfig.parameterTolerance = 1e-12;
    exponentialConfig.residualTolerance = 1e-12;
    exponentialConfig.initialParameters.values = {1.0, 0.0};

    GaussNewtonResult exponentialResult = runGaussNewton(exponentialModel, exponentialDataset, exponentialConfig);
    double exponentialInitialResidualNorm = exponentialResult.residualNormHistory.front();
    double exponentialFinalResidualNorm = exponentialResult.residualNormHistory.back();
    bool exponentialImproved = exponentialFinalResidualNorm < exponentialInitialResidualNorm;

    std::cout << "gauss_newton exponential"
              << " initial_residual_norm=" << exponentialInitialResidualNorm
              << " final_residual_norm=" << exponentialFinalResidualNorm
              << " iterations=" << exponentialResult.iterationCount << std::endl;

    if (!exponentialImproved) {
        allTestsPassed = false;
    }

    Dataset sinusoidalDataset;
    generateSinusoidalDataset(sinusoidalDataset, 3.0, 2.0, 0.5, 0.0, 5.0, 10, 0.0, 42);
    SinusoidalModel sinusoidalModel;

    GaussNewtonConfig sinusoidalConfig;
    sinusoidalConfig.maxIterationCount = 50;
    sinusoidalConfig.parameterTolerance = 1e-8;
    sinusoidalConfig.residualTolerance = 1e-8;
    sinusoidalConfig.initialParameters.values = {3.1, 2.05, 0.52};

    GaussNewtonResult sinusoidalResult = runGaussNewton(sinusoidalModel, sinusoidalDataset, sinusoidalConfig);
    bool sinusoidalConverged = parametersWithinTolerance(
        sinusoidalResult.finalParameters,
        sinusoidalDataset.trueParameters,
        1e-4);

    std::cout << "gauss_newton sinusoidal"
              << " final_residual_norm=" << sinusoidalResult.residualNormHistory.back()
              << " iterations=" << sinusoidalResult.iterationCount << std::endl;

    if (!sinusoidalConverged) {
        allTestsPassed = false;
    }

    return allTestsPassed;
}
