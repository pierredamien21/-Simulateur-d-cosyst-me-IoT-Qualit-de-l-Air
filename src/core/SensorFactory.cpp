#include "core/SensorFactory.h"

#include "core/TemperatureSensor.h"
#include "core/HumiditySensor.h"
#include "core/Co2Sensor.h"
#include "core/SoundSensor.h"
#include "core/LightSensor.h"

std::unique_ptr<Sensor> SensorFactory::create(const std::string& type, Server* srv)
{
    if (type == "Temperature") {
        return std::make_unique<TemperatureSensor>(srv);
    }
    if (type == "Humidity") {
        return std::make_unique<HumiditySensor>(srv);
    }
    if (type == "CO2") {
        return std::make_unique<Co2Sensor>(srv);
    }
    if (type == "Sound") {
        return std::make_unique<SoundSensor>(srv);
    }
    if (type == "Light") {
        return std::make_unique<LightSensor>(srv);
    }
    return nullptr;   // type inconnu
}