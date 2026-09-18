#pragma once

#include "data/Dataset.h"
#include "data/Vec.h"
#include "solver/GaussNewtonSolver.h"

#include <gui/plot/View.h>

#include <vector>

struct PlotDataBuffers {
    std::vector<gui::CoordType> fitDataX;
    std::vector<gui::CoordType> fitDataY;
    std::vector<gui::CoordType> fitCurveX;
    std::vector<gui::CoordType> fitCurveY;
    std::vector<gui::CoordType> convergenceX;
    std::vector<gui::CoordType> convergenceY;
};

void fillFitPlotBuffers(PlotDataBuffers& plotDataBuffers, int modelIndex, const Dataset& dataset, const Vec& fittedParameters);
void fillConvergencePlotBuffers(PlotDataBuffers& plotDataBuffers, const std::vector<double>& residualNormHistory);
void applyFitPlot(gui::plot::View& plotView, const PlotDataBuffers& plotDataBuffers);
void applyConvergencePlot(gui::plot::View& plotView, const PlotDataBuffers& plotDataBuffers);
