#pragma once

#include "Vec.h"
#include <string>

struct Dataset {
    Vec timePoints;
    Vec observations;
    Vec weights;
    bool hasWeights;
    std::string modelName;
    Vec trueParameters;
};
