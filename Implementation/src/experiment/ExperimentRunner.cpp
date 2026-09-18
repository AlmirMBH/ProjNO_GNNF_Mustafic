#include "ExperimentRunner.h"

#include "data/ModelGenerators.h"
#include "models/ExponentialModel.h"
#include "models/LogisticModel.h"
#include "models/SinusoidalModel.h"
#include "solver/GaussNewtonSolver.h"
#include "solver/NewtonSolver.h"

#include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>

static void generateDatasetForModel(const std::string& modelName, int pointCount, double noiseSigma, unsigned int seed, Dataset& dataset) {
    if (modelName == "exponential") {
        generateExponentialDataset(dataset, 2.0, 0.3, 0.0, 5.0, pointCount, noiseSigma, seed);
        return;
    }
    if (modelName == "logistic") {
        generateLogisticDataset(dataset, 10.0, 1.5, 2.5, 0.0, 5.0, pointCount, noiseSigma, seed);
        return;
    }
    generateSinusoidalDataset(dataset, 3.0, 2.0, 0.5, 0.0, 5.0, pointCount, noiseSigma, seed);
}

static Vec buildInitialParametersFromOffset(const Vec& trueParameters, double thetaOffset) {
    Vec initialParameters;
    initialParameters.values.resize(trueParameters.size());
    for (size_t parameterIndex = 0; parameterIndex < trueParameters.size(); parameterIndex++) {
        initialParameters[parameterIndex] = trueParameters[parameterIndex] + thetaOffset;
    }
    return initialParameters;
}

static std::string solverMethodName(SolverMethod method) {
    if (method == SolverMethod::Newton) {
        return "newton";
    }
    return "gauss_newton";
}

static double computeMaxParameterError(const Vec& parameterErrors) {
    double maxParameterError = 0.0;
    for (size_t parameterIndex = 0; parameterIndex < parameterErrors.size(); parameterIndex++) {
        if (parameterErrors[parameterIndex] > maxParameterError) {
            maxParameterError = parameterErrors[parameterIndex];
        }
    }
    return maxParameterError;
}

