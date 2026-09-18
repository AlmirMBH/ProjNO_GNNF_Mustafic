#pragma once

#include "data/Dataset.h"
#include "solver/GaussNewtonSolver.h"

#include <string>

void exportRunReport(
    const Dataset& dataset,
    const GaussNewtonResult& solverResult,
    const std::string& methodName,
    double noiseSigma,
    const std::string& fitPlotPath,
    const std::string& convergencePlotPath,
    const std::string& filePath);
