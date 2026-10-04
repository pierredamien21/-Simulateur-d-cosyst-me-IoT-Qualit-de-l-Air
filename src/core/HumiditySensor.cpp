#include "core/HumiditySensor.h"
#include "core/RoomState.h"

#include <random>

HumiditySensor::HumiditySensor(Server* srv)
    : Sensor(srv, "Humidity", "%", 2000)
{
}

double HumiditySensor::generateValue()
{
    // Lit l'humidite de la piece + bruit de mesure (+- 1.5 %)
    std::normal_distribution<double> noise(0.0, 1.5);
    return RoomState::getInstance().getHumidity() + noise(rng);
}