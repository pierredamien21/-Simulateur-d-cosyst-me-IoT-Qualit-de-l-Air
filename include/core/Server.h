#ifndef AIRQ_SERVER_H
#define AIRQ_SERVER_H

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
class ServerObserver;

// Serveur central : il recoit les mesures des capteurs, les ecrit
// dans les CSV, les analyse et allume / eteint les actionneurs.
// Ce n'est pas un singleton : on le cree dans main() et on donne
// son adresse aux capteurs.
class Server {
public:
    // --------------------------------------------------------
    // Seuils de decision
    // Le purificateur se declenche en premier, la ventilation
    // ensuite. Seuil ON et seuil OFF differents pour eviter
    // que ca clignote.
    // --------------------------------------------------------
    static constexpr double CO2_CRITICAL      = 1000.0;  // ppm
    static constexpr double TTT_HORIZON_MIN   = 240.0;   // au dela on ne fait pas confiance a la prevision
    static constexpr double VENT_ON_TTT_MIN   = 60.0;    // minutes avant le seuil
    static constexpr double VENT_ON_AQI       = 50.0;
    static constexpr double VENT_OFF_CO2      = 650.0;   // ppm
    static constexpr double VENT_OFF_AQI      = 60.0;
    static constexpr double PURIFIER_ON_AQI   = 52.0;
    static constexpr double PURIFIER_OFF_AQI  = 62.0;

    // --------------------------------------------------------
    // Forme canonique de Coplien
    // --------------------------------------------------------
    Server();
    Server(const Server& other);
    Server& operator=(const Server& other);
    ~Server();   // pas "= default" ici a cause des unique_ptr sur types incomplets

    // --------------------------------------------------------
    // Services de base
    // --------------------------------------------------------
    void consoleWrite(const std::string& message);
    void fileWrite(const std::string& type, const std::string& line);
    void openLogs();
    void closeLogs();

    // Remet tout a zero (utilise par le bouton Reset)
    void reset();

    // --------------------------------------------------------
    // Capteurs / analyse / decision
    // --------------------------------------------------------
    void receive(const Sensor& sensor);   // appele par Sensor::execute()
    void process();                        // AQI + prevision + actionneurs
    void notify(const std::string& category, const std::string& msg);

    // --------------------------------------------------------
    // Observer
    // --------------------------------------------------------
    void attach(ServerObserver* observer);
    void detach(ServerObserver* observer);

    // --------------------------------------------------------
    // Strategy : on choisit les algos depuis l'exterieur
    // --------------------------------------------------------
    void setPredictor(std::unique_ptr<PredictiveModel> p);        // temps avant seuil
    void setAnomalyDetector(std::unique_ptr<PredictiveModel> d);  // z-score

    void addActuator(std::unique_ptr<Actuator> a);

    // --------------------------------------------------------
    // Accesseurs
    // --------------------------------------------------------
    std::size_t getLogCount() const;
    std::size_t getRecentMessageCount() const;
    const DataAnalyzer& getAnalyzer() const;
    std::size_t getActuatorCount() const;
    std::size_t getAnomalyCount() const;

    // En minutes, -1 si pas de depassement prevu
    double getLastTTT() const;
    bool   isActuatorActive(const std::string& name) const;

    // Dossier logs/ a la racine du projet
    static std::string logDir();

    friend std::ostream& operator<<(std::ostream& os, const Server& srv);

private:
    // "CO2" -> "co2"
    static std::string normalize(const std::string& type);

    // Heure simulee au format ISO 8601
    static std::string timestamp();

    // Garde le message dans l'historique (1000 max)
    void remember(const std::string& message);

    void logActuatorState(const std::string& name, bool active);

    // Nombre de mesures CO2 -> minutes simulees
    double measuresToMinutes(double measures) const;

    std::map<std::string, std::ofstream>       logFiles;
    std::vector<std::string>                   recentMessages;
    DataAnalyzer                               analyzer;
    std::unique_ptr<PredictiveModel>           predictor;
    std::unique_ptr<PredictiveModel>           anomalyDetector;
    std::vector<std::unique_ptr<Actuator>>     actuators;
    std::vector<ServerObserver*>               observers;      // on ne les possede pas
    std::map<std::string, int>                 samplePeriodMs; // periode de chaque capteur
    std::map<std::string, bool>                lastRejected;   // la mesure d'avant a ete rejetee ?
    double                                     lastTTT;        // en minutes
    bool                                       co2Critical;    // pour n'alerter qu'au franchissement
    std::size_t                                anomalyCount;
};

#endif // AIRQ_SERVER_H