ExperimentMetrics runExperiment(const ExperimentConfig& experimentConfig) {
    Dataset dataset;
    generateDatasetForModel(experimentConfig.modelName, experimentConfig.pointCount, experimentConfig.noiseSigma, experimentConfig.seed, dataset);

    GaussNewtonConfig solverConfig;
    solverConfig.maxIterationCount = experimentConfig.maxIterationCount;
    solverConfig.parameterTolerance = experimentConfig.tolerance;
    solverConfig.residualTolerance = experimentConfig.tolerance;
    solverConfig.initialParameters = experimentConfig.initialParameters;

    auto startTime = std::chrono::high_resolution_clock::now();
    GaussNewtonResult solverResult;

    if (experimentConfig.modelName == "exponential") {
        ExponentialModel exponentialModel;
        if (experimentConfig.method == SolverMethod::Newton) {
            solverResult = runNewton(exponentialModel, dataset, solverConfig);
        } else {
            solverResult = runGaussNewton(exponentialModel, dataset, solverConfig);
        }
    } else if (experimentConfig.modelName == "logistic") {
        LogisticModel logisticModel;
        if (experimentConfig.method == SolverMethod::Newton) {
            solverResult = runNewton(logisticModel, dataset, solverConfig);
        } else {
            solverResult = runGaussNewton(logisticModel, dataset, solverConfig);
        }
    } else {
        SinusoidalModel sinusoidalModel;
        if (experimentConfig.method == SolverMethod::Newton) {
            solverResult = runNewton(sinusoidalModel, dataset, solverConfig);
        } else {
            solverResult = runGaussNewton(sinusoidalModel, dataset, solverConfig);
        }
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    double wallTimeSeconds = std::chrono::duration<double>(endTime - startTime).count();

    ExperimentMetrics experimentMetrics;
    experimentMetrics.modelName = experimentConfig.modelName;
    experimentMetrics.methodName = solverMethodName(experimentConfig.method);
    experimentMetrics.pointCount = experimentConfig.pointCount;
    experimentMetrics.noiseSigma = experimentConfig.noiseSigma;
    experimentMetrics.thetaOffset = experimentConfig.thetaOffset;
    experimentMetrics.iterationCount = solverResult.iterationCount;
    experimentMetrics.wallTimeSeconds = wallTimeSeconds;
    experimentMetrics.finalResidualNorm = solverResult.residualNormHistory.back();
    experimentMetrics.failed = solverResult.failed;
    experimentMetrics.parameterErrors.values.resize(dataset.trueParameters.size());
    for (size_t parameterIndex = 0; parameterIndex < dataset.trueParameters.size(); parameterIndex++) {
        experimentMetrics.parameterErrors[parameterIndex] = std::fabs(
            solverResult.finalParameters[parameterIndex] - dataset.trueParameters[parameterIndex]);
    }
    experimentMetrics.maxParameterError = computeMaxParameterError(experimentMetrics.parameterErrors);

    return experimentMetrics;
}

static std::vector<ExperimentMetrics> runComparisonGridInternal() {
    std::vector<double> noiseLevels = {0.0, 0.05, 0.2};
    std::vector<double> thetaOffsets = {0.05, 1.0};
    std::vector<std::string> modelNames = {"exponential", "logistic", "sinusoidal"};
    std::vector<SolverMethod> solverMethods = {SolverMethod::GaussNewton, SolverMethod::Newton};

    std::vector<ExperimentMetrics> experimentResults;

    for (size_t modelIndex = 0; modelIndex < modelNames.size(); modelIndex++) {
        Dataset referenceDataset;
        generateDatasetForModel(modelNames[modelIndex], 10, 0.0, 42, referenceDataset);

        for (size_t methodIndex = 0; methodIndex < solverMethods.size(); methodIndex++) {
            for (size_t noiseIndex = 0; noiseIndex < noiseLevels.size(); noiseIndex++) {
                for (size_t offsetIndex = 0; offsetIndex < thetaOffsets.size(); offsetIndex++) {
                    ExperimentConfig experimentConfig;
                    experimentConfig.modelName = modelNames[modelIndex];
                    experimentConfig.pointCount = 10;
                    experimentConfig.noiseSigma = noiseLevels[noiseIndex];
                    experimentConfig.thetaOffset = thetaOffsets[offsetIndex];
                    experimentConfig.initialParameters = buildInitialParametersFromOffset(referenceDataset.trueParameters, thetaOffsets[offsetIndex]);
                    experimentConfig.method = solverMethods[methodIndex];
                    experimentConfig.maxIterationCount = 50;
                    experimentConfig.tolerance = 1e-8;
                    experimentConfig.seed = 42;
                    experimentResults.push_back(runExperiment(experimentConfig));
                }
            }
        }
    }

    return experimentResults;
}

std::vector<ExperimentMetrics> runExperimentGrid() {
    return runComparisonGridInternal();
}

std::vector<ExperimentMetrics> runComparisonExperiments() {
    return runComparisonGridInternal();
}

std::vector<ComparisonSummaryRow> buildComparisonSummary(const std::vector<ExperimentMetrics>& experimentResults) {
    std::vector<ComparisonSummaryRow> comparisonSummaryRows;

    for (size_t resultIndex = 0; resultIndex < experimentResults.size(); resultIndex++) {
        const ExperimentMetrics& experimentMetrics = experimentResults[resultIndex];
        if (experimentMetrics.methodName != "gauss_newton") {
            continue;
        }

        ComparisonSummaryRow comparisonSummaryRow;
        comparisonSummaryRow.modelName = experimentMetrics.modelName;
        comparisonSummaryRow.noiseSigma = experimentMetrics.noiseSigma;
        comparisonSummaryRow.thetaOffset = experimentMetrics.thetaOffset;
        comparisonSummaryRow.gaussNewtonIterationCount = experimentMetrics.iterationCount;
        comparisonSummaryRow.gaussNewtonMaxParameterError = experimentMetrics.maxParameterError;
        comparisonSummaryRow.gaussNewtonFailed = experimentMetrics.failed;

        bool foundNewtonResult = false;
        for (size_t otherIndex = 0; otherIndex < experimentResults.size(); otherIndex++) {
            const ExperimentMetrics& otherMetrics = experimentResults[otherIndex];
            if (otherMetrics.modelName == experimentMetrics.modelName
                && otherMetrics.noiseSigma == experimentMetrics.noiseSigma
                && otherMetrics.thetaOffset == experimentMetrics.thetaOffset
                && otherMetrics.methodName == "newton") {
                comparisonSummaryRow.newtonIterationCount = otherMetrics.iterationCount;
                comparisonSummaryRow.newtonMaxParameterError = otherMetrics.maxParameterError;
                comparisonSummaryRow.newtonFailed = otherMetrics.failed;
                foundNewtonResult = true;
                break;
            }
        }

        if (!foundNewtonResult) {
            continue;
        }

        comparisonSummaryRow.methodsDiffer =
            comparisonSummaryRow.gaussNewtonIterationCount != comparisonSummaryRow.newtonIterationCount
            || comparisonSummaryRow.gaussNewtonFailed != comparisonSummaryRow.newtonFailed
            || std::fabs(comparisonSummaryRow.gaussNewtonMaxParameterError - comparisonSummaryRow.newtonMaxParameterError) > 1e-6;

        comparisonSummaryRows.push_back(comparisonSummaryRow);
    }

    return comparisonSummaryRows;
}

void exportExperimentResultsToCsv(const std::vector<ExperimentMetrics>& experimentResults, const std::string& filePath) {
    std::ofstream outputFile(filePath);
    outputFile << "model,method,n_points,noise_sigma,theta_offset,iteration_count,wall_time_sec,final_residual_norm,max_param_error,failed,param_error_0,param_error_1,param_error_2\n";
    outputFile << std::fixed << std::setprecision(8);

    for (size_t resultIndex = 0; resultIndex < experimentResults.size(); resultIndex++) {
        const ExperimentMetrics& experimentMetrics = experimentResults[resultIndex];
        outputFile << experimentMetrics.modelName << ","
                   << experimentMetrics.methodName << ","
                   << experimentMetrics.pointCount << ","
                   << experimentMetrics.noiseSigma << ","
                   << experimentMetrics.thetaOffset << ","
                   << experimentMetrics.iterationCount << ","
                   << experimentMetrics.wallTimeSeconds << ","
                   << experimentMetrics.finalResidualNorm << ","
                   << experimentMetrics.maxParameterError << ","
                   << (experimentMetrics.failed ? 1 : 0) << ",";

        for (int parameterIndex = 0; parameterIndex < 3; parameterIndex++) {
            if ((size_t)parameterIndex < experimentMetrics.parameterErrors.size()) {
                outputFile << experimentMetrics.parameterErrors[parameterIndex];
            } else {
                outputFile << 0.0;
            }
            if (parameterIndex < 2) {
                outputFile << ",";
            }
        }
        outputFile << "\n";
    }
}

void exportComparisonSummaryToCsv(const std::vector<ComparisonSummaryRow>& comparisonSummaryRows, const std::string& filePath) {
    std::ofstream outputFile(filePath);
    outputFile << "model,noise_sigma,theta_offset,gn_iterations,newton_iterations,gn_max_param_error,newton_max_param_error,gn_failed,newton_failed,methods_differ\n";
    outputFile << std::fixed << std::setprecision(8);

    for (size_t rowIndex = 0; rowIndex < comparisonSummaryRows.size(); rowIndex++) {
        const ComparisonSummaryRow& comparisonSummaryRow = comparisonSummaryRows[rowIndex];
        outputFile << comparisonSummaryRow.modelName << ","
                   << comparisonSummaryRow.noiseSigma << ","
                   << comparisonSummaryRow.thetaOffset << ","
                   << comparisonSummaryRow.gaussNewtonIterationCount << ","
                   << comparisonSummaryRow.newtonIterationCount << ","
                   << comparisonSummaryRow.gaussNewtonMaxParameterError << ","
                   << comparisonSummaryRow.newtonMaxParameterError << ","
                   << (comparisonSummaryRow.gaussNewtonFailed ? 1 : 0) << ","
                   << (comparisonSummaryRow.newtonFailed ? 1 : 0) << ","
                   << (comparisonSummaryRow.methodsDiffer ? 1 : 0) << "\n";
    }
}

void printComparisonSummary(const std::vector<ComparisonSummaryRow>& comparisonSummaryRows) {
    for (size_t rowIndex = 0; rowIndex < comparisonSummaryRows.size(); rowIndex++) {
        const ComparisonSummaryRow& comparisonSummaryRow = comparisonSummaryRows[rowIndex];
        std::cout << comparisonSummaryRow.modelName
                  << " noise=" << comparisonSummaryRow.noiseSigma
                  << " theta_offset=" << comparisonSummaryRow.thetaOffset
                  << " gn_iter=" << comparisonSummaryRow.gaussNewtonIterationCount
                  << " newton_iter=" << comparisonSummaryRow.newtonIterationCount
                  << " gn_err=" << comparisonSummaryRow.gaussNewtonMaxParameterError
                  << " newton_err=" << comparisonSummaryRow.newtonMaxParameterError
                  << " gn_failed=" << comparisonSummaryRow.gaussNewtonFailed
                  << " newton_failed=" << comparisonSummaryRow.newtonFailed
                  << " differ=" << comparisonSummaryRow.methodsDiffer
                  << std::endl;
    }
}
