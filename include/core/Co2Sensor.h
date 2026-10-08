#ifndef AIRQ_CO2_SENSOR_H
#define AIRQ_CO2_SENSOR_H

#include "core/Sensor.h"

// Capteur de CO2 (ppm), une mesure toutes les 0,5 s.
// De temps en temps il renvoie une valeur fausse (pic), pour
// verifier que le detecteur d'anomalies fait bien son travail.
class Co2Sensor : public Sensor {
public:
    static constexpr double GLITCH_PROBABILITY = 0.01;    // 1 mesure sur 100
    static constexpr double GLITCH_AMPLITUDE   = 400.0;   // ppm en trop

    explicit Co2Sensor(Server* srv);

protected:
    double generateValue() override;
};

#endif // AIRQ_CO2_SENSOR_H