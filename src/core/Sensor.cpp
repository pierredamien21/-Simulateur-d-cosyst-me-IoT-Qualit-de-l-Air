#include "core/Sensor.h"
#include "core/Server.h"
#include "core/SimClock.h"

#include <algorithm>   // std::transform
#include <cctype>      // std::tolower
#include <iomanip>     // std::fixed, std::setprecision
#include <sstream>     // std::ostringstream


// Membres statiques
int Sensor::counter = 0;
std::mt19937 Sensor::rng{ std::random_device{}() };


// Constructeur avec parametres
Sensor::Sensor(Server* srv, const std::string& type, const std::string& unit, int periodMs): id(++counter), type(type), unit(unit), value(0.0), periodMs(periodMs), elapsedMs(0), server(srv){}

// Forme canonique de Coplien

Sensor::Sensor(const Sensor& other): id(other.id), type(other.type), unit(other.unit), value(other.value), periodMs(other.periodMs), elapsedMs(other.elapsedMs), server(other.server){}

Sensor& Sensor::operator=(const Sensor& other){
    if (this == &other) {
        return *this;
    }

    // On garde notre id, chaque capteur a le sien.
    type = other.type;
    unit = other.unit;
    value = other.value;
    periodMs = other.periodMs;
    elapsedMs = other.elapsedMs;
    server = other.server;

    return *this;
}

Sensor::~Sensor() = default;

// Comportement dynamique

void Sensor::update(){
    elapsedMs += SimClock::TICK_MS;

    if (elapsedMs >= periodMs) {
        execute();
        elapsedMs = 0;
    }
}

void Sensor::execute()
{
    value = generateValue();

    if (server) {
        server->receive(*this);   // c'est le serveur qui ecrit dans le CSV
    }
}

void Sensor::reset()
{
    value = 0.0;
    elapsedMs = 0;
}


// generateValue() est virtuelle pure, chaque capteur
// (TemperatureSensor, Co2Sensor...) a sa propre version.


// Conversion du type en minuscules

std::string Sensor::logKey() const {
    std::string key = type;
    std::transform(key.begin(), key.end(), key.begin(),[](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        });
    return key;
}

// Accesseurs

int Sensor::getId() const { return id; }
std::string Sensor::getType() const { return type; }
double Sensor::getValue() const { return value; }
std::string Sensor::getUnit() const { return unit; }
int Sensor::getPeriodMs() const { return periodMs; }


// Operateur de sortie

std::ostream& operator<<(std::ostream& os, const Sensor& s){
    os << "Sensor [id=" << s.id << ", type=" << s.type << ", value=" << s.value << ", unit=" << s.unit << ", period=" << s.periodMs << "ms]";
    return os;
}