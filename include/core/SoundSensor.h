#ifndef AP4A_SOUND_SENSOR_H
#define AP4A_SOUND_SENSOR_H

#include "core/Sensor.h"

/**
 * @brief Capteur sonore (30-90 dB).
 */
class SoundSensor : public Sensor {
public:
    explicit SoundSensor(Server* srv);

protected:
    double generateValue() override;
};

#endif // AP4A_SOUND_SENSOR_H