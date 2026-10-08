#ifndef AIRQ_ANOMALY_DETECTOR_MODEL_H
#define AIRQ_ANOMALY_DETECTOR_MODEL_H

#include "core/PredictiveModel.h"

// Detecte les mesures aberrantes avec un z-score.
// predict() renvoie le score de la derniere mesure
// (au dessus de Z_THRESHOLD = aberrant), ou -1 s'il n'y a
// pas encore assez de valeurs.
class AnomalyDetectorModel : public PredictiveModel {
public:
    static constexpr double      Z_THRESHOLD = 4.0;
    static constexpr std::size_t MIN_SAMPLES = 6;

    double      predict(const std::deque<double>& history) const override;
    std::string getName() const override;
};

#endif // AIRQ_ANOMALY_DETECTOR_MODEL_H
