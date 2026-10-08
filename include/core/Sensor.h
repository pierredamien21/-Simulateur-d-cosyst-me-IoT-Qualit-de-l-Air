#ifndef AIRQ_SENSOR_H
#define AIRQ_SENSOR_H

#include <ostream>
#include <random>
#include <string>

class Server;

// Classe de base de tous les capteurs (abstraite).
// Chaque capteur a sa propre periode : le Scheduler appelle
// update() a chaque tick et le capteur ne mesure que quand
// sa periode est ecoulee.
// Les classes filles n'ont qu'a ecrire generateValue().
class Sensor {
public:
    Sensor(Server* srv,
        const std::string& type,
        const std::string& unit,
        int periodMs);

    // Forme canonique de Coplien
    Sensor(const Sensor& other);
    Sensor& operator=(const Sensor& other);
    virtual ~Sensor();   // virtuel pour detruire via un Sensor*

    virtual void update();    // a chaque tick
    virtual void execute();   // quand la periode est ecoulee
    virtual void reset();

    int getId() const;
    std::string getType() const;
    double getValue() const;
    std::string getUnit() const;
    int getPeriodMs() const;


    friend std::ostream& operator<<(std::ostream& os, const Sensor& s);

protected:
    // A definir dans chaque capteur
    virtual double generateValue() = 0;

    // "Temperature" -> "temperature" (nom du fichier CSV)
    std::string logKey() const;


    static int counter;            // pour donner un id unique
    static std::mt19937 rng;       // generateur aleatoire partage

    int id;
    std::string type;
    std::string unit;
    double value;
    int periodMs;
    int elapsedMs;
    Server* server;            // le capteur ne possede pas le serveur
};

#endif // AIRQ_SENSOR_H
