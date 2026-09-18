#pragma once

#include "PlotUpdater.h"

#include <gui/View.h>
#include <gui/Button.h>
#include <gui/Label.h>
#include <gui/ComboBox.h>
#include <gui/Slider.h>
#include <gui/NumericEdit.h>
#include <gui/TextEdit.h>
#include <gui/VerticalLayout.h>
#include <gui/HorizontalLayout.h>
#include <gui/Font.h>
#include <gui/plot/View.h>

class MainView : public gui::View {
    struct PlotFonts {
        gui::Font axis;
        gui::Font legend;

        PlotFonts() {
            axis.create("Arial", 12.0f, gui::Font::Style::Normal, gui::Font::Unit::LogicalPixel);
            legend.create("Arial", 10.0f, gui::Font::Style::Normal, gui::Font::Unit::LogicalPixel);
        }
    };

protected:
    gui::ComboBox modelComboBox;
    gui::ComboBox methodComboBox;
    gui::Label noiseLabel;
    gui::Slider noiseSlider;
    gui::Label theta0Label0;
    gui::Label theta0Label1;
    gui::Label theta0Label2;
    gui::NumericEdit theta0Edit0;
    gui::NumericEdit theta0Edit1;
    gui::NumericEdit theta0Edit2;
    gui::Button runButton;
    gui::Button exportLogButton;
    gui::Label statusLabel;
    PlotFonts plotFonts;
    gui::plot::View fitPlotView;
    gui::plot::View convergencePlotView;
    gui::TextEdit logTextEdit;
    PlotDataBuffers plotDataBuffers;
    gui::VerticalLayout mainLayout;
    gui::HorizontalLayout theta0Layout;
    gui::HorizontalLayout buttonLayout;
    gui::HorizontalLayout plotLayout;

    void refreshPlots(int modelIndex, const Dataset& dataset, const GaussNewtonResult& solverResult);

public:
    MainView();
    bool onClick(gui::Button* button) override;
};
