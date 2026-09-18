#include "ExperimentTests.h"

#include "ExperimentRunner.h"
#include "data/ModelGenerators.h"

#include <fstream>
#include <iostream>
#include <string>

static double computeMaxParameterError(const ExperimentMetrics& experimentMetrics) {
    double maxParameterError = 0.0;
    for (size_t parameterIndex = 0; parameterIndex < experimentMetrics.parameterErrors.size(); parameterIndex++) {
        if (experimentMetrics.parameterErrors[parameterIndex] > maxParameterError) {
            maxParameterError = experimentMetrics.parameterErrors[parameterIndex];
        }
    }
    return maxParameterError;
}

static bool csvHasExpectedColumns(const std::string& filePath, int expectedRowCount) {
    std::ifstream inputFile(filePath);
    std::string headerLine;
    std::getline(inputFile, headerLine);

    if (headerLine.find("model") == std::string::npos
        || headerLine.find("method") == std::string::npos
        || headerLine.find("theta_offset") == std::string::npos
        || headerLine.find("max_param_error") == std::string::npos
        || headerLine.find("failed") == std::string::npos
        || headerLine.find("final_residual_norm") == std::string::npos
        || headerLine.find("param_error_0") == std::string::npos) {
        return false;
    }

    int rowCount = 0;
    std::string dataLine;
    while (std::getline(inputFile, dataLine)) {
        if (!dataLine.empty()) {
            rowCount++;
        }
    }

    return rowCount == expectedRowCount;
}

bool runExperimentTests() {
    bool allTestsPassed = true;

    std::vector<std::string> modelNames = {"exponential", "logistic", "sinusoidal"};
    std::vector<SolverMethod> solverMethods = {SolverMethod::GaussNewton, SolverMethod::Newton};
    std::vector<ExperimentMetrics> experimentResults;

    for (size_t modelIndex = 0; modelIndex < modelNames.size(); modelIndex++) {
        Dataset referenceDataset;
        if (modelNames[modelIndex] == "exponential") {
            generateExponentialDataset(referenceDataset, 2.0, 0.3, 0.0, 5.0, 10, 0.0, 42);
        } else if (modelNames[modelIndex] == "logistic") {
            generateLogisticDataset(referenceDataset, 10.0, 1.5, 2.5, 0.0, 5.0, 10, 0.0, 42);
        } else {
            generateSinusoidalDataset(referenceDataset, 3.0, 2.0, 0.5, 0.0, 5.0, 10, 0.0, 42);
        }

        for (size_t methodIndex = 0; methodIndex < solverMethods.size(); methodIndex++) {
            ExperimentConfig experimentConfig;
            experimentConfig.modelName = modelNames[modelIndex];
            experimentConfig.pointCount = 10;
            experimentConfig.noiseSigma = 0.0;
            experimentConfig.thetaOffset = 0.05;
            experimentConfig.initialParameters = referenceDataset.trueParameters;
            for (size_t parameterIndex = 0; parameterIndex < experimentConfig.initialParameters.size(); parameterIndex++) {
                experimentConfig.initialParameters[parameterIndex] += 0.05;
            }
            experimentConfig.method = solverMethods[methodIndex];
            experimentConfig.maxIterationCount = 50;
            experimentConfig.tolerance = 1e-8;
            experimentConfig.seed = 42;
            experimentResults.push_back(runExperiment(experimentConfig));
        }
    }

    const std::string testCsvPath = "experiment_results_test.csv";
    exportExperimentResultsToCsv(experimentResults, testCsvPath);
    bool csvValid = csvHasExpectedColumns(testCsvPath, 6);

    std::cout << "experiment_runner csv_rows=" << experimentResults.size()
              << " csv_valid=" << csvValid << std::endl;

    if (!csvValid) {
        allTestsPassed = false;
    }

    ExperimentConfig closeInitialConfig;
    closeInitialConfig.modelName = "sinusoidal";
    closeInitialConfig.pointCount = 10;
    closeInitialConfig.noiseSigma = 0.0;
    closeInitialConfig.thetaOffset = 0.05;
    closeInitialConfig.maxIterationCount = 50;
    closeInitialConfig.tolerance = 1e-8;
    closeInitialConfig.seed = 42;
    closeInitialConfig.method = SolverMethod::GaussNewton;
    closeInitialConfig.initialParameters.values = {3.05, 2.05, 0.52};

    ExperimentConfig poorInitialConfig = closeInitialConfig;
    poorInitialConfig.thetaOffset = 1.0;
    poorInitialConfig.initialParameters.values = {6.0, 5.0, 3.0};

    ExperimentMetrics closeInitialMetrics = runExperiment(closeInitialConfig);
    ExperimentMetrics poorInitialMetrics = runExperiment(poorInitialConfig);

    double closeMaxParameterError = computeMaxParameterError(closeInitialMetrics);
    double poorMaxParameterError = computeMaxParameterError(poorInitialMetrics);
    bool parameterErrorDecreasesWithCloseInit = closeMaxParameterError < poorMaxParameterError;

    std::cout << "experiment_runner close_max_param_error=" << closeMaxParameterError
              << " poor_max_param_error=" << poorMaxParameterError << std::endl;

    if (!parameterErrorDecreasesWithCloseInit) {
        allTestsPassed = false;
    }

    std::vector<ExperimentMetrics> gridResults = runExperimentGrid();
    exportExperimentResultsToCsv(gridResults, "experiment_results.csv");
    std::cout << "experiment_runner grid_rows=" << gridResults.size() << std::endl;

    return allTestsPassed;
}
