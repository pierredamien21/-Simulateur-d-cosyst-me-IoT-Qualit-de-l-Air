#ifndef AIRQ_HUMIDITY_SENSOR_H
#define AIRQ_HUMIDITY_SENSOR_H

#include "core/Sensor.h"

// Capteur d'humidite (%), une mesure toutes les 2 s

class HumiditySensor : public Sensor {
public:
    explicit HumiditySensor(Server* srv);

protected:
    double generateValue() override;
};

#endif // AIRQ_HUMIDITY_SENSOR_H