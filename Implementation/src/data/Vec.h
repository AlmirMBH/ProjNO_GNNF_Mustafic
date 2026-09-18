#pragma once

#include <vector>

struct Vec {
    std::vector<double> values;

    size_t size() const {
        return values.size();
    }

    double& operator[](size_t index) {
        return values[index];
    }

    const double& operator[](size_t index) const {
        return values[index];
    }
};
