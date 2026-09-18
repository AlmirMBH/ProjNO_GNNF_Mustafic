#include "LogisticModel.h"

#include <cmath>

int LogisticModel::getParameterCount() const {
    return 3;
}

double LogisticModel::evaluateFunction(double timePoint, const Vec& parameters) const {
    double capacity = parameters[0];
    double growthRate = parameters[1];
    double midpointTime = parameters[2];
    double exponentArgument = -growthRate * (timePoint - midpointTime);
    return capacity / (1.0 + std::exp(exponentArgument));
}

void LogisticModel::fillJacobianRow(double timePoint, const Vec& parameters, dense::DblMatrix& jacobianMatrix, int rowIndex) const {
    double capacity = parameters[0];
    double growthRate = parameters[1];
    double midpointTime = parameters[2];
    double exponentArgument = -growthRate * (timePoint - midpointTime);
    double exponentialTerm = std::exp(exponentArgument);
    double denominator = 1.0 + exponentialTerm;
    double sigmoidValue = 1.0 / denominator;
    double sigmoidDerivativeFactor = sigmoidValue * (1.0 - sigmoidValue);
    double timeOffset = timePoint - midpointTime;
    auto jacobianWriter = jacobianMatrix.getManipulator();
    jacobianWriter(rowIndex, 0) = -sigmoidValue;
    jacobianWriter(rowIndex, 1) = -capacity * sigmoidDerivativeFactor * timeOffset;
    jacobianWriter(rowIndex, 2) = capacity * sigmoidDerivativeFactor * growthRate;
}

void LogisticModel::fillResidualHessian(double timePoint, const Vec& parameters, dense::DblMatrix& hessianMatrix) const {
    double capacity = parameters[0];
    double growthRate = parameters[1];
    double midpointTime = parameters[2];
    double exponentArgument = -growthRate * (timePoint - midpointTime);
    double exponentialTerm = std::exp(exponentArgument);
    double denominator = 1.0 + exponentialTerm;
    double sigmoidValue = 1.0 / denominator;
    double sigmoidDerivativeFactor = sigmoidValue * (1.0 - sigmoidValue);
    double sigmoidSecondFactor = sigmoidDerivativeFactor * (1.0 - 2.0 * sigmoidValue);
    double timeOffset = timePoint - midpointTime;
    auto hessianWriter = hessianMatrix.getManipulator();
    hessianWriter(0, 0) = 0.0;
    hessianWriter(0, 1) = -sigmoidDerivativeFactor * timeOffset;
    hessianWriter(1, 0) = -sigmoidDerivativeFactor * timeOffset;
    hessianWriter(0, 2) = sigmoidDerivativeFactor * growthRate;
    hessianWriter(2, 0) = sigmoidDerivativeFactor * growthRate;
    hessianWriter(1, 1) = -capacity * timeOffset * timeOffset * sigmoidSecondFactor;
    hessianWriter(2, 2) = -capacity * growthRate * growthRate * sigmoidSecondFactor;
    hessianWriter(1, 2) = capacity * timeOffset * growthRate * sigmoidSecondFactor + capacity * sigmoidDerivativeFactor;
    hessianWriter(2, 1) = capacity * timeOffset * growthRate * sigmoidSecondFactor + capacity * sigmoidDerivativeFactor;
}
