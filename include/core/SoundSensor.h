#ifndef AIRQ_SOUND_SENSOR_H
#define AIRQ_SOUND_SENSOR_H

#include "core/Sensor.h"

// Capteur de bruit (dB)
class SoundSensor : public Sensor {
public:
    explicit SoundSensor(Server* srv);

protected:
    double generateValue() override;
};

#endif // AIRQ_SOUND_SENSOR_H