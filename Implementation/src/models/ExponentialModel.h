#pragma once

#include "Model.h"

class ExponentialModel : public Model {
public:
    int getParameterCount() const override;
    double evaluateFunction(double timePoint, const Vec& parameters) const override;
    void fillJacobianRow(double timePoint, const Vec& parameters, dense::DblMatrix& jacobianMatrix, int rowIndex) const override;
    void fillResidualHessian(double timePoint, const Vec& parameters, dense::DblMatrix& hessianMatrix) const override;
};
