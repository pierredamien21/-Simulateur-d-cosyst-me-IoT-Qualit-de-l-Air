#ifndef AIRQ_AIR_PURIFIER_H
#define AIRQ_AIR_PURIFIER_H

#include "core/Actuator.h"

// Purificateur d'air : premiere reaction quand l'AQI baisse.
// Il fait un peu baisser le CO2 et l'humidite de la piece.
class AirPurifier : public Actuator {
public:
    AirPurifier();

    void activate()   override;
    void deactivate() override;
    bool isActive() const override;
    std::string getName() const override;

private:
    bool active;
};

#endif // AIRQ_AIR_PURIFIER_H