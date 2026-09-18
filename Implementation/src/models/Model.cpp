#include "Model.h"

void Model::computeResidual(const Dataset& dataset, const Vec& parameters, dense::DblMatrix& residualVector) const {
    int pointCount = (int)dataset.timePoints.size();
    residualVector.setDimension(pointCount, 1);
    auto residualWriter = residualVector.getManipulator();

    for (int pointIndex = 0; pointIndex < pointCount; pointIndex++) {
        double timePoint = dataset.timePoints[pointIndex];
        double modelValue = evaluateFunction(timePoint, parameters);
        residualWriter(pointIndex, 0) = dataset.observations[pointIndex] - modelValue;
    }
}

void Model::computeJacobian(const Dataset& dataset, const Vec& parameters, dense::DblMatrix& jacobianMatrix) const {
    int pointCount = (int)dataset.timePoints.size();
    int parameterCount = getParameterCount();
    jacobianMatrix.setDimension(pointCount, parameterCount);

    for (int pointIndex = 0; pointIndex < pointCount; pointIndex++) {
        fillJacobianRow(dataset.timePoints[pointIndex], parameters, jacobianMatrix, pointIndex);
    }
}
