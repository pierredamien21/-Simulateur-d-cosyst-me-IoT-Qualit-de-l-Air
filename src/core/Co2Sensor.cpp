#include "core/Co2Sensor.h"
#include "core/RoomState.h"

#include <random>

Co2Sensor::Co2Sensor(Server* srv)
    : Sensor(srv, "CO2", "ppm", 500)
{
}

double Co2Sensor::generateValue()
{
    // Lit le CO2 de la piece + bruit de mesure (+- 15 ppm)
    std::normal_distribution<double> noise(0.0, 15.0);
    return RoomState::getInstance().getCO2() + noise(rng);
}