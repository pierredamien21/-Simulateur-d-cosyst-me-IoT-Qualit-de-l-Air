#include "core/Scheduler.h"
#include "core/Sensor.h"
#include "core/RoomState.h"      // <-- AJOUT

#include <utility>

// ------------------------------------------------------------
// Singleton — idiome de Meyers
// ------------------------------------------------------------

Scheduler& Scheduler::getInstance()
{
    static Scheduler instance;   // cree au premier appel, detruit a la fin
    return instance;
}

// ------------------------------------------------------------
// Constructeur / destructeur prives
// ------------------------------------------------------------

Scheduler::Scheduler()
    : sensors()
    , currentTick(0)
{
}

Scheduler::~Scheduler() = default;

// ------------------------------------------------------------
// Gestion des capteurs
// ------------------------------------------------------------

void Scheduler::addSensor(std::unique_ptr<Sensor> sensor)
{
    sensors.push_back(std::move(sensor));
}

std::size_t Scheduler::getSensorCount() const
{
    return sensors.size();
}

int Scheduler::getCurrentTick() const
{
    return currentTick;
}

// ------------------------------------------------------------
// Boucle de simulation
// ------------------------------------------------------------

void Scheduler::simulation(int ticks)
{
    RoomState& room = RoomState::getInstance();

    for (int i = 0; i < ticks; ++i) {
        // 1. Mettre a jour le contexte temporel de la piece
        room.setDayContext(isDay(), getHour());

        // 2. Faire evoluer la physique de la piece AVANT les mesures
        room.update();

        // 3. Animer les capteurs (qui liront le nouvel etat)
        for (auto& sensor : sensors) {
            sensor->update();
        }

        ++currentTick;
    }
}

// ------------------------------------------------------------
// Contexte temporel
// ------------------------------------------------------------

int Scheduler::getHour() const
{
    const int totalHours = currentTick / TICKS_PER_HOUR;
    return (START_HOUR + totalHours) % 24;
}

bool Scheduler::isDay() const
{
    const int hour = getHour();
    return hour >= 6 && hour < 19;
}