#ifndef AP4A_SCHEDULER_H
#define AP4A_SCHEDULER_H

#include <cstddef>
#include <memory>
#include <vector>

class Sensor;

/**
 * @brief Ordonnanceur des processus capteurs (Singleton).
 *
 * Le Scheduler anime les capteurs via leur methode update()
 * selon une base de temps globale (1 tick = 100 ms simules).
 *
 * Singleton :
 *   - constructeur prive
 *   - methode statique getInstance() pour obtenir l'unique instance
 *   - copie et affectation supprimees
 */
class Scheduler {
public:
    // --------------------------------------------------------
    // Singleton : acces a l'instance unique
    // --------------------------------------------------------
    static Scheduler& getInstance();

    // Non copiable / non affectable
    Scheduler(const Scheduler& other) = delete;
    Scheduler& operator=(const Scheduler& other) = delete;

    // --------------------------------------------------------
    // Gestion des capteurs
    // --------------------------------------------------------
    void addSensor(std::unique_ptr<Sensor> sensor);

    // --------------------------------------------------------
    // Boucle de simulation
    // --------------------------------------------------------
    void simulation(int ticks);

    // --------------------------------------------------------
    // Contexte temporel
    // --------------------------------------------------------
    bool isDay()  const;
    int  getHour() const;

    // --------------------------------------------------------
    // Accesseurs
    // --------------------------------------------------------
    std::size_t getSensorCount() const;
    int         getCurrentTick() const;

private:
    // --------------------------------------------------------
    // Constructeur / destructeur prives (Singleton)
    // --------------------------------------------------------
    Scheduler();
    ~Scheduler();

    static constexpr int TICK_MS = 100;
    static constexpr int TICKS_PER_HOUR = 36000;
    static constexpr int START_HOUR = 6;

    std::vector<std::unique_ptr<Sensor>> sensors;
    int currentTick;
};

#endif // AP4A_SCHEDULER_H