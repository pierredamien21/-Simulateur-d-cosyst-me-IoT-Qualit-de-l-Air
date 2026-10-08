#include "core/SimClock.h"

#include <ctime>
#include <iomanip>
#include <sstream>

// ------------------------------------------------------------
// localtime() n'est pas thread-safe : version Windows / Linux
// ------------------------------------------------------------
static std::tm toLocalTm(std::time_t t)
{
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    return tm;
}

static std::string format(SimClock::TimePoint tp, const char* fmt)
{
    const std::tm tm = toLocalTm(SimClock::Clock::to_time_t(tp));
    std::ostringstream oss;
    oss << std::put_time(&tm, fmt);
    return oss.str();
}

// ------------------------------------------------------------
// Conversions
// ------------------------------------------------------------

SimClock::TimePoint SimClock::todayAt(int hour)
{
    std::tm tm = toLocalTm(Clock::to_time_t(Clock::now()));
    tm.tm_hour  = hour;
    tm.tm_min   = 0;
    tm.tm_sec   = 0;
    tm.tm_isdst = -1;   // mktime gere l'heure d'ete tout seul
    return Clock::from_time_t(std::mktime(&tm));
}

std::chrono::seconds SimClock::ticksToDuration(long long ticks)
{
    return std::chrono::seconds(ticks * SECONDS_PER_TICK);
}

std::string SimClock::toIso8601(TimePoint tp)
{
    return format(tp, "%Y-%m-%dT%H:%M:%S");
}

std::string SimClock::toHourMinute(TimePoint tp)
{
    return format(tp, "%H:%M");
}
