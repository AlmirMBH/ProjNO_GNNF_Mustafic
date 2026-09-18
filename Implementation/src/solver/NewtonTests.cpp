#include "NewtonTests.h"

#include "GaussNewtonSolver.h"
#include "NewtonSolver.h"
#include "data/ModelGenerators.h"
#include "models/ExponentialModel.h"
#include "models/LogisticModel.h"
#include "models/SinusoidalModel.h"

#include <iostream>

bool runNewtonTests() {
    bool allTestsPassed = true;
    bool foundDifferentIterationCount = false;

    Dataset sinusoidalDataset;
    generateSinusoidalDataset(sinusoidalDataset, 3.0, 2.0, 0.5, 0.0, 5.0, 10, 0.0, 42);
    SinusoidalModel sinusoidalModel;

    GaussNewtonConfig sinusoidalConfig;
    sinusoidalConfig.maxIterationCount = 50;
    sinusoidalConfig.parameterTolerance = 1e-8;
    sinusoidalConfig.residualTolerance = 1e-8;
    sinusoidalConfig.initialParameters.values = {3.1, 2.05, 0.52};

    GaussNewtonResult gaussNewtonSinusoidalResult = runGaussNewton(sinusoidalModel, sinusoidalDataset, sinusoidalConfig);
    GaussNewtonResult newtonSinusoidalResult = runNewton(sinusoidalModel, sinusoidalDataset, sinusoidalConfig);

    std::cout << "newton_comparison sinusoidal gn_iterations=" << gaussNewtonSinusoidalResult.iterationCount
              << " gn_final_residual_norm=" << gaussNewtonSinusoidalResult.residualNormHistory.back()
              << " newton_iterations=" << newtonSinusoidalResult.iterationCount
              << " newton_final_residual_norm=" << newtonSinusoidalResult.residualNormHistory.back()
              << std::endl;

    if (gaussNewtonSinusoidalResult.iterationCount != newtonSinusoidalResult.iterationCount) {
        foundDifferentIterationCount = true;
    }

    Dataset exponentialDataset;
    generateExponentialDataset(exponentialDataset, 2.0, 0.3, 0.0, 2.0, 3, 0.05, 42);
    ExponentialModel exponentialModel;

    GaussNewtonConfig exponentialConfig;
    exponentialConfig.maxIterationCount = 20;
    exponentialConfig.parameterTolerance = 1e-10;
    exponentialConfig.residualTolerance = 1e-10;
    exponentialConfig.initialParameters.values = {1.0, 0.0};

    GaussNewtonResult gaussNewtonExponentialResult = runGaussNewton(exponentialModel, exponentialDataset, exponentialConfig);
    GaussNewtonResult newtonExponentialResult = runNewton(exponentialModel, exponentialDataset, exponentialConfig);

    std::cout << "newton_comparison exponential gn_iterations=" << gaussNewtonExponentialResult.iterationCount
              << " gn_final_residual_norm=" << gaussNewtonExponentialResult.residualNormHistory.back()
              << " newton_iterations=" << newtonExponentialResult.iterationCount
              << " newton_final_residual_norm=" << newtonExponentialResult.residualNormHistory.back()
              << std::endl;

    if (gaussNewtonExponentialResult.iterationCount != newtonExponentialResult.iterationCount) {
        foundDifferentIterationCount = true;
    }

    Dataset logisticDataset;
    generateLogisticDataset(logisticDataset, 10.0, 1.5, 2.5, 0.0, 5.0, 10, 0.1, 42);
    LogisticModel logisticModel;

    GaussNewtonConfig logisticConfig;
    logisticConfig.maxIterationCount = 30;
    logisticConfig.parameterTolerance = 1e-10;
    logisticConfig.residualTolerance = 1e-10;
    logisticConfig.initialParameters.values = {5.0, 0.5, 1.0};

    GaussNewtonResult gaussNewtonLogisticResult = runGaussNewton(logisticModel, logisticDataset, logisticConfig);
    GaussNewtonResult newtonLogisticResult = runNewton(logisticModel, logisticDataset, logisticConfig);

    std::cout << "newton_comparison logistic gn_iterations=" << gaussNewtonLogisticResult.iterationCount
              << " gn_final_residual_norm=" << gaussNewtonLogisticResult.residualNormHistory.back()
              << " newton_iterations=" << newtonLogisticResult.iterationCount
              << " newton_final_residual_norm=" << newtonLogisticResult.residualNormHistory.back()
              << std::endl;

    if (gaussNewtonLogisticResult.iterationCount != newtonLogisticResult.iterationCount) {
        foundDifferentIterationCount = true;
    }

    if (!foundDifferentIterationCount) {
        allTestsPassed = false;
        std::cout << "newton_comparison failed: gauss-newton and newton had same iteration counts on all cases" << std::endl;
    }

    return allTestsPassed;
}
