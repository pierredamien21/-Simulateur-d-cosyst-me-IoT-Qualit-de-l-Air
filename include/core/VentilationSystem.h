#ifndef AIRQ_VENTILATION_SYSTEM_H
#define AIRQ_VENTILATION_SYSTEM_H

#include "core/Actuator.h"

// Ventilation : la grosse solution quand le CO2 monte trop
// (ou va bientot depasser le seuil).
class VentilationSystem : public Actuator {
public:
    VentilationSystem();

    void activate() override;
    void deactivate() override;
    bool isActive() const override;
    std::string getName() const override;

private:
    bool active;
};

#endif // AIRQ_VENTILATION_SYSTEM_H