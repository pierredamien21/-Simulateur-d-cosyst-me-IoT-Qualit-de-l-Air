#ifndef AIRQ_SCHEDULER_H
#define AIRQ_SCHEDULER_H

#include "core/SimClock.h"

#include <cstddef>
#include <memory>
#include <vector>

class Sensor;

// Ordonnanceur (singleton) : il fait avancer la simulation tick
// par tick, met a jour la piece puis tous les capteurs.
// C'est aussi lui qui donne l'heure simulee.
class Scheduler {
public:
    static Scheduler& getInstance();

    // Pas de copie pour un singleton
    Scheduler(const Scheduler& other) = delete;
    Scheduler& operator=(const Scheduler& other) = delete;

    void addSensor(std::unique_ptr<Sensor> sensor);

    // Avance de "ticks" pas de simulation
    void simulation(int ticks);

    // Remet l'heure au debut et les capteurs a zero
    void reset();

    // --------------------------------------------------------
    // Heure simulee
    // --------------------------------------------------------
    bool isDay()  const;
    int  getHour() const;
    SimClock::TimePoint getSimulatedTime() const;

    std::size_t getSensorCount() const;
    int         getCurrentTick() const;

private:
    Scheduler();
    ~Scheduler();

    static constexpr int START_HOUR = 6;   // la journee commence a 6h

    std::vector<std::unique_ptr<Sensor>> sensors;
    int                 currentTick;
    SimClock::TimePoint startTime;
};

#endif // AIRQ_SCHEDULER_H
