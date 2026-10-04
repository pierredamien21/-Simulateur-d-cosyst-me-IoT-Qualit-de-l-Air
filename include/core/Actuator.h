#ifndef AP4A_ACTUATOR_H
#define AP4A_ACTUATOR_H

#include <string>

/**
 * @brief Interface abstraite d'un actionneur.
 *
 * Un actionneur est un composant capable de modifier
 * l'environnement en reponse a une decision du serveur
 * (ex : ventilation, purificateur d'air).
 *
 * Interface pure : toutes les methodes sont virtuelles pures.
 */
class Actuator {
public:
    virtual ~Actuator() = default;

    virtual void activate() = 0;
    virtual void deactivate() = 0;
    virtual bool isActive() const = 0;
    virtual std::string getName() const = 0;
};

#endif // AP4A_ACTUATOR_H