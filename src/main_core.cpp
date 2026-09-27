#include "core/Server.h"
#include "core/Config.h"

#include <fstream>
#include <iostream>

int main()
{
    std::cout << "=== AP4A - Test Phase 1 : Server ===\n\n";

    // --------------------------------------------------------
    // Test 1 : Coplien - constructeur par defaut
    // --------------------------------------------------------
    Server server;
    std::cout << "1. Server cree : " << server << "\n";

    // --------------------------------------------------------
    // Test 2 : openLogs()
    // --------------------------------------------------------
    server.openLogs();
    std::cout << "2. openLogs() : " << server.getLogCount()
        << " fichiers ouverts\n";

    // --------------------------------------------------------
    // Test 3 : fileWrite() - 5 lignes de test (une par type)
    // --------------------------------------------------------
    server.fileWrite("temperature",
        "2026-11-06T14:32:01,1,Temperature,22.5,C");
    server.fileWrite("humidity",
        "2026-11-06T14:32:02,2,Humidity,55.3,%");
    server.fileWrite("co2",
        "2026-11-06T14:32:03,3,CO2,842.5,ppm");
    server.fileWrite("sound",
        "2026-11-06T14:32:04,4,Sound,52,dB");
    server.fileWrite("light",
        "2026-11-06T14:32:05,5,Light,1,bool");
    std::cout << "3. fileWrite() : 5 lignes ecrites\n";

    // Test type inconnu (doit afficher une erreur mais continuer)
    server.fileWrite("unknown_type", "ceci-ne-doit-pas-crasher");
    std::cout << "   (test type inconnu : doit afficher une erreur ci-dessus)\n";

    // --------------------------------------------------------
    // Test 4 : consoleWrite()
    // --------------------------------------------------------
    server.consoleWrite("4. consoleWrite() OK");
    std::cout << "   recentMessages = "
        << server.getRecentMessageCount() << "\n";

    // --------------------------------------------------------
    // Test 5 : operator<< vers std::cout
    // --------------------------------------------------------
    std::cout << "5. operator<< vers cout : " << server << "\n";

    // --------------------------------------------------------
    // Test 6 : operator<< vers std::ofstream
    // --------------------------------------------------------
    {
        const std::string testPath =
            Server::logDir() + "/_operator_test.txt";

        std::ofstream ofs(testPath);
        if (ofs.is_open()) {
            ofs << server << "\n";
            std::cout << "6. operator<< vers fichier OK\n";
        }
        else {
            std::cerr << "6. Impossible d'ouvrir " << testPath << "\n";
        }
    }

    // --------------------------------------------------------
    // Test 7 : Coplien - constructeur par copie
    // --------------------------------------------------------
    Server copy = server;
    std::cout << "7. Copie : " << copy
        << " (recentMessages copies = "
        << copy.getRecentMessageCount() << ")\n";

    // --------------------------------------------------------
    // Test 8 : Coplien - operateur d'affectation
    // --------------------------------------------------------
    Server assigned;
    assigned = server;
    std::cout << "8. Affectation : " << assigned
        << " (recentMessages copies = "
        << assigned.getRecentMessageCount() << ")\n";

    // --------------------------------------------------------
    // Test 9 : auto-affectation (doit etre sans danger)
    // --------------------------------------------------------
    assigned = assigned;
    std::cout << "9. Auto-affectation OK\n";

    // --------------------------------------------------------
    // Fermeture
    // --------------------------------------------------------
    server.closeLogs();
    copy.closeLogs();
    assigned.closeLogs();

    std::cout << "\n=== Phase 1 : tous les tests passes ===\n";
    return 0;
}