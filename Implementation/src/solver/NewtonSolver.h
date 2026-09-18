#pragma once

#include "GaussNewtonSolver.h"

GaussNewtonResult runNewton(const Model& model, const Dataset& dataset, const GaussNewtonConfig& config);
