#ifndef AIRQ_SIM_CLOCK_H
#define AIRQ_SIM_CLOCK_H

#include <chrono>
#include <string>

// Tout ce qui touche au temps de la simulation.
// 1 tick = 100 ms pour les capteurs, mais l'horloge de la piece
// va 360 fois plus vite : 1 tick = 36 s, donc 1 h = 100 ticks
// et une journee = 2400 ticks. Sinon il faudrait attendre
// 24 h pour voir passer une journee.
class SimClock {
public:
    using Clock     = std::chrono::system_clock;
    using TimePoint = Clock::time_point;

    static constexpr int TICK_MS        = 100;
    static constexpr int ACCELERATION   = 360;
    static constexpr int SECONDS_PER_TICK = TICK_MS * ACCELERATION / 1000;  // 36 s
    static constexpr int TICKS_PER_HOUR = 3600 / SECONDS_PER_TICK;          // 100
    static constexpr int TICKS_PER_DAY  = 24 * TICKS_PER_HOUR;              // 2400

    SimClock() = delete;   // que des methodes statiques

    // Aujourd'hui a hour:00:00 (heure locale)
    static TimePoint todayAt(int hour);

    static std::chrono::seconds ticksToDuration(long long ticks);

    // ex : 2026-10-08T14:35:12
    static std::string toIso8601(TimePoint tp);

    // ex : 14:35
    static std::string toHourMinute(TimePoint tp);
};

#endif // AIRQ_SIM_CLOCK_H
