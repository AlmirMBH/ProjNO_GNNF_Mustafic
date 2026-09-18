#include "data/CsvExport.h"
#include "data/ModelGenerators.h"
#include "models/ExponentialModel.h"
#include "models/LogisticModel.h"
#include "models/ModelTests.h"
#include "models/SinusoidalModel.h"
#include "solver/GaussNewtonTests.h"
#include "solver/NewtonTests.h"
#include "report/ReportExportTests.h"
#include "experiment/ComparisonTests.h"
#include "experiment/ExperimentTests.h"

#include <iostream>

static void printTrueParameters(const Dataset& dataset) {
    std::cout << "theta_true:";
    for (size_t parameterIndex = 0; parameterIndex < dataset.trueParameters.size(); parameterIndex++) {
        std::cout << " " << dataset.trueParameters[parameterIndex];
    }
    std::cout << std::endl;
}

static void printSampleRows(const Dataset& dataset, int rowCount) {
    int printCount = rowCount;
    if ((size_t)printCount > dataset.timePoints.size()) {
        printCount = (int)dataset.timePoints.size();
    }
    for (int rowIndex = 0; rowIndex < printCount; rowIndex++) {
        std::cout << "  t=" << dataset.timePoints[rowIndex]
                  << " y=" << dataset.observations[rowIndex];
        if (dataset.hasWeights) {
            std::cout << " sigma=" << dataset.weights[rowIndex];
        }
        std::cout << std::endl;
    }
}

static bool runSyntheticDataModelTests() {
    const unsigned int fixedSeed = 42;
    const int pointCount = 10;
    bool allTestsPassed = true;

    Dataset exponentialDataset;
    generateExponentialDataset(exponentialDataset, 2.0, 0.3, 0.0, 5.0, pointCount, 0.0, fixedSeed);
    ExponentialModel exponentialModel;
    std::cout << "synthetic_data_model exponential" << std::endl;
    if (!runModelTests(exponentialModel, exponentialDataset, exponentialDataset.trueParameters)) {
        allTestsPassed = false;
    }

    Dataset logisticDataset;
    generateLogisticDataset(logisticDataset, 10.0, 1.5, 2.5, 0.0, 5.0, pointCount, 0.0, fixedSeed);
    LogisticModel logisticModel;
    std::cout << "synthetic_data_model logistic" << std::endl;
    if (!runModelTests(logisticModel, logisticDataset, logisticDataset.trueParameters)) {
        allTestsPassed = false;
    }

    Dataset sinusoidalDataset;
    generateSinusoidalDataset(sinusoidalDataset, 3.0, 2.0, 0.5, 0.0, 5.0, pointCount, 0.0, fixedSeed);
    SinusoidalModel sinusoidalModel;
    std::cout << "synthetic_data_model sinusoidal" << std::endl;
    if (!runModelTests(sinusoidalModel, sinusoidalDataset, sinusoidalDataset.trueParameters)) {
        allTestsPassed = false;
    }

    return allTestsPassed;
}

int main() {
    std::cout << "GaussNewtonFit version 0.1.0" << std::endl;

    const unsigned int fixedSeed = 42;
    const int pointCount = 10;
    const double noiseSigma = 0.05;

    Dataset exponentialDataset;
    generateExponentialDataset(exponentialDataset, 2.0, 0.3, 0.0, 5.0, pointCount, noiseSigma, fixedSeed);
    std::cout << "exponential" << std::endl;
    printTrueParameters(exponentialDataset);
    printSampleRows(exponentialDataset, 3);
    exportDatasetToCsv(exponentialDataset, "exponential.csv");

    Dataset logisticDataset;
    generateLogisticDataset(logisticDataset, 10.0, 1.5, 2.5, 0.0, 5.0, pointCount, noiseSigma, fixedSeed);
    std::cout << "logistic" << std::endl;
    printTrueParameters(logisticDataset);
    printSampleRows(logisticDataset, 3);
    exportDatasetToCsv(logisticDataset, "logistic.csv");

    Dataset sinusoidalDataset;
    generateSinusoidalDataset(sinusoidalDataset, 3.0, 2.0, 0.5, 0.0, 5.0, pointCount, noiseSigma, fixedSeed);
    std::cout << "sinusoidal" << std::endl;
    printTrueParameters(sinusoidalDataset);
    printSampleRows(sinusoidalDataset, 3);
    exportDatasetToCsv(sinusoidalDataset, "sinusoidal.csv");

    if (!runSyntheticDataModelTests()) {
        return 1;
    }

    if (!runGaussNewtonTests()) {
        return 1;
    }

    if (!runNewtonTests()) {
        return 1;
    }

    if (!runExperimentTests()) {
        return 1;
    }

    if (!runComparisonTests()) {
        return 1;
    }

    if (!runReportExportTests()) {
        return 1;
    }

    return 0;
}
