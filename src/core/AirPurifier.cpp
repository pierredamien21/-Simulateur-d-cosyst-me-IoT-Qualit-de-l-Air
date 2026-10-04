#include "core/AirPurifier.h"

AirPurifier::AirPurifier(): active(false) {}

void AirPurifier::activate() {active = true;}

void AirPurifier::deactivate() {active = false;}

bool AirPurifier::isActive() const { return active;}

std::string AirPurifier::getName() const {return "AirPurifier";}