#include "core/RoomState.h"

#include <algorithm>
#include <cmath>

// ------------------------------------------------------------
// Singleton
// ------------------------------------------------------------

RoomState& RoomState::getInstance()
{
    static RoomState instance;
    return instance;
}

RoomState::RoomState()
    : co2(CO2_BASE)
    , temperature(TEMP_BASE)
    , humidity(HUM_BASE)
    , sound(35.0)
    , light(0.0)
    , occupants(0.0)
    , ventilationOn(false)
    , purifierOn(false)
    , isDay(true)
    , currentHour(6)
{
}

// ------------------------------------------------------------
// Contexte temporel
// ------------------------------------------------------------

void RoomState::setDayContext(bool day, int hour)
{
    isDay = day;
    currentHour = hour;
}

// ------------------------------------------------------------
// Mise a jour de la piece (1 tick = 36 s d'horloge)
// ------------------------------------------------------------

void RoomState::update()
{
    // --------------------------------------------------------
    // 1. Nombre d'occupants selon l'heure
    // --------------------------------------------------------
    if (isDay) {
        // Max vers 14h, au moins 2 personnes
        const double h = static_cast<double>(currentHour);
        const double factor = std::sin((h - 8.0) * 3.14159265 / 12.0);
        occupants = std::max(2.0, 5.0 + 8.0 * factor);
    }
    else {
        occupants = 0.5;   // gardien de securite la nuit
    }
    occupants = std::min(occupants, static_cast<double>(MAX_OCCUPANTS));

    // --------------------------------------------------------
    // 2. CO2
    // --------------------------------------------------------
    double co2Delta = occupants * CO2_PER_PERSON;

    // L'air se renouvelle un peu tout seul (plus il y a de CO2, plus ca sort)
    co2Delta -= (co2 - CO2_BASE) * CO2_LEAK_RATE;

    if (ventilationOn) {
        co2Delta -= CO2_VENT_RATE;
    }
    if (purifierOn) {
        co2Delta -= CO2_PURIF_RATE;
    }

    co2 += co2Delta;
    co2 = std::max(CO2_BASE, co2);

    // --------------------------------------------------------
    // 3. Temperature
    // --------------------------------------------------------
    const double targetTemp = ventilationOn ? TEMP_VENT
                            : (isDay ? TEMP_MAX : TEMP_NIGHT);
    temperature += (targetTemp - temperature) * 0.01;
    temperature += occupants * 0.002;

    // --------------------------------------------------------
    // 4. Humidite
    // --------------------------------------------------------
    double targetHum = HUM_BASE;
    if (ventilationOn) {
        targetHum = HUM_VENT;
    }
    else if (purifierOn) {
        targetHum = HUM_PURIF;
    }
    humidity += (targetHum - humidity) * 0.008;
    humidity += occupants * 0.005;
    humidity = std::clamp(humidity, HUM_MIN, HUM_MAX);

    // --------------------------------------------------------
    // 5. Son
    // --------------------------------------------------------
    const double soundBase = isDay ? 40.0 : 30.0;
    sound = soundBase + occupants * 1.5;

    // --------------------------------------------------------
    // 6. Lumiere
    // --------------------------------------------------------
    light = isDay ? 1.0 : 0.0;
}

// ------------------------------------------------------------
// Accesseurs
// ------------------------------------------------------------

double RoomState::getCO2()         const { return co2; }
double RoomState::getTemperature() const { return temperature; }
double RoomState::getHumidity()    const { return humidity; }
double RoomState::getSound()       const { return sound; }
double RoomState::getLight()       const { return light; }
double RoomState::getOccupants()   const { return occupants; }

// ------------------------------------------------------------
// Controle
// ------------------------------------------------------------

void RoomState::setVentilation(bool on) { ventilationOn = on; }
bool RoomState::isVentilationOn() const { return ventilationOn; }

void RoomState::setPurifier(bool on) { purifierOn = on; }
bool RoomState::isPurifierOn() const { return purifierOn; }

// ------------------------------------------------------------
// Reinitialisation
// ------------------------------------------------------------

void RoomState::reset()
{
    co2           = CO2_BASE;
    temperature   = TEMP_BASE;
    humidity      = HUM_BASE;
    sound         = 35.0;
    light         = 0.0;
    occupants     = 0.0;
    ventilationOn = false;
    purifierOn    = false;
    isDay         = true;
    currentHour   = 6;
}