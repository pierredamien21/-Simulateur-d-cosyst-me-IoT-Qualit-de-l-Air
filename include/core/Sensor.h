#ifndef AP4A_SENSOR_H
#define AP4A_SENSOR_H

#include <ostream>
#include <random>
#include <string>

class Server; // forward declaration : Server est une classe

/**
 * @brief Capteur autonome du simulateur IoT (classe ABSTRAITE).
 *
 * Un capteur est un processus anime par le Scheduler. Il possede :
 *  - un identifiant unique (genere via compteur statique),
 *  - un type textuel ("Temperature", "CO2", ...),
 *  - une valeur courante (double, unite portee par 'unit'),
 *  - une base de temps interne (period / elapsed) qui determine
 *    la frequence reelle de ses mesures,
 *  - un pointeur vers le Server dedie (non proprietaire).
 *
 *  - update() et execute() gardent une implementation par defaut
 *    (logique de timing + formatage CSV) : pas de duplication.
 *  - generateValue() est PURE VIRTUELLE : chaque specialisation
 *    produit sa propre plage de valeurs (TemperatureSensor,
 *    Co2Sensor, etc.).
 */
class Sensor {
public:
    // Constructeur avec parametres
    Sensor(Server* srv,
        const std::string& type,
        const std::string& unit,
        int periodMs);

    // Forme canonique de Coplien
    Sensor(const Sensor& other);
    Sensor& operator=(const Sensor& other);
    virtual ~Sensor();   // virtuel : destruction polymorphe

    // Comportement dynamique
    virtual void update();    // appele par le Scheduler a chaque tick
    virtual void execute();   // appele quand elapsed >= period

    // Accesseurs (lecture seule)
    int getId() const;
    std::string getType() const;
    double getValue() const;
    std::string getUnit() const;
    int getPeriodMs() const;


    // Operateur de sortie (friend)
    friend std::ostream& operator<<(std::ostream& os, const Sensor& s);

protected:
    // Methode ABSTRAITE : chaque specialisation doit la definir
    // pour produire une valeur dans sa propre plage realiste.
    virtual double generateValue() = 0;

    // Conversion du type en minuscules ("Temperature" -> "temperature")
    // pour matcher la cle utilisee par Server::logFiles.
    std::string logKey() const;


    // Attributs
    static int counter;   // compteur global d'identifiants
    static std::mt19937 rng;       // generateur de nombres aleatoires

    int id;
    std::string type;
    std::string unit;
    double value;
    int periodMs;
    int elapsedMs;
    Server* server;            // pointeur non proprietaire
};

#endif // AP4A_SENSOR_H