#include "PlotUpdater.h"

#include "models/ExponentialModel.h"
#include "models/LogisticModel.h"
#include "models/SinusoidalModel.h"

static double evaluateModelFunction(int modelIndex, double timePoint, const Vec& parameters) {
    if (modelIndex == 0) {
        ExponentialModel exponentialModel;
        return exponentialModel.evaluateFunction(timePoint, parameters);
    }
    if (modelIndex == 1) {
        LogisticModel logisticModel;
        return logisticModel.evaluateFunction(timePoint, parameters);
    }
    SinusoidalModel sinusoidalModel;
    return sinusoidalModel.evaluateFunction(timePoint, parameters);
}

void fillFitPlotBuffers(PlotDataBuffers& plotDataBuffers, int modelIndex, const Dataset& dataset, const Vec& fittedParameters) {
    int pointCount = (int)dataset.timePoints.size();
    plotDataBuffers.fitDataX.resize(pointCount);
    plotDataBuffers.fitDataY.resize(pointCount);

    for (int pointIndex = 0; pointIndex < pointCount; pointIndex++) {
        plotDataBuffers.fitDataX[pointIndex] = (gui::CoordType)dataset.timePoints[pointIndex];
        plotDataBuffers.fitDataY[pointIndex] = (gui::CoordType)dataset.observations[pointIndex];
    }

    const int curvePointCount = 50;
    double timeStart = dataset.timePoints[0];
    double timeEnd = dataset.timePoints[pointCount - 1];
    plotDataBuffers.fitCurveX.resize(curvePointCount);
    plotDataBuffers.fitCurveY.resize(curvePointCount);

    for (int curveIndex = 0; curveIndex < curvePointCount; curveIndex++) {
        double timeValue = timeStart;
        if (curvePointCount > 1) {
            timeValue = timeStart + (timeEnd - timeStart) * curveIndex / (curvePointCount - 1);
        }
        plotDataBuffers.fitCurveX[curveIndex] = (gui::CoordType)timeValue;
        plotDataBuffers.fitCurveY[curveIndex] = (gui::CoordType)evaluateModelFunction(modelIndex, timeValue, fittedParameters);
    }
}

void fillConvergencePlotBuffers(PlotDataBuffers& plotDataBuffers, const std::vector<double>& residualNormHistory) {
    int historyCount = (int)residualNormHistory.size();
    plotDataBuffers.convergenceX.resize(historyCount);
    plotDataBuffers.convergenceY.resize(historyCount);

    for (int historyIndex = 0; historyIndex < historyCount; historyIndex++) {
        plotDataBuffers.convergenceX[historyIndex] = (gui::CoordType)historyIndex;
        plotDataBuffers.convergenceY[historyIndex] = (gui::CoordType)residualNormHistory[historyIndex];
    }
}

void applyFitPlot(gui::plot::View& plotView, const PlotDataBuffers& plotDataBuffers) {
    plotView.reset();
    plotView.setxAxisName("t");
    plotView.setyAxisName("y");
    plotView.addFunction(
        (gui::CoordType*)plotDataBuffers.fitDataX.data(),
        (gui::CoordType*)plotDataBuffers.fitDataY.data(),
        plotDataBuffers.fitDataX.size(),
        td::ColorID::Red,
        2,
        td::LinePattern::Dot,
        "data");
    plotView.addFunction(
        (gui::CoordType*)plotDataBuffers.fitCurveX.data(),
        (gui::CoordType*)plotDataBuffers.fitCurveY.data(),
        plotDataBuffers.fitCurveX.size(),
        td::ColorID::Blue,
        2,
        td::LinePattern::Solid,
        "fit");
    plotView.showLegend(true);
    plotView.fitToWindow();
}

void applyConvergencePlot(gui::plot::View& plotView, const PlotDataBuffers& plotDataBuffers) {
    plotView.reset();
    plotView.setxAxisName("iteration");
    plotView.setyAxisName("residual_norm");
    plotView.addFunction(
        (gui::CoordType*)plotDataBuffers.convergenceX.data(),
        (gui::CoordType*)plotDataBuffers.convergenceY.data(),
        plotDataBuffers.convergenceX.size(),
        td::ColorID::Green,
        2,
        td::LinePattern::Solid,
        "residual");
    plotView.showLegend(true);
    plotView.fitToWindow();
}
