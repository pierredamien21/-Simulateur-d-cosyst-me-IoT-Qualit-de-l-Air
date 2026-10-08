#include "core/AirPurifier.h"
#include "core/RoomState.h"

AirPurifier::AirPurifier() : active(false) {}

void AirPurifier::activate()
{
    active = true;
    RoomState::getInstance().setPurifier(true);
}

void AirPurifier::deactivate()
{
    active = false;
    RoomState::getInstance().setPurifier(false);
}

bool AirPurifier::isActive() const { return active; }

std::string AirPurifier::getName() const { return "AirPurifier"; }
