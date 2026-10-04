#ifndef AP4A_AIR_PURIFIER_H
#define AP4A_AIR_PURIFIER_H

#include "core/Actuator.h"

/**
 * @brief Purificateur d'air.
 *
 * Active lorsque l'AQI est degrade ou qu'une anomalie
 * est detectee (decision du Server).
 */
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

#endif // AP4A_AIR_PURIFIER_H