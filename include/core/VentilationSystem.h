#ifndef AP4A_VENTILATION_SYSTEM_H
#define AP4A_VENTILATION_SYSTEM_H

#include "core/Actuator.h"

/**
 * @brief Systeme de ventilation.
 *
 * Active lorsque le CO2 depasse un seuil critique
 * ou que l'AQI se degrade (decision du Server).
 */
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

#endif // AP4A_VENTILATION_SYSTEM_H