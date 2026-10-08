#include "core/Server.h"
#include "core/Config.h"

#include "core/Sensor.h"
#include "core/Actuator.h"
#include "core/PredictiveModel.h"
#include "core/AnomalyDetectorModel.h"
#include "core/ServerObserver.h"
#include "core/Scheduler.h"
#include "core/SimClock.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <utility>

// ------------------------------------------------------------
// Dossier des logs
// ------------------------------------------------------------
std::string Server::logDir()
{
    return std::string(AIRQ_PROJECT_ROOT) + "/logs";
}

// ------------------------------------------------------------
// "CO2" -> "co2"
// ------------------------------------------------------------
std::string Server::normalize(const std::string& type)
{
    std::string key = type;
    std::transform(key.begin(), key.end(), key.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return key;
}

// ------------------------------------------------------------
// Heure simulee (donnee par le Scheduler) au format ISO 8601
// ------------------------------------------------------------
std::string Server::timestamp()
{
    return SimClock::toIso8601(Scheduler::getInstance().getSimulatedTime());
}

// ------------------------------------------------------------
// Forme canonique de Coplien
// ------------------------------------------------------------

Server::Server()
    : logFiles()
    , recentMessages()
    , analyzer()
    , predictor(nullptr)
    , anomalyDetector(nullptr)
    , actuators()
    , observers()
    , samplePeriodMs()
    , lastRejected()
    , lastTTT(-1.0)
    , co2Critical(false)
    , anomalyCount(0)
{
}

Server::Server(const Server& other)
    : logFiles()                            // un ofstream ne se copie pas
    , recentMessages(other.recentMessages)
    , analyzer(other.analyzer)
    , predictor(nullptr)                    // unique_ptr : pas de copie
    , anomalyDetector(nullptr)
    , actuators()
    , observers()
    , samplePeriodMs(other.samplePeriodMs)
    , lastRejected(other.lastRejected)
    , lastTTT(other.lastTTT)
    , co2Critical(other.co2Critical)
    , anomalyCount(other.anomalyCount)
{
    // On copie les donnees d'analyse, mais pas les fichiers,
    // les algos, les actionneurs ni les observateurs.
}

Server& Server::operator=(const Server& other)
{
    if (this == &other) {
        return *this;
    }

    closeLogs();

    recentMessages = other.recentMessages;
    analyzer       = other.analyzer;
    samplePeriodMs = other.samplePeriodMs;
    lastRejected   = other.lastRejected;
    lastTTT        = other.lastTTT;
    co2Critical    = other.co2Critical;
    anomalyCount   = other.anomalyCount;
    // pas de copie des algos, actionneurs et observateurs

    return *this;
}

Server::~Server() = default;

// ------------------------------------------------------------
// Services de base
// ------------------------------------------------------------

void Server::remember(const std::string& message)
{
    recentMessages.push_back(message);

    constexpr std::size_t MAX_MESSAGES = 1000;
    if (recentMessages.size() > MAX_MESSAGES) {
        recentMessages.erase(
            recentMessages.begin(),
            recentMessages.begin() + (recentMessages.size() - MAX_MESSAGES)
        );
    }
}

void Server::consoleWrite(const std::string& message)
{
    std::cout << message << "\n";
    remember(message);
}

void Server::fileWrite(const std::string& type, const std::string& line)
{
    auto it = logFiles.find(type);
    if (it != logFiles.end() && it->second.is_open()) {
        it->second << line << "\n";
        it->second.flush();
    }
    else {
        std::cerr << "[Server] fileWrite : type inconnu ou flux ferme : "
            << type << "\n";
    }
}

void Server::openLogs()
{
    namespace fs = std::filesystem;

    const std::string dir = logDir();

    std::error_code ec;
    fs::create_directories(dir, ec);
    if (ec) {
        std::cerr << "[Server] Impossible de creer " << dir
            << " : " << ec.message() << "\n";
        return;
    }

    // Un fichier par capteur + 3 fichiers pour l'analyse
    const std::map<std::string, std::string> headers = {
        { "temperature", "timestamp,id,type,value,unit"  },
        { "humidity",    "timestamp,id,type,value,unit"  },
        { "co2",         "timestamp,id,type,value,unit"  },
        { "sound",       "timestamp,id,type,value,unit"  },
        { "light",       "timestamp,id,type,value,unit"  },
        { "predictions", "timestamp,kind,value,unit"     },
        { "actuators",   "timestamp,actuator,state"      },
        { "alerts",      "timestamp,category,message"    },
    };

    for (const auto& [type, header] : headers) {
        const std::string path = dir + "/" + type + ".csv";
        std::ofstream& ofs = logFiles[type];
        if (ofs.is_open()) {
            ofs.close();
        }
        ofs.open(path, std::ios::out | std::ios::trunc);
        if (ofs.is_open()) {
            ofs << header << "\n";
            ofs.flush();
        }
        else {
            std::cerr << "[Server] Impossible d'ouvrir " << path << "\n";
        }
    }
}

void Server::closeLogs()
{
    for (auto& [type, ofs] : logFiles) {
        if (ofs.is_open()) {
            ofs.flush();
            ofs.close();
        }
    }
}

void Server::reset()
{
    for (auto& act : actuators) {
        if (act->isActive()) {
            act->deactivate();
        }
    }

    recentMessages.clear();
    analyzer.clear();
    samplePeriodMs.clear();
    lastRejected.clear();
    lastTTT      = -1.0;
    co2Critical  = false;
    anomalyCount = 0;

    openLogs();   // repart avec des fichiers vides
}

// ------------------------------------------------------------
// Integration capteurs
// ------------------------------------------------------------

void Server::receive(const Sensor& sensor)
{
    const std::string key   = normalize(sensor.getType());
    const double      value = sensor.getValue();
    const std::string ts    = timestamp();

    samplePeriodMs[key] = sensor.getPeriodMs();

    // 1. On ecrit la mesure dans le CSV (meme si elle est fausse)
    {
        std::ostringstream oss;
        oss << ts
            << "," << sensor.getId()
            << "," << sensor.getType()
            << "," << std::fixed << std::setprecision(2) << value
            << "," << sensor.getUnit();
        fileWrite(key, oss.str());
    }

    // 2. Mesure aberrante ? (pas pour la lumiere qui vaut 0 ou 1)
    //    Si la mesure d'avant a deja ete rejetee et que celle-ci
    //    saute encore, c'est que ca a vraiment change : on garde.
    if (anomalyDetector && sensor.getUnit() != "bool") {
        std::deque<double> candidate = analyzer.getHistory(key);
        candidate.push_back(value);
        const double z = anomalyDetector->predict(candidate);

        bool& previousRejected = lastRejected[key];
        if (z > AnomalyDetectorModel::Z_THRESHOLD && !previousRejected) {
            previousRejected = true;
            ++anomalyCount;

            std::ostringstream log;
            log << ts << ",ANOMALY_" << key
                << "," << std::fixed << std::setprecision(2) << z << ",z-score";
            fileWrite("predictions", log.str());

            std::ostringstream msg;
            msg << sensor.getType() << " mesure aberrante rejetee ("
                << std::fixed << std::setprecision(1) << value << " "
                << sensor.getUnit() << " / z = " << std::setprecision(2) << z << ")";
            notify("ANOMALY", msg.str());
            return;   // on ne la garde pas pour les calculs
        }
        previousRejected = false;
    }

    // 3. On l'ajoute a l'analyse
    analyzer.push(key, value);

    // 4. Alerte CO2, seulement au moment ou on passe le seuil
    if (key == "co2") {
        if (!co2Critical && value > CO2_CRITICAL) {
            co2Critical = true;
            notify("CO2", "seuil critique depasse : "
                + std::to_string(static_cast<int>(value)) + " ppm");
        }
        else if (co2Critical && value < CO2_CRITICAL) {
            co2Critical = false;
            notify("CO2", "retour sous le seuil critique : "
                + std::to_string(static_cast<int>(value)) + " ppm");
        }
    }
}

double Server::measuresToMinutes(double measures) const
{
    auto it = samplePeriodMs.find("co2");
    if (it == samplePeriodMs.end()) {
        return -1.0;
    }
    const double ticks = measures * it->second / SimClock::TICK_MS;
    return ticks * SimClock::SECONDS_PER_TICK / 60.0;
}

void Server::process()
{
    const std::string ts = timestamp();

    // 1. AQI
    const double aqi = analyzer.computeAQI();

    // 2. Dans combien de temps on atteint 1000 ppm
    lastTTT = -1.0;
    if (predictor) {
        const double measures = predictor->predict(analyzer.getHistory("co2"));
        if (measures >= 0.0) {
            const double minutes = measuresToMinutes(measures);
            if (minutes <= TTT_HORIZON_MIN) {
                lastTTT = minutes;
            }
        }
    }

    // 3. predictions.csv
    {
        std::ostringstream oss;
        oss << ts << ",AQI"
            << "," << std::fixed << std::setprecision(2) << aqi
            << ",/100";
        fileWrite("predictions", oss.str());
    }
    if (lastTTT >= 0.0) {
        std::ostringstream oss;
        oss << ts << ",TTT"
            << "," << std::fixed << std::setprecision(2) << lastTTT
            << ",min";
        fileWrite("predictions", oss.str());
    }

    // 4. On allume / eteint les actionneurs.
    //    Seuils ON et OFF differents, sinon ca clignote.
    const double co2Avg = analyzer.getAverage("co2");

    for (auto& act : actuators) {
        const std::string name = act->getName();
        const bool active = act->isActive();
        bool shouldBeActive = active;

        if (name == "VentilationSystem") {
            if (!active) {
                shouldBeActive = (lastTTT >= 0.0 && lastTTT < VENT_ON_TTT_MIN)
                              || aqi < VENT_ON_AQI
                              || co2Avg > CO2_CRITICAL;
            }
            else {
                shouldBeActive = !(co2Avg < VENT_OFF_CO2 && aqi >= VENT_OFF_AQI);
            }
        }
        else if (name == "AirPurifier") {
            shouldBeActive = active ? (aqi < PURIFIER_OFF_AQI)
                                    : (aqi < PURIFIER_ON_AQI);
        }

        if (shouldBeActive && !active) {
            act->activate();
            logActuatorState(name, true);
        }
        else if (!shouldBeActive && active) {
            act->deactivate();
            logActuatorState(name, false);
        }
    }
}

// ------------------------------------------------------------
// Observer
// ------------------------------------------------------------

void Server::attach(ServerObserver* observer)
{
    if (observer &&
        std::find(observers.begin(), observers.end(), observer) == observers.end()) {
        observers.push_back(observer);
    }
}

void Server::detach(ServerObserver* observer)
{
    observers.erase(std::remove(observers.begin(), observers.end(), observer),
                    observers.end());
}

void Server::notify(const std::string& category, const std::string& msg)
{
    const Notification n{ timestamp(), category, msg };

    remember("[" + category + "] " + msg);
    fileWrite("alerts", n.timestamp + "," + category + "," + msg);

    for (ServerObserver* obs : observers) {
        obs->onNotify(n);
    }
}

void Server::logActuatorState(const std::string& name, bool active)
{
    fileWrite("actuators", timestamp() + "," + name + "," + (active ? "ON" : "OFF"));
    notify("ACTUATOR", name + (active ? " active" : " desactive"));
}

// ------------------------------------------------------------
// Strategy
// ------------------------------------------------------------

void Server::setPredictor(std::unique_ptr<PredictiveModel> p)
{
    predictor = std::move(p);
}

void Server::setAnomalyDetector(std::unique_ptr<PredictiveModel> d)
{
    anomalyDetector = std::move(d);
}

void Server::addActuator(std::unique_ptr<Actuator> a)
{
    if (a) {
        actuators.push_back(std::move(a));
    }
}

// ------------------------------------------------------------
// Accesseurs
// ------------------------------------------------------------

std::size_t Server::getLogCount() const
{
    return logFiles.size();
}

std::size_t Server::getRecentMessageCount() const
{
    return recentMessages.size();
}

const DataAnalyzer& Server::getAnalyzer() const
{
    return analyzer;
}

std::size_t Server::getActuatorCount() const
{
    return actuators.size();
}

std::size_t Server::getAnomalyCount() const
{
    return anomalyCount;
}

double Server::getLastTTT() const
{
    return lastTTT;
}

bool Server::isActuatorActive(const std::string& name) const
{
    for (const auto& a : actuators) {
        if (a->getName() == name) {
            return a->isActive();
        }
    }
    return false;
}

// ------------------------------------------------------------
// Operateur de sortie
// ------------------------------------------------------------

std::ostream& operator<<(std::ostream& os, const Server& srv)
{
    os << "Server ["
        << "logs=" << srv.logFiles.size()
        << ", recentMessages=" << srv.recentMessages.size()
        << ", actuators=" << srv.actuators.size()
        << ", observers=" << srv.observers.size()
        << ", predictor=" << (srv.predictor ? srv.predictor->getName() : "none")
        << ", anomalyDetector=" << (srv.anomalyDetector ? srv.anomalyDetector->getName() : "none")
        << "]";
    return os;
}
