#ifndef AP4A_LIGHT_SENSOR_H
#define AP4A_LIGHT_SENSOR_H

#include "core/Sensor.h"

/**
 * @brief Capteur de lumiere (bool : 0 = nuit, 1 = jour).
 *
 * Represente par un double (Option A : value : double + unit).
 *  - 0.0 : pas de lumiere
 *  - 1.0 : lumiere detectee
 */
class LightSensor : public Sensor {
public:
    explicit LightSensor(Server* srv);

protected:
    double generateValue() override;
};

#endif // AP4A_LIGHT_SENSOR_H