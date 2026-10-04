#ifndef AP4A_SENSOR_FACTORY_H
#define AP4A_SENSOR_FACTORY_H

#include <memory>
#include <string>

class Sensor;
class Server;

/**
 * @brief Fabrique de capteurs (pattern Factory).
 *
 * Cree dynamiquement une instance de Sensor (via ses
 * specialisations) a partir d'un type textuel :
 *   "Temperature", "Humidity", "CO2", "Sound", "Light"
 *
 * Retourne nullptr si le type est inconnu.
 */
class SensorFactory {
public:
    // Interdit l'instanciation (classe utilitaire)
    SensorFactory() = delete;

    // Methode statique de creation
    static std::unique_ptr<Sensor> create(const std::string& type, Server* srv);
};

#endif // AP4A_SENSOR_FACTORY_H