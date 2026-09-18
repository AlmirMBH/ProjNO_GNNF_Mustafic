#include "MainView.h"

#include "data/ModelGenerators.h"
#include "experiment/ExperimentConfig.h"
#include "models/ExponentialModel.h"
#include "models/LogisticModel.h"
#include "models/SinusoidalModel.h"
#include "solver/GaussNewtonSolver.h"
#include "report/ReportExport.h"
#include "solver/NewtonSolver.h"

#include <fstream>
#include <iomanip>
#include <sstream>

MainView::MainView()
    : modelComboBox(),
      methodComboBox(),
      noiseLabel("Noise sigma:"),
      noiseSlider(),
      theta0Label0("theta0[0]:"),
      theta0Label1("theta0[1]:"),
      theta0Label2("theta0[2]:"),
      theta0Edit0(td::real8, gui::LineEdit::Messages::DoNotSend, false, "", 4),
      theta0Edit1(td::real8, gui::LineEdit::Messages::DoNotSend, false, "", 4),
      theta0Edit2(td::real8, gui::LineEdit::Messages::DoNotSend, false, "", 4),
      runButton("Run"),
      exportLogButton("Export log"),
      statusLabel("Status: ready"),
      plotFonts(),
      fitPlotView(&plotFonts.axis, &plotFonts.legend),
      convergencePlotView(&plotFonts.axis, &plotFonts.legend),
      logTextEdit(gui::TextEdit::HorizontalScroll::Yes, gui::TextEdit::Events::DoNotSend, true),
      mainLayout(9),
      theta0Layout(6),
      buttonLayout(2),
      plotLayout(2) {
    modelComboBox.addItem("exponential");
    modelComboBox.addItem("logistic");
    modelComboBox.addItem("sinusoidal");
    modelComboBox.selectIndex(0);

    methodComboBox.addItem("gauss_newton");
    methodComboBox.addItem("newton");
    methodComboBox.selectIndex(0);

    noiseSlider.setRange(0.0, 0.2);
    noiseSlider.setValue(0.05);

    theta0Edit0.setValue(2.05);
    theta0Edit1.setValue(0.35);
    theta0Edit2.setValue(2.55);

    theta0Layout.append(theta0Label0);
    theta0Layout.append(theta0Edit0);
    theta0Layout.append(theta0Label1);
    theta0Layout.append(theta0Edit1);
    theta0Layout.append(theta0Label2);
    theta0Layout.append(theta0Edit2);

    buttonLayout.append(runButton);
    buttonLayout.append(exportLogButton);

    plotLayout.append(fitPlotView);
    plotLayout.append(convergencePlotView);

    mainLayout.append(modelComboBox);
    mainLayout.append(methodComboBox);
    mainLayout.append(noiseLabel);
    mainLayout.append(noiseSlider);
    mainLayout.append(theta0Layout);
    mainLayout.append(buttonLayout);
    mainLayout.append(statusLabel);
    mainLayout.append(plotLayout);
    mainLayout.append(logTextEdit);

    setLayout(&mainLayout);
}

static GaussNewtonResult runSolverForModel(
    int modelIndex,
    SolverMethod solverMethod,
    const Dataset& dataset,
    const GaussNewtonConfig& solverConfig) {
    if (modelIndex == 0) {
        ExponentialModel exponentialModel;
        if (solverMethod == SolverMethod::Newton) {
            return runNewton(exponentialModel, dataset, solverConfig);
        }
        return runGaussNewton(exponentialModel, dataset, solverConfig);
    }
    if (modelIndex == 1) {
        LogisticModel logisticModel;
        if (solverMethod == SolverMethod::Newton) {
            return runNewton(logisticModel, dataset, solverConfig);
        }
        return runGaussNewton(logisticModel, dataset, solverConfig);
    }
    SinusoidalModel sinusoidalModel;
    if (solverMethod == SolverMethod::Newton) {
        return runNewton(sinusoidalModel, dataset, solverConfig);
    }
    return runGaussNewton(sinusoidalModel, dataset, solverConfig);
}

