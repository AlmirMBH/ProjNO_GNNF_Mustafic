#include "ReportExport.h"

#include <fstream>
#include <iomanip>

void exportRunReport(
    const Dataset& dataset,
    const GaussNewtonResult& solverResult,
    const std::string& methodName,
    double noiseSigma,
    const std::string& fitPlotPath,
    const std::string& convergencePlotPath,
    const std::string& filePath) {
    std::ofstream outputFile(filePath);
    outputFile << std::fixed << std::setprecision(8);
    outputFile << "model=" << dataset.modelName << "\n";
    outputFile << "method=" << methodName << "\n";
    outputFile << "noise_sigma=" << noiseSigma << "\n";
    outputFile << "iteration_count=" << solverResult.iterationCount << "\n";
    outputFile << "failed=" << (solverResult.failed ? 1 : 0) << "\n";
    outputFile << "final_residual_norm=" << solverResult.residualNormHistory.back() << "\n";
    outputFile << "final_theta";
    for (size_t parameterIndex = 0; parameterIndex < solverResult.finalParameters.size(); parameterIndex++) {
        outputFile << " " << solverResult.finalParameters[parameterIndex];
    }
    outputFile << "\n";
    outputFile << "fit_plot_path=" << fitPlotPath << "\n";
    outputFile << "convergence_plot_path=" << convergencePlotPath << "\n";
    outputFile << "iteration_history\n";
    for (size_t historyIndex = 0; historyIndex < solverResult.residualNormHistory.size(); historyIndex++) {
        outputFile << historyIndex << " " << solverResult.residualNormHistory[historyIndex] << "\n";
    }
}
