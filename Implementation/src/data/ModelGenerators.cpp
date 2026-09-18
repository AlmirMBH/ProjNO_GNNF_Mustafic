#include "ModelGenerators.h"

#include "Rng.h"

#include <cmath>

static void fillTimePoints(Dataset& dataset, double timeStart, double timeEnd, int pointCount) {
    dataset.timePoints.values.resize(pointCount);
    if (pointCount == 1) {
        dataset.timePoints[0] = timeStart;
        return;
    }
    double timeStep = (timeEnd - timeStart) / (pointCount - 1);
    for (int pointIndex = 0; pointIndex < pointCount; pointIndex++) {
        dataset.timePoints[pointIndex] = timeStart + pointIndex * timeStep;
    }
}

void generateExponentialDataset(Dataset& dataset, double amplitude, double decayRate,
    double timeStart, double timeEnd, int pointCount, double noiseSigma, unsigned int seed) {
    seedRng(seed);
    fillTimePoints(dataset, timeStart, timeEnd, pointCount);

    dataset.observations.values.resize(pointCount);
    dataset.weights.values.resize(pointCount);
    dataset.hasWeights = noiseSigma > 0.0;

    for (int pointIndex = 0; pointIndex < pointCount; pointIndex++) {
        double timeValue = dataset.timePoints[pointIndex];
        double cleanValue = amplitude * std::exp(-decayRate * timeValue);
        dataset.observations[pointIndex] = cleanValue + gaussianNoise(noiseSigma);
        dataset.weights[pointIndex] = noiseSigma;
    }

    dataset.modelName = "exponential";
    dataset.trueParameters.values = {amplitude, decayRate};
}

void generateLogisticDataset(Dataset& dataset, double capacity, double growthRate, double midpointTime,
    double timeStart, double timeEnd, int pointCount, double noiseSigma, unsigned int seed) {
    seedRng(seed);
    fillTimePoints(dataset, timeStart, timeEnd, pointCount);

    dataset.observations.values.resize(pointCount);
    dataset.weights.values.resize(pointCount);
    dataset.hasWeights = noiseSigma > 0.0;

    for (int pointIndex = 0; pointIndex < pointCount; pointIndex++) {
        double timeValue = dataset.timePoints[pointIndex];
        double cleanValue = capacity / (1.0 + std::exp(-growthRate * (timeValue - midpointTime)));
        dataset.observations[pointIndex] = cleanValue + gaussianNoise(noiseSigma);
        dataset.weights[pointIndex] = noiseSigma;
    }

    dataset.modelName = "logistic";
    dataset.trueParameters.values = {capacity, growthRate, midpointTime};
}

void generateSinusoidalDataset(Dataset& dataset, double amplitude, double angularFrequency, double phase,
    double timeStart, double timeEnd, int pointCount, double noiseSigma, unsigned int seed) {
    seedRng(seed);
    fillTimePoints(dataset, timeStart, timeEnd, pointCount);

    dataset.observations.values.resize(pointCount);
    dataset.weights.values.resize(pointCount);
    dataset.hasWeights = noiseSigma > 0.0;

    for (int pointIndex = 0; pointIndex < pointCount; pointIndex++) {
        double timeValue = dataset.timePoints[pointIndex];
        double cleanValue = amplitude * std::sin(angularFrequency * timeValue + phase);
        dataset.observations[pointIndex] = cleanValue + gaussianNoise(noiseSigma);
        dataset.weights[pointIndex] = noiseSigma;
    }

    dataset.modelName = "sinusoidal";
    dataset.trueParameters.values = {amplitude, angularFrequency, phase};
}
