#include "core/LightSensor.h"
#include "core/RoomState.h"

LightSensor::LightSensor(Server* srv)
    : Sensor(srv, "Light", "bool", 1500)
{
}

double LightSensor::generateValue()
{
    // Lit la luminosite de la piece (0.0 ou 1.0)
    return RoomState::getInstance().getLight();
}