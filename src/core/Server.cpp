#include "core/Server.h"
#include "core/Config.h"

#include "core/Sensor.h"
#include "core/Actuator.h"
#include "core/PredictiveModel.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <utility>

// ------------------------------------------------------------
// Helper : chemin du dossier de logs
// ------------------------------------------------------------
std::string Server::logDir()
{
    return std::string(AP4A_PROJECT_ROOT) + "/logs";
}

// ------------------------------------------------------------
// Helper : normalisation de cle ("CO2" -> "co2")
// ------------------------------------------------------------
std::string Server::normalize(const std::string& type)
{
    std::string key = type;
    std::transform(key.begin(), key.end(), key.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return key;
}

// ------------------------------------------------------------
// Forme canonique de Coplien
// ------------------------------------------------------------

Server::Server()
    : logFiles()
    , recentMessages()
    , analyzer()
    , predictor(nullptr)
    , actuators()
{
}

Server::Server(const Server& other)
    : logFiles()                            // ofstream non copiable
    , recentMessages(other.recentMessages)
    , analyzer(other.analyzer)              // DataAnalyzer copiable
    , predictor(nullptr)                    // unique_ptr non copiable
    , actuators()                           // unique_ptr non copiable
{
    // La copie herite des messages et de l'analyzer,
    // mais PAS des flux ni du predictor ni des actionneurs.
}

Server& Server::operator=(const Server& other)
{
    if (this == &other) {
        return *this;
    }

    closeLogs();

    recentMessages = other.recentMessages;
    analyzer = other.analyzer;
    // predictor et actuators : NON copies (unique_ptr).
    // Repartent a vide.

    return *this;
}

Server::~Server() = default;

// ------------------------------------------------------------
// Services de base
// ------------------------------------------------------------

void Server::consoleWrite(const std::string& message)
{
    std::cout << message << "\n";
    recentMessages.push_back(message);

    constexpr std::size_t MAX_MESSAGES = 1000;
    if (recentMessages.size() > MAX_MESSAGES) {
        recentMessages.erase(
            recentMessages.begin(),
            recentMessages.begin() + (recentMessages.size() - MAX_MESSAGES)
        );
    }
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

    // Logs par type de capteur + fichiers d'analyse
    const std::vector<std::string> types = {
        "temperature", "humidity", "co2", "sound", "light",
        "predictions", "actuators"
    };

    const std::map<std::string, std::string> headers = {
        { "temperature", "timestamp,id,type,value,unit\n" },
        { "humidity",    "timestamp,id,type,value,unit\n" },
        { "co2",         "timestamp,id,type,value,unit\n" },
        { "sound",       "timestamp,id,type,value,unit\n" },
        { "light",       "timestamp,id,type,value,unit\n" },
        { "predictions", "timestamp,kind,value,unit\n"   },
        { "actuators",   "timestamp,actuator,state\n"    },
    };

    for (const auto& type : types) {
        const std::string path = dir + "/" + type + ".csv";
        logFiles[type].open(path, std::ios::out | std::ios::trunc);
        if (logFiles[type].is_open()) {
            logFiles[type] << headers.at(type);
            logFiles[type].flush();
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

// ------------------------------------------------------------
// Integration capteurs
// ------------------------------------------------------------

void Server::receive(const Sensor& sensor)
{
    const std::string key = normalize(sensor.getType());
    const double      value = sensor.getValue();

    // 1. Alimenter l'analyseur (fenetre glissante)
    analyzer.push(key, value);

    // 2. Ecrire dans le CSV correspondant
    std::ostringstream oss;
    oss << "2026-11-06T00:00:00"
        << "," << sensor.getId()
        << "," << sensor.getType()
        << "," << std::fixed << std::setprecision(2) << value
        << "," << sensor.getUnit();
    fileWrite(key, oss.str());

    // 3. Verification critique (Observer)
    if (key == "co2" && value > 1000.0) {
        notify("CO2 critique : " + std::to_string(static_cast<int>(value)) + " ppm");
    }
}

void Server::process()
{
    // 1. Indice de qualite de l'air
    const double aqi = analyzer.computeAQI();

    // 2. Prediction Time-to-Threshold via la strategie courante
    double ttt = -1.0;
    if (predictor) {
        ttt = predictor->predict(analyzer.getHistory("co2"));
    }

    // 3. Log predictions.csv
    {
        std::ostringstream oss;
        oss << "2026-11-06T00:00:00"
            << ",AQI"
            << "," << std::fixed << std::setprecision(2) << aqi
            << ",/100";
        fileWrite("predictions", oss.str());
    }
    if (ttt >= 0.0) {
        std::ostringstream oss;
        oss << "2026-11-06T00:00:00"
            << ",TTT"
            << "," << std::fixed << std::setprecision(2) << ttt
            << ",measures";
        fileWrite("predictions", oss.str());
    }

    // 4. Decision sur les actionneurs
    const bool ventilationNeeded = (ttt >= 0.0 && ttt < 50.0) || (aqi < 40.0);
    const bool purifierNeeded = (aqi < 30.0);

    for (auto& act : actuators) {
        const std::string name = act->getName();
        bool shouldBeActive = false;

        if (name == "VentilationSystem") {
            shouldBeActive = ventilationNeeded;
        }
        else if (name == "AirPurifier") {
            shouldBeActive = purifierNeeded;
        }

        if (shouldBeActive && !act->isActive()) {
            act->activate();
            logActuatorState(name, true);
        }
        else if (!shouldBeActive && act->isActive()) {
            act->deactivate();
            logActuatorState(name, false);
        }
    }
}

void Server::notify(const std::string& msg)
{
    consoleWrite("[NOTIFY] " + msg);
}

void Server::logActuatorState(const std::string& name, bool active)
{
    std::ostringstream oss;
    oss << "2026-11-06T00:00:00"
        << "," << name
        << "," << (active ? "ON" : "OFF");
    fileWrite("actuators", oss.str());
}

// ------------------------------------------------------------
// Injection de strategie
// ------------------------------------------------------------

void Server::setPredictor(std::unique_ptr<PredictiveModel> p)
{
    predictor = std::move(p);
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

// ------------------------------------------------------------
// Accesseurs supplementaires (IHM Qt)
// ------------------------------------------------------------

double Server::getLastTTT() const
{
    if (!predictor) {
        return -1.0;
    }
    return predictor->predict(analyzer.getHistory("co2"));
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
        << ", hasPredictor=" << (srv.predictor ? "yes" : "no")
        << "]";
    return os;
}