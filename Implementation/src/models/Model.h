#pragma once

#include "data/Dataset.h"
#include "data/Vec.h"

#include <dense/Matrix.h>

class Model {
public:
    virtual ~Model() {}

    virtual int getParameterCount() const = 0;
    virtual double evaluateFunction(double timePoint, const Vec& parameters) const = 0;
    virtual void fillJacobianRow(double timePoint, const Vec& parameters, dense::DblMatrix& jacobianMatrix, int rowIndex) const = 0;
    virtual void fillResidualHessian(double timePoint, const Vec& parameters, dense::DblMatrix& hessianMatrix) const = 0;

    void computeResidual(const Dataset& dataset, const Vec& parameters, dense::DblMatrix& residualVector) const;
    void computeJacobian(const Dataset& dataset, const Vec& parameters, dense::DblMatrix& jacobianMatrix) const;
};
