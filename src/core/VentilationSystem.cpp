#include "core/VentilationSystem.h"
#include "core/RoomState.h"

VentilationSystem::VentilationSystem()
    : active(false)
{
}

void VentilationSystem::activate()
{
    active = true;
    RoomState::getInstance().setVentilation(true);
}

void VentilationSystem::deactivate()
{
    active = false;
    RoomState::getInstance().setVentilation(false);
}

bool VentilationSystem::isActive() const
{
    return active;
}

std::string VentilationSystem::getName() const
{
    return "VentilationSystem";
}