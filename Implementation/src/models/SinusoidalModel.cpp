#include "SinusoidalModel.h"

#include <cmath>

int SinusoidalModel::getParameterCount() const {
    return 3;
}

double SinusoidalModel::evaluateFunction(double timePoint, const Vec& parameters) const {
    double amplitude = parameters[0];
    double angularFrequency = parameters[1];
    double phase = parameters[2];
    return amplitude * std::sin(angularFrequency * timePoint + phase);
}

void SinusoidalModel::fillJacobianRow(double timePoint, const Vec& parameters, dense::DblMatrix& jacobianMatrix, int rowIndex) const {
    double amplitude = parameters[0];
    double angularFrequency = parameters[1];
    double phase = parameters[2];
    double phaseArgument = angularFrequency * timePoint + phase;
    double sineValue = std::sin(phaseArgument);
    double cosineValue = std::cos(phaseArgument);
    auto jacobianWriter = jacobianMatrix.getManipulator();
    jacobianWriter(rowIndex, 0) = -sineValue;
    jacobianWriter(rowIndex, 1) = -amplitude * timePoint * cosineValue;
    jacobianWriter(rowIndex, 2) = -amplitude * cosineValue;
}

void SinusoidalModel::fillResidualHessian(double timePoint, const Vec& parameters, dense::DblMatrix& hessianMatrix) const {
    double amplitude = parameters[0];
    double angularFrequency = parameters[1];
    double phase = parameters[2];
    double phaseArgument = angularFrequency * timePoint + phase;
    double sineValue = std::sin(phaseArgument);
    double cosineValue = std::cos(phaseArgument);
    auto hessianWriter = hessianMatrix.getManipulator();
    hessianWriter(0, 0) = 0.0;
    hessianWriter(0, 1) = -timePoint * cosineValue;
    hessianWriter(1, 0) = -timePoint * cosineValue;
    hessianWriter(0, 2) = -cosineValue;
    hessianWriter(2, 0) = -cosineValue;
    hessianWriter(1, 1) = amplitude * timePoint * timePoint * sineValue;
    hessianWriter(2, 2) = amplitude * sineValue;
    hessianWriter(1, 2) = amplitude * timePoint * sineValue;
    hessianWriter(2, 1) = amplitude * timePoint * sineValue;
}
