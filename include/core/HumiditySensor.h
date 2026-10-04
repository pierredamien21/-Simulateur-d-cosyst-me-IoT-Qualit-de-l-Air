#ifndef AP4A_HUMIDITY_SENSOR_H
#define AP4A_HUMIDITY_SENSOR_H

#include "core/Sensor.h"

/**
 * @brief Capteur d'humidite (30-70 %).
 */

class HumiditySensor : public Sensor {
public:
    explicit HumiditySensor(Server* srv);

protected:
    double generateValue() override;
};

#endif // AP4A_HUMIDITY_SENSOR_H