#ifndef AIRQ_ROOM_STATE_H
#define AIRQ_ROOM_STATE_H

// Etat "physique" de la piece : CO2, temperature, humidite,
// son, lumiere et nombre de personnes.
// Les capteurs lisent ces valeurs (en ajoutant un peu de bruit)
// et les actionneurs les modifient.
// Une seule piece -> singleton.
// 1 tick = 36 s d'horloge (voir SimClock).
class RoomState {
public:
    static RoomState& getInstance();

    RoomState(const RoomState&) = delete;
    RoomState& operator=(const RoomState&) = delete;

    // Fait evoluer la piece d'un tick (appele par le Scheduler)
    void update();

    // Retour a l'etat de depart
    void reset();

    // Le Scheduler donne l'heure et s'il fait jour
    void setDayContext(bool day, int hour);

    // --------------------------------------------------------
    // Lecture (capteurs)
    // --------------------------------------------------------
    double getCO2()         const;
    double getTemperature() const;
    double getHumidity()    const;
    double getSound()       const;
    double getLight()       const;
    double getOccupants()   const;

    // --------------------------------------------------------
    // Actionneurs
    // --------------------------------------------------------
    void setVentilation(bool on);
    bool isVentilationOn() const;

    void setPurifier(bool on);
    bool isPurifierOn() const;

private:
    RoomState();
    ~RoomState() = default;

    double co2;
    double temperature;
    double humidity;
    double sound;
    double light;
    double occupants;

    bool ventilationOn;
    bool purifierOn;

    bool isDay;
    int  currentHour;

    // --------------------------------------------------------
    // Reglages de la simulation (valeurs par tick)
    // --------------------------------------------------------
    static constexpr double CO2_BASE       = 400.0;   // air exterieur (ppm)
    static constexpr double CO2_PER_PERSON = 0.20;    // ppm ajoutes par personne
    static constexpr double CO2_VENT_RATE  = 5.0;     // ppm retires par la ventilation
    static constexpr double CO2_PURIF_RATE = 1.5;     // ppm retires par le purificateur
    static constexpr double CO2_LEAK_RATE  = 0.003;   // l'air se renouvelle un peu tout seul

    static constexpr double TEMP_BASE  = 22.0;
    static constexpr double TEMP_MAX   = 26.0;   // jour, sans ventilation
    static constexpr double TEMP_NIGHT = 19.0;   // nuit, sans ventilation
    static constexpr double TEMP_VENT  = 20.0;   // avec ventilation

    static constexpr double HUM_BASE  = 50.0;
    static constexpr double HUM_VENT  = 40.0;
    static constexpr double HUM_PURIF = 45.0;   // le purificateur asseche un peu

    static constexpr double HUM_MIN = 20.0;
    static constexpr double HUM_MAX = 80.0;

    static constexpr int    MAX_OCCUPANTS = 15;
};

#endif // AIRQ_ROOM_STATE_H
