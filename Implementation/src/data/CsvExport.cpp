#include "CsvExport.h"

#include <fstream>
#include <iomanip>

void exportDatasetToCsv(const Dataset& dataset, const std::string& filePath) {
    std::ofstream outputFile(filePath);
    outputFile << "model," << dataset.modelName << "\n";
    outputFile << "theta_true";
    for (size_t parameterIndex = 0; parameterIndex < dataset.trueParameters.size(); parameterIndex++) {
        outputFile << "," << dataset.trueParameters[parameterIndex];
    }
    outputFile << "\n";
    outputFile << "t,y";
    if (dataset.hasWeights) {
        outputFile << ",sigma";
    }
    outputFile << "\n";

    outputFile << std::fixed << std::setprecision(6);
    for (size_t pointIndex = 0; pointIndex < dataset.timePoints.size(); pointIndex++) {
        outputFile << dataset.timePoints[pointIndex] << "," << dataset.observations[pointIndex];
        if (dataset.hasWeights) {
            outputFile << "," << dataset.weights[pointIndex];
        }
        outputFile << "\n";
    }
}
