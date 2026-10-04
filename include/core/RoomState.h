#ifndef AP4A_ROOM_STATE_H
#define AP4A_ROOM_STATE_H

/**
 * @brief Etat physique de la piece (Singleton).
 *
 * Modelise les grandeurs physiques de l'espace de travail :
 *  - CO2 (ppm)
 *  - Temperature (C)
 *  - Humidite (%)
 *  - Son (dB)
 *  - Lumiere (0.0 / 1.0)
 *  - Nombre d'occupants
 *
 * Chaque tick :
 *  - les occupants evoluent selon l'heure simulee
 *  - le CO2 augmente (respiration) ou diminue (ventilation)
 *  - la temperature et l'humidite suivent des tendances
 *
 * Les CAPTEURS lisent cet etat (avec un peu de bruit) au lieu
 * de generer des valeurs purement aleatoires.
 *
 * Les ACTIONNEURS modifient cet etat via setVentilation().
 */
class RoomState {
public:
    // --------------------------------------------------------
    // Singleton
    // --------------------------------------------------------
    static RoomState& getInstance();

    RoomState(const RoomState&) = delete;
    RoomState& operator=(const RoomState&) = delete;

    // --------------------------------------------------------
    // Mise a jour physique (appelee par Scheduler chaque tick)
    // --------------------------------------------------------
    void update();

    // --------------------------------------------------------
    // Contexte temporel (pousse par Scheduler)
    // --------------------------------------------------------
    void setDayContext(bool day, int hour);

    // --------------------------------------------------------
    // Accesseurs (lus par les capteurs)
    // --------------------------------------------------------
    double getCO2()         const;
    double getTemperature() const;
    double getHumidity()    const;
    double getSound()       const;
    double getLight()       const;
    double getOccupants()   const;

    // --------------------------------------------------------
    // Controle (par les actionneurs)
    // --------------------------------------------------------
    void setVentilation(bool on);
    bool isVentilationOn() const;

private:
    RoomState();
    ~RoomState() = default;

    // --------------------------------------------------------
    // Etat physique
    // --------------------------------------------------------
    double co2;
    double temperature;
    double humidity;
    double sound;
    double light;
    double occupants;

    bool ventilationOn;

    // --------------------------------------------------------
    // Contexte temporel
    // --------------------------------------------------------
    bool isDay;
    int  currentHour;

    // --------------------------------------------------------
    // Constantes de simulation
    // --------------------------------------------------------
    static constexpr double CO2_BASE = 400.0;   // air exterieur (ppm)
    static constexpr double CO2_PER_PERSON = 0.15;    // ppm/tick par occupant
    static constexpr double CO2_VENT_RATE = 5.0;     // ppm/tick retires par ventilation
    static constexpr double CO2_LEAK = 0.1;     // fuite naturelle

    static constexpr double TEMP_BASE = 22.0;    // confort (C)
    static constexpr double TEMP_MAX = 26.0;    // sans ventilation
    static constexpr double TEMP_VENT = 20.0;    // avec ventilation

    static constexpr double HUM_BASE = 50.0;
    static constexpr double HUM_VENT = 40.0;

    static constexpr double HUM_MIN = 20.0;
    static constexpr double HUM_MAX = 80.0;

    static constexpr int    MAX_OCCUPANTS = 15;
};

#endif // AP4A_ROOM_STATE_H