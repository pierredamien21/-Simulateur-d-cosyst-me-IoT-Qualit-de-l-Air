#ifndef AP4A_TEMPERATURE_SENSOR_H
#define AP4A_TEMPERATURE_SENSOR_H

#include "core/Sensor.h"

/**
 * @brief Capteur de temperature (5-28 C).
 */

class TemperatureSensor : public Sensor {
public:
	explicit TemperatureSensor(Server* srv);

protected:
	double generateValue() override;

};
#endif // AP4A_TEMPERATURE_SENSOR_H