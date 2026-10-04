#ifndef AP4A_SERVER_H
#define AP4A_SERVER_H

#include "core/DataAnalyzer.h"

#include <cstddef>
#include <fstream>
#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

class Sensor;
class Actuator;
class PredictiveModel;

/**
 * @brief Serveur central du simulateur IoT.
 *
 * Le Server est le chef d'orchestre :
 *  - recoit les mesures via receive()
 *  - alimente le DataAnalyzer
 *  - ecrit les logs CSV par type de capteur
 *  - analyse et predit via process()
 *  - declenche les actionneurs selon les decisions
 *  - notifie les evenements critiques (Observer)
 *
 * Forme canonique de Coplien : constructeur par defaut,
 * constructeur par copie, operateur d'affectation, destructeur.
 *
 * NOTE : Server n'est PAS un Singleton (contrainte Coplien).
 * Une instance unique est creee dans main() et partagee aux
 * capteurs via pointeur.
 */
class Server {
public:
    // --------------------------------------------------------
    // Forme canonique de Coplien
    // --------------------------------------------------------
    Server();
    Server(const Server& other);
    Server& operator=(const Server& other);
    ~Server();   // declare (pas = default) : unique_ptr incomplets

    // --------------------------------------------------------
    // Services de base
    // --------------------------------------------------------
    void consoleWrite(const std::string& message);
    void fileWrite(const std::string& type, const std::string& line);
    void openLogs();
    void closeLogs();

    // --------------------------------------------------------
    // Integration capteurs / analyse / decision
    // --------------------------------------------------------
    void receive(const Sensor& sensor);   // appele par Sensor::execute()
    void process();                        // analyse + prediction + decision
    void notify(const std::string& msg);   // alertes (Observer)

    // --------------------------------------------------------
    // Injection de strategie (pattern Strategy)
    // --------------------------------------------------------
    void setPredictor(std::unique_ptr<PredictiveModel> p);

    // --------------------------------------------------------
    // Enregistrement d'un actionneur (pattern Composition)
    // --------------------------------------------------------
    void addActuator(std::unique_ptr<Actuator> a);

    // --------------------------------------------------------
    // Accesseurs (lecture seule)
    // --------------------------------------------------------
    std::size_t getLogCount() const;
    std::size_t getRecentMessageCount() const;
    const DataAnalyzer& getAnalyzer() const;
    std::size_t getActuatorCount() const;

    // --------------------------------------------------------
    // Accesseurs supplementaires (utilises par l'IHM Qt)
    // --------------------------------------------------------
    double getLastTTT() const;                                    // <-- AJOUT
    bool   isActuatorActive(const std::string& name) const;       // <-- AJOUT

    // --------------------------------------------------------
    // Chemin du dossier de logs (racine du projet)
    // --------------------------------------------------------
    static std::string logDir();

    // --------------------------------------------------------
    // Operateur de sortie (friend)
    // --------------------------------------------------------
    friend std::ostream& operator<<(std::ostream& os, const Server& srv);

private:
    // Met une cle de type en minuscules ("CO2" -> "co2")
    static std::string normalize(const std::string& type);

    // Ecrit une ligne dans le CSV des actionneurs
    void logActuatorState(const std::string& name, bool active);

    std::map<std::string, std::ofstream>       logFiles;
    std::vector<std::string>                   recentMessages;
    DataAnalyzer                               analyzer;
    std::unique_ptr<PredictiveModel>           predictor;
    std::vector<std::unique_ptr<Actuator>>     actuators;
};

#endif // AP4A_SERVER_H