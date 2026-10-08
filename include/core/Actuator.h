#ifndef AIRQ_ACTUATOR_H
#define AIRQ_ACTUATOR_H

#include <string>

// Interface commune aux actionneurs (ventilation, purificateur...).
// Le serveur les allume et les eteint selon ses decisions.
class Actuator {
public:
    virtual ~Actuator() = default;

    virtual void activate() = 0;
    virtual void deactivate() = 0;
    virtual bool isActive() const = 0;
    virtual std::string getName() const = 0;
};

#endif // AIRQ_ACTUATOR_H