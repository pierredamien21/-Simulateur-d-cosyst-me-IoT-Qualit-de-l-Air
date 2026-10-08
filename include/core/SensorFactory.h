#ifndef AIRQ_SENSOR_FACTORY_H
#define AIRQ_SENSOR_FACTORY_H

#include <memory>
#include <string>

class Sensor;
class Server;

// Factory : cree le bon capteur a partir de son nom
// ("Temperature", "Humidity", "CO2", "Sound", "Light").
// Renvoie nullptr si le nom n'existe pas.
class SensorFactory {
public:
    SensorFactory() = delete;   // que des methodes statiques

    static std::unique_ptr<Sensor> create(const std::string& type, Server* srv);
};

#endif // AIRQ_SENSOR_FACTORY_H