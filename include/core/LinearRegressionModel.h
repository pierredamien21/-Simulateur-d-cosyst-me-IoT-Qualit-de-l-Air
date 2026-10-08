#ifndef AIRQ_LINEAR_REGRESSION_MODEL_H
#define AIRQ_LINEAR_REGRESSION_MODEL_H

#include "core/PredictiveModel.h"

// Prevoit dans combien de mesures le CO2 atteindra THRESHOLD
// en tracant une droite sur les dernieres valeurs.
class LinearRegressionModel : public PredictiveModel {
public:
    static constexpr double      THRESHOLD = 1000.0;
    static constexpr std::size_t MIN_SAMPLES = 10;   // en dessous la droite part dans tous les sens

    double      predict(const std::deque<double>& history) const override;
    std::string getName() const override;
};

#endif // AIRQ_LINEAR_REGRESSION_MODEL_H