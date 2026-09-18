#include "ReportExportTests.h"

#include "ReportExport.h"
#include "data/ModelGenerators.h"
#include "models/SinusoidalModel.h"
#include "solver/GaussNewtonSolver.h"

#include <fstream>
#include <iostream>
#include <string>

static bool reportFileHasRequiredFields(const std::string& filePath) {
    std::ifstream inputFile(filePath);
    std::string fileContent((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
    return fileContent.find("final_theta") != std::string::npos
        && fileContent.find("iteration_history") != std::string::npos
        && fileContent.find("fit_plot_path") != std::string::npos
        && fileContent.find("convergence_plot_path") != std::string::npos
        && fileContent.find("final_residual_norm") != std::string::npos;
}

bool runReportExportTests() {
    Dataset dataset;
    generateSinusoidalDataset(dataset, 3.0, 2.0, 0.5, 0.0, 5.0, 10, 0.05, 42);

    GaussNewtonConfig solverConfig;
    solverConfig.maxIterationCount = 50;
    solverConfig.parameterTolerance = 1e-8;
    solverConfig.residualTolerance = 1e-8;
    solverConfig.initialParameters.values = {3.1, 2.05, 0.52};

    SinusoidalModel sinusoidalModel;
    GaussNewtonResult solverResult = runGaussNewton(sinusoidalModel, dataset, solverConfig);

    exportRunReport(
        dataset,
        solverResult,
        "gauss_newton",
        0.05,
        "fit_plot.xml",
        "convergence_plot.xml",
        "run_export.txt");

    bool reportValid = reportFileHasRequiredFields("run_export.txt");
    std::cout << "report_export run_export_valid=" << reportValid << std::endl;
    return reportValid;
}
