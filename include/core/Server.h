// pour eviter les definitions multiples
#ifndef AP4A_SERVER_H
#define AP4A_SERVER_H

// les bibliotheques necessaires
#include <cstddef>    // pour std::size_t
#include <fstream>    // pour l'edition de fichiers
#include <map>        // pour la gestion de dictionnaires (cle/valeur)
#include <iostream>   // pour l'affichage dans la console
#include <string>     // pour la gestion de chaine de caracteres
#include <vector>     // pour la gestion de tableaux dynamiques

/**
 * @brief Serveur central du simulateur IoT.
 *
 * Le serveur recoit les mesures des capteurs, les formate, les
 * affiche en console et les stocke dans des fichiers de logs CSV
 * (un fichier par type de capteur).
 *
 * Forme canonique de Coplien : constructeur par defaut,
 * constructeur par copie, operateur d'affectation, destructeur.
 *
 * Note : le Server n'est PAS un Singleton, afin de respecter
 * pleinement la forme de Coplien. Une unique instance sera
 * creee dans main() et partagee par pointeur.
 */


class Server {
public:

    // Forme canonique de Coplien
    Server();
    Server(const Server& other);
    Server& operator=(const Server& other);
    ~Server();


    // Services
    void consoleWrite(const std::string& message);
    void fileWrite(const std::string& type, const std::string& message);
    void openLogs();
    void closeLogs();

   
    // Accesseurs (lecture seule)
    std::size_t getLogCount() const;
    std::size_t getRecentMessageCount() const;


    // Chemin du dossier de logs (racine du projet)
    static std::string logDir();


    // Operateur de sortie (friend)
    friend std::ostream& operator<<(std::ostream& os, const Server& server);


private:
    std::map<std::string, std::ofstream> logFiles;
    std::vector<std::string>             recentMessages;
};

#endif // AP4A_SERVER_H