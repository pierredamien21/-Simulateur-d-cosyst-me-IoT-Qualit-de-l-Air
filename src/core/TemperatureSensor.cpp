#include "core/TemperatureSensor.h"
#include "core/RoomState.h"

#include <random>

TemperatureSensor::TemperatureSensor(Server* srv)
    : Sensor(srv, "Temperature", "C", 1000)
{
}

double TemperatureSensor::generateValue()
{
    // Lit la temperature de la piece + bruit de mesure (+- 0.3 C)
    std::normal_distribution<double> noise(0.0, 0.3);
    return RoomState::getInstance().getTemperature() + noise(rng);
}