#ifndef AIRQ_LIGHT_SENSOR_H
#define AIRQ_LIGHT_SENSOR_H

#include "core/Sensor.h"

// Capteur de lumiere : 1 s'il fait jour, 0 sinon
// (stocke dans un double comme les autres capteurs)
class LightSensor : public Sensor {
public:
    explicit LightSensor(Server* srv);

protected:
    double generateValue() override;
};

#endif // AIRQ_LIGHT_SENSOR_H