void MainView::refreshPlots(int modelIndex, const Dataset& dataset, const GaussNewtonResult& solverResult) {
    fillFitPlotBuffers(plotDataBuffers, modelIndex, dataset, solverResult.finalParameters);
    fillConvergencePlotBuffers(plotDataBuffers, solverResult.residualNormHistory);
    applyFitPlot(fitPlotView, plotDataBuffers);
    applyConvergencePlot(convergencePlotView, plotDataBuffers);
}

bool MainView::onClick(gui::Button* button) {
    if (button == &runButton) {
        int modelIndex = modelComboBox.getSelectedIndex();
        SolverMethod solverMethod = methodComboBox.getSelectedIndex() == 1 ? SolverMethod::Newton : SolverMethod::GaussNewton;
        double noiseSigma = noiseSlider.getValue();

        Dataset dataset;
        if (modelIndex == 0) {
            generateExponentialDataset(dataset, 2.0, 0.3, 0.0, 5.0, 10, noiseSigma, 42);
        } else if (modelIndex == 1) {
            generateLogisticDataset(dataset, 10.0, 1.5, 2.5, 0.0, 5.0, 10, noiseSigma, 42);
        } else {
            generateSinusoidalDataset(dataset, 3.0, 2.0, 0.5, 0.0, 5.0, 10, noiseSigma, 42);
        }

        GaussNewtonConfig solverConfig;
        solverConfig.maxIterationCount = 50;
        solverConfig.parameterTolerance = 1e-8;
        solverConfig.residualTolerance = 1e-8;
        solverConfig.initialParameters.values.resize(dataset.trueParameters.size());
        solverConfig.initialParameters[0] = theta0Edit0.getValue().r8Val();
        solverConfig.initialParameters[1] = theta0Edit1.getValue().r8Val();
        if (dataset.trueParameters.size() > 2) {
            solverConfig.initialParameters[2] = theta0Edit2.getValue().r8Val();
        }

        GaussNewtonResult solverResult = runSolverForModel(modelIndex, solverMethod, dataset, solverConfig);
        refreshPlots(modelIndex, dataset, solverResult);

        const std::string fitPlotPath = "fit_plot.xml";
        const std::string convergencePlotPath = "convergence_plot.xml";
        fitPlotView.save(fitPlotPath.c_str());
        convergencePlotView.save(convergencePlotPath.c_str());
        exportRunReport(
            dataset,
            solverResult,
            solverMethod == SolverMethod::Newton ? "newton" : "gauss_newton",
            noiseSigma,
            fitPlotPath,
            convergencePlotPath,
            "run_export.txt");

        std::ostringstream statusStream;
        statusStream << "Status: iterations=" << solverResult.iterationCount
                     << " final_residual_norm=" << std::fixed << std::setprecision(6)
                     << solverResult.residualNormHistory.back();
        statusLabel.setTitle(statusStream.str());

        std::ostringstream logStream;
        logStream << "model=" << dataset.modelName
                  << " method=" << (solverMethod == SolverMethod::Newton ? "newton" : "gauss_newton")
                  << " noise_sigma=" << noiseSigma
                  << " iterations=" << solverResult.iterationCount
                  << " final_residual_norm=" << solverResult.residualNormHistory.back()
                  << " theta_hat=";
        for (size_t parameterIndex = 0; parameterIndex < solverResult.finalParameters.size(); parameterIndex++) {
            logStream << solverResult.finalParameters[parameterIndex] << " ";
        }
        logStream << "\n";
        logTextEdit.appendString(logStream.str().c_str());
        return true;
    }

    if (button == &exportLogButton) {
        std::ofstream outputFile("fit_log.txt");
        outputFile << logTextEdit.getText().c_str();
        statusLabel.setTitle("Status: log exported to fit_log.txt");
        return true;
    }

    return false;
}
