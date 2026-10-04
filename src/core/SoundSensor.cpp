#include "core/SoundSensor.h"
#include "core/RoomState.h"

#include <random>

SoundSensor::SoundSensor(Server* srv)
    : Sensor(srv, "Sound", "dB", 800)
{
}

double SoundSensor::generateValue()
{
    // Lit le son de la piece + bruit de mesure (+- 2 dB)
    std::normal_distribution<double> noise(0.0, 2.0);
    return RoomState::getInstance().getSound() + noise(rng);
}