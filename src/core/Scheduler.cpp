#include "core/Scheduler.h"
#include "core/Sensor.h"
#include "core/RoomState.h"

#include <utility>

// ------------------------------------------------------------
// Singleton (static local, cree au premier appel)
// ------------------------------------------------------------

Scheduler& Scheduler::getInstance()
{
    static Scheduler instance;
    return instance;
}

// ------------------------------------------------------------
// Constructeur / destructeur prives
// ------------------------------------------------------------

Scheduler::Scheduler()
    : sensors()
    , currentTick(0)
    , startTime(SimClock::todayAt(START_HOUR))
{
}

Scheduler::~Scheduler() = default;

// ------------------------------------------------------------
// Gestion des capteurs
// ------------------------------------------------------------

void Scheduler::addSensor(std::unique_ptr<Sensor> sensor)
{
    if (sensor) {
        sensors.push_back(std::move(sensor));
    }
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
        // 1. On donne l'heure a la piece
        room.setDayContext(isDay(), getHour());

        // 2. La piece evolue (avant les mesures)
        room.update();

        // 3. Les capteurs mesurent le nouvel etat
        for (auto& sensor : sensors) {
            sensor->update();
        }

        ++currentTick;
    }
}

void Scheduler::reset()
{
    currentTick = 0;
    for (auto& sensor : sensors) {
        sensor->reset();
    }
}

// ------------------------------------------------------------
// Contexte temporel
// ------------------------------------------------------------

int Scheduler::getHour() const
{
    const int totalHours = currentTick / SimClock::TICKS_PER_HOUR;
    return (START_HOUR + totalHours) % 24;
}

bool Scheduler::isDay() const
{
    const int hour = getHour();
    return hour >= 6 && hour < 19;
}

SimClock::TimePoint Scheduler::getSimulatedTime() const
{
    return startTime + SimClock::ticksToDuration(currentTick);
}
