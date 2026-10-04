#ifndef AP4A_CO2_SENSOR_H
#define AP4A_CO2_SENSOR_H

#include "core/Sensor.h"

/**
 * @brief Capteur de CO2 (400-1500 ppm).
 *
 * Distribution NORMALE centree sur 800 ppm (ecart-type 150),
 * tronquee dans [400, 1500]. Realiste pour un espace de travail.
 */
class Co2Sensor : public Sensor {
public:
    explicit Co2Sensor(Server* srv);

protected:
    double generateValue() override;
};

#endif // AP4A_CO2_SENSOR_H