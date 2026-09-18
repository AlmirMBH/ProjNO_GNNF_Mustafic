#include "NewtonSolver.h"

#include <cmath>

static double computeResidualNorm(const dense::DblMatrix& residualVector) {
    double sumSquares = 0.0;
    auto residualReader = residualVector.getManipulator();
    for (td::UINT4 rowIndex = 0; rowIndex < residualVector.getNoOfRows(); rowIndex++) {
        double residualValue = residualReader(rowIndex, 0);
        sumSquares += residualValue * residualValue;
    }
    return std::sqrt(sumSquares);
}

static double computeMaxAbsoluteValue(const dense::DblMatrix& vectorMatrix) {
    double maxAbsoluteValue = 0.0;
    auto vectorReader = vectorMatrix.getManipulator();
    for (td::UINT4 rowIndex = 0; rowIndex < vectorMatrix.getNoOfRows(); rowIndex++) {
        double absoluteValue = std::fabs(vectorReader(rowIndex, 0));
        if (absoluteValue > maxAbsoluteValue) {
            maxAbsoluteValue = absoluteValue;
        }
    }
    return maxAbsoluteValue;
}

GaussNewtonResult runNewton(const Model& model, const Dataset& dataset, const GaussNewtonConfig& config) {
    GaussNewtonResult result;
    result.finalParameters = config.initialParameters;
    result.iterationCount = 0;
    result.failed = false;

    int pointCount = (int)dataset.timePoints.size();
    int parameterCount = model.getParameterCount();
    Vec currentParameters = config.initialParameters;

    dense::DblMatrix residualVector;
    model.computeResidual(dataset, currentParameters, residualVector);
    result.residualNormHistory.push_back(computeResidualNorm(residualVector));
    result.parameterHistory.push_back(currentParameters);

    for (int iterationIndex = 0; iterationIndex < config.maxIterationCount; iterationIndex++) {
        dense::DblMatrix jacobianMatrix;
        model.computeJacobian(dataset, currentParameters, jacobianMatrix);

        dense::DblMatrix jacobianTranspose = jacobianMatrix.transpose();
        dense::DblMatrix hessianMatrix(parameterCount, parameterCount);
        hessianMatrix.fromMult(jacobianTranspose, jacobianMatrix, "N", "N");

        auto residualReader = residualVector.getManipulator();
        for (int pointIndex = 0; pointIndex < pointCount; pointIndex++) {
            dense::DblMatrix residualHessian(parameterCount, parameterCount);
            model.fillResidualHessian(dataset.timePoints[pointIndex], currentParameters, residualHessian);
            residualHessian *= residualReader(pointIndex, 0);
            hessianMatrix += residualHessian;
        }

        dense::DblMatrix gradientVector(parameterCount, 1);
        jacobianTranspose.gemv(residualVector, gradientVector);
        auto gradientWriter = gradientVector.getManipulator();
        for (int parameterIndex = 0; parameterIndex < parameterCount; parameterIndex++) {
            gradientWriter(parameterIndex, 0) = -gradientWriter(parameterIndex, 0);
        }

        dense::DblMatrix hessianMatrixCopy = hessianMatrix.makeCopy();
        if (!hessianMatrixCopy.solve(gradientVector)) {
            result.failed = true;
            break;
        }

        double maxParameterDelta = computeMaxAbsoluteValue(gradientVector);
        auto deltaReader = gradientVector.getManipulator();
        for (int parameterIndex = 0; parameterIndex < parameterCount; parameterIndex++) {
            currentParameters[parameterIndex] += deltaReader(parameterIndex, 0);
        }

        result.iterationCount = iterationIndex + 1;
        result.finalParameters = currentParameters;
        result.parameterHistory.push_back(currentParameters);

        model.computeResidual(dataset, currentParameters, residualVector);
        double residualNorm = computeResidualNorm(residualVector);
        result.residualNormHistory.push_back(residualNorm);

        if (maxParameterDelta < config.parameterTolerance) {
            break;
        }
        if (residualNorm < config.residualTolerance) {
            break;
        }
    }

    if (result.iterationCount >= config.maxIterationCount
        && result.residualNormHistory.back() >= config.residualTolerance) {
        result.failed = true;
    }

    return result;
}
