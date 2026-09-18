#include "ModelTests.h"

#include <cmath>
#include <iostream>

static double computeResidualNorm(const dense::DblMatrix& residualVector) {
    double sumSquares = 0.0;
    auto residualReader = residualVector.getManipulator();
    for (td::UINT4 rowIndex = 0; rowIndex < residualVector.getNoOfRows(); rowIndex++) {
        double residualValue = residualReader(rowIndex, 0);
        sumSquares += residualValue * residualValue;
    }
    return std::sqrt(sumSquares);
}

static double computeJacobianMaxError(const Model& model, const Dataset& dataset, const Vec& parameters, double epsilon) {
    int pointCount = (int)dataset.timePoints.size();
    int parameterCount = model.getParameterCount();

    dense::DblMatrix analyticJacobian;
    model.computeJacobian(dataset, parameters, analyticJacobian);
    auto analyticReader = analyticJacobian.getManipulator();

    double maxError = 0.0;
    Vec perturbedParameters = parameters;

    for (int parameterIndex = 0; parameterIndex < parameterCount; parameterIndex++) {
        perturbedParameters[parameterIndex] = parameters[parameterIndex] + epsilon;
        dense::DblMatrix residualPlus;
        model.computeResidual(dataset, perturbedParameters, residualPlus);
        auto plusReader = residualPlus.getManipulator();

        perturbedParameters[parameterIndex] = parameters[parameterIndex] - epsilon;
        dense::DblMatrix residualMinus;
        model.computeResidual(dataset, perturbedParameters, residualMinus);
        auto minusReader = residualMinus.getManipulator();

        perturbedParameters[parameterIndex] = parameters[parameterIndex];

        for (int pointIndex = 0; pointIndex < pointCount; pointIndex++) {
            double finiteDifferenceValue = (plusReader(pointIndex, 0) - minusReader(pointIndex, 0)) / (2.0 * epsilon);
            double analyticValue = analyticReader(pointIndex, parameterIndex);
            double errorValue = std::fabs(finiteDifferenceValue - analyticValue);
            if (errorValue > maxError) {
                maxError = errorValue;
            }
        }
    }

    return maxError;
}

static double computeHessianMaxError(const Model& model, const Dataset& dataset, const Vec& parameters, int pointIndex, double epsilon) {
    int parameterCount = model.getParameterCount();
    double timePoint = dataset.timePoints[pointIndex];

    dense::DblMatrix analyticHessian(parameterCount, parameterCount);
    model.fillResidualHessian(timePoint, parameters, analyticHessian);
    auto analyticReader = analyticHessian.getManipulator();

    double maxError = 0.0;
    Vec perturbedParameters = parameters;

    for (int rowParameterIndex = 0; rowParameterIndex < parameterCount; rowParameterIndex++) {
        for (int columnParameterIndex = rowParameterIndex; columnParameterIndex < parameterCount; columnParameterIndex++) {
            double hessianValue = 0.0;

            if (rowParameterIndex == columnParameterIndex) {
                perturbedParameters[rowParameterIndex] = parameters[rowParameterIndex] + epsilon;
                double residualPlus = dataset.observations[pointIndex] - model.evaluateFunction(timePoint, perturbedParameters);

                perturbedParameters[rowParameterIndex] = parameters[rowParameterIndex] - epsilon;
                double residualMinus = dataset.observations[pointIndex] - model.evaluateFunction(timePoint, perturbedParameters);

                perturbedParameters[rowParameterIndex] = parameters[rowParameterIndex];
                hessianValue = (residualPlus - 2.0 * (dataset.observations[pointIndex] - model.evaluateFunction(timePoint, parameters)) + residualMinus) / (epsilon * epsilon);
            } else {
                perturbedParameters[rowParameterIndex] = parameters[rowParameterIndex] + epsilon;
                perturbedParameters[columnParameterIndex] = parameters[columnParameterIndex] + epsilon;
                double residualPlusPlus = dataset.observations[pointIndex] - model.evaluateFunction(timePoint, perturbedParameters);

                perturbedParameters[rowParameterIndex] = parameters[rowParameterIndex] + epsilon;
                perturbedParameters[columnParameterIndex] = parameters[columnParameterIndex] - epsilon;
                double residualPlusMinus = dataset.observations[pointIndex] - model.evaluateFunction(timePoint, perturbedParameters);

                perturbedParameters[rowParameterIndex] = parameters[rowParameterIndex] - epsilon;
                perturbedParameters[columnParameterIndex] = parameters[columnParameterIndex] + epsilon;
                double residualMinusPlus = dataset.observations[pointIndex] - model.evaluateFunction(timePoint, perturbedParameters);

                perturbedParameters[rowParameterIndex] = parameters[rowParameterIndex] - epsilon;
                perturbedParameters[columnParameterIndex] = parameters[columnParameterIndex] - epsilon;
                double residualMinusMinus = dataset.observations[pointIndex] - model.evaluateFunction(timePoint, perturbedParameters);

                perturbedParameters[rowParameterIndex] = parameters[rowParameterIndex];
                perturbedParameters[columnParameterIndex] = parameters[columnParameterIndex];
                hessianValue = (residualPlusPlus - residualPlusMinus - residualMinusPlus + residualMinusMinus) / (4.0 * epsilon * epsilon);
            }

            double analyticValue = analyticReader(rowParameterIndex, columnParameterIndex);
            double errorValue = std::fabs(hessianValue - analyticValue);
            if (errorValue > maxError) {
                maxError = errorValue;
            }

            if (rowParameterIndex != columnParameterIndex) {
                double symmetricError = std::fabs(hessianValue - analyticReader(columnParameterIndex, rowParameterIndex));
                if (symmetricError > maxError) {
                    maxError = symmetricError;
                }
            }
        }
    }

    return maxError;
}

bool runModelTests(const Model& model, const Dataset& dataset, const Vec& testParameters) {
    const double epsilon = 1e-6;
    const double jacobianTolerance = 1e-4;
    const double hessianTolerance = 3e-3;
    const double residualTolerance = 1e-9;

    double jacobianMaxError = computeJacobianMaxError(model, dataset, testParameters, epsilon);
    int hessianPointIndex = (int)dataset.timePoints.size() / 2;
    double hessianMaxError = computeHessianMaxError(model, dataset, testParameters, hessianPointIndex, epsilon);

    dense::DblMatrix residualVector;
    model.computeResidual(dataset, testParameters, residualVector);
    double residualNorm = computeResidualNorm(residualVector);

    std::cout << "  jacobian_max_error=" << jacobianMaxError
              << " hessian_max_error=" << hessianMaxError
              << " residual_norm=" << residualNorm << std::endl;

    return jacobianMaxError < jacobianTolerance
        && hessianMaxError < hessianTolerance
        && residualNorm < residualTolerance;
}
