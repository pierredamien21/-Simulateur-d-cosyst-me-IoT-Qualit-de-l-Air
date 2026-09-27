#include "core/Server.h"
#include "core/Config.h"

#include <filesystem>   // pour la gestion des fichiers et repertoires
#include <iostream>
#include <utility>


// Chemin du dossier de logs (racine du projet, injecte par CMake)
std::string Server::logDir()
{
    return std::string(AP4A_PROJECT_ROOT) + "/logs";
}


// Forme canonique de Coplien
Server::Server()
    : logFiles()
    , recentMessages()
{
}

Server::Server(const Server& other)
    : logFiles()                        // ofstream non copiable : on ne copie
    , recentMessages(other.recentMessages)   // pas les flux, seulement les messages
{
    // La copie herite des messages recents mais PAS des flux ouverts.
    // L'appelant peut appeler openLogs() sur la copie s'il en a besoin.
}

Server& Server::operator=(const Server& other)
{
    if (this == &other) {
        return *this;
    }

    // Fermer les flux courants avant de copier
    closeLogs();

    recentMessages = other.recentMessages;
    // logFiles : on ne copie pas les flux (ofstream non copiable).
    // Une reouverture via openLogs() est requise pour la copie.

    return *this;
}

Server::~Server()
{
    closeLogs();
}


// Services
void Server::consoleWrite(const std::string& message)
{
    std::cout << message << "\n";
    recentMessages.push_back(message);

    // Limiter la taille de l'historique en memoire (eviter croissance infinie)
    constexpr std::size_t MAX_MESSAGES = 1000;
    if (recentMessages.size() > MAX_MESSAGES) {
        recentMessages.erase(
            recentMessages.begin(),
            recentMessages.begin() +
            (recentMessages.size() - MAX_MESSAGES)
        );
    }
}


/*
 * fileWrite :
 *  - it->first  : cle  (type de capteur, ex: "temperature")
 *  - it->second : valeur (flux ofstream correspondant)
 */

void Server::fileWrite(const std::string& type, const std::string& message)
{
    auto it = logFiles.find(type);

    if (it != logFiles.end() && it->second.is_open()) {
        it->second << message << "\n";
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

    // Creer le dossier logs s'il n'existe pas
    std::error_code ec;
    fs::create_directories(dir, ec);
    if (ec) {
        std::cerr << "[Server] Impossible de creer " << dir
            << " : " << ec.message() << "\n";
        return;
    }

    // Types de capteurs et en-tete CSV
    const std::vector<std::string> types = {
        "temperature", "humidity", "co2", "sound", "light"
    };
    const std::string header = "timestamp,id,type,value,unit\n";

    for (const auto& type : types) {
        const std::string path = dir + "/" + type + ".csv";

        // Ouvrir en ecrasement (trunc) pour repartir d'un etat propre
        logFiles[type].open(path, std::ios::out | std::ios::trunc);

        if (logFiles[type].is_open()) {
            logFiles[type] << header;
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


// Accesseurs
std::size_t Server::getLogCount() const
{
    return logFiles.size();
}

std::size_t Server::getRecentMessageCount() const
{
    return recentMessages.size();
}


// Operateur de sortie (friend)
std::ostream& operator<<(std::ostream& os, const Server& srv)
{
    os << "Server ["
        << "logs=" << srv.logFiles.size()
        << ", recentMessages=" << srv.recentMessages.size()
        << "]";
    return os;
}