#ifndef AIRQ_TEMPERATURE_SENSOR_H
#define AIRQ_TEMPERATURE_SENSOR_H

#include "core/Sensor.h"

// Capteur de temperature (C), une mesure par seconde

class TemperatureSensor : public Sensor {
public:
	explicit TemperatureSensor(Server* srv);

protected:
	double generateValue() override;

};
#endif // AIRQ_TEMPERATURE_SENSOR_H