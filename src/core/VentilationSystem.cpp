#include "core/VentilationSystem.h"
#include "core/RoomState.h"

VentilationSystem::VentilationSystem()
    : active(false)
{
}

void VentilationSystem::activate()
{
    active = true;
    RoomState::getInstance().setVentilation(true);   // <-- AJOUT
}

void VentilationSystem::deactivate()
{
    active = false;
    RoomState::getInstance().setVentilation(false);  // <-- AJOUT
}

bool VentilationSystem::isActive() const
{
    return active;
}

std::string VentilationSystem::getName() const
{
    return "VentilationSystem";
}