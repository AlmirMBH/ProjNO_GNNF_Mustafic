#include "ExponentialModel.h"

#include <cmath>

int ExponentialModel::getParameterCount() const {
    return 2;
}

double ExponentialModel::evaluateFunction(double timePoint, const Vec& parameters) const {
    double amplitude = parameters[0];
    double decayRate = parameters[1];
    return amplitude * std::exp(-decayRate * timePoint);
}

void ExponentialModel::fillJacobianRow(double timePoint, const Vec& parameters, dense::DblMatrix& jacobianMatrix, int rowIndex) const {
    double amplitude = parameters[0];
    double decayRate = parameters[1];
    double exponentialTerm = std::exp(-decayRate * timePoint);
    auto jacobianWriter = jacobianMatrix.getManipulator();
    jacobianWriter(rowIndex, 0) = -exponentialTerm;
    jacobianWriter(rowIndex, 1) = amplitude * timePoint * exponentialTerm;
}

void ExponentialModel::fillResidualHessian(double timePoint, const Vec& parameters, dense::DblMatrix& hessianMatrix) const {
    double amplitude = parameters[0];
    double decayRate = parameters[1];
    double exponentialTerm = std::exp(-decayRate * timePoint);
    auto hessianWriter = hessianMatrix.getManipulator();
    hessianWriter(0, 0) = 0.0;
    hessianWriter(0, 1) = timePoint * exponentialTerm;
    hessianWriter(1, 0) = timePoint * exponentialTerm;
    hessianWriter(1, 1) = -amplitude * timePoint * timePoint * exponentialTerm;
}
