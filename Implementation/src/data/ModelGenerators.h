#pragma once

#include "Dataset.h"

void generateExponentialDataset(Dataset& dataset, double amplitude, double decayRate,
    double timeStart, double timeEnd, int pointCount, double noiseSigma, unsigned int seed);

void generateLogisticDataset(Dataset& dataset, double capacity, double growthRate, double midpointTime,
    double timeStart, double timeEnd, int pointCount, double noiseSigma, unsigned int seed);

void generateSinusoidalDataset(Dataset& dataset, double amplitude, double angularFrequency, double phase,
    double timeStart, double timeEnd, int pointCount, double noiseSigma, unsigned int seed);
