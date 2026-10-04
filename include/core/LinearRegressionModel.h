#ifndef AP4A_LINEAR_REGRESSION_MODEL_H
#define AP4A_LINEAR_REGRESSION_MODEL_H

#include "core/PredictiveModel.h"

class LinearRegressionModel : public PredictiveModel {
public:
    static constexpr double      THRESHOLD = 1000.0;
    static constexpr std::size_t MIN_SAMPLES = 5;

    double      predict(const std::deque<double>& history) const override;
    std::string getName() const override;
};

#endif // AP4A_LINEAR_REGRESSION_MODEL_H