#include "core/Config.h"

#include "core/Server.h"
#include "core/Sensor.h"
#include "core/Scheduler.h"
#include "core/SensorFactory.h"

#include "core/VentilationSystem.h"
#include "core/AirPurifier.h"

#include "core/LinearRegressionModel.h"

#include <iostream>
#include <memory>

// ------------------------------------------------------------
// AP4A - Simulateur d'Ecosysteme IoT & Qualite de l'Air
//
// Point d'entree de la version console (Phase 3 finale).
//
// Ce programme :
//   1. Cree un Server (centre d'orchestration)
//   2. Lui injecte un modele predictif (Strategy) et des actionneurs
//   3. Cree dynamiquement 5 capteurs via SensorFactory
//   4. Anime la simulation via Scheduler (Singleton)
//   5. Analyse periodiquement les mesures (AQI, prediction)
//   6. Declenche les actionneurs selon les decisions
// ------------------------------------------------------------
int main()
{
    std::cout << "==============================================\n";
    std::cout << " AP4A - Simulateur IoT Qualite de l'Air\n";
    std::cout << " Version console - Phase 3 finale\n";
    std::cout << "==============================================\n\n";

    // --------------------------------------------------------
    // 1. Serveur
    // --------------------------------------------------------
    Server server;
    server.openLogs();
    std::cout << "[1] Server initialise : "
        << server.getLogCount() << " fichiers de logs\n";

    // --------------------------------------------------------
    // 2. Injection IA + actionneurs
    // --------------------------------------------------------
    server.setPredictor(std::make_unique<LinearRegressionModel>());
    server.addActuator(std::make_unique<VentilationSystem>());
    server.addActuator(std::make_unique<AirPurifier>());
    std::cout << "[2] IA + actionneurs injectes\n";
    std::cout << "    " << server << "\n";

    // --------------------------------------------------------
    // 3. Scheduler (Singleton)
    // --------------------------------------------------------
    Scheduler& scheduler = Scheduler::getInstance();

    // --------------------------------------------------------
    // 4. Capteurs via Factory
    // --------------------------------------------------------
    scheduler.addSensor(SensorFactory::create("Temperature", &server));
    scheduler.addSensor(SensorFactory::create("Humidity", &server));
    scheduler.addSensor(SensorFactory::create("CO2", &server));
    scheduler.addSensor(SensorFactory::create("Sound", &server));
    scheduler.addSensor(SensorFactory::create("Light", &server));
    std::cout << "[3] " << scheduler.getSensorCount()
        << " capteurs crees via SensorFactory\n\n";

    // --------------------------------------------------------
    // 5. Simulation + analyse periodique
    // --------------------------------------------------------
    constexpr int TOTAL_TICKS = 500;
    constexpr int ANALYZE_EVERY = 50;
    constexpr int BATCHES = TOTAL_TICKS / ANALYZE_EVERY;

    std::cout << "[4] Simulation de " << TOTAL_TICKS
        << " ticks (" << TOTAL_TICKS / 10 << " s simulees)\n";
    std::cout << "    Analyse IA tous les " << ANALYZE_EVERY << " ticks\n\n";

    std::cout << "    Tick  |   AQI   | Statut actionneurs\n";
    std::cout << "    ------+---------+-------------------\n";

    for (int batch = 0; batch < BATCHES; ++batch) {
        scheduler.simulation(ANALYZE_EVERY);
        server.process();

        const double aqi = server.getAnalyzer().computeAQI();

        // Compacter l'affichage des actionneurs
        std::string actLine;
        const auto& acts = { "VentilationSystem", "AirPurifier" };
        (void)acts;   // (utilisation optionnelle)

        std::cout << "    " << scheduler.getCurrentTick()
            << "\t| " << aqi
            << "\t| voir logs/actuators.csv\n";
    }

    // --------------------------------------------------------
    // 6. Recapitulatif final
    // --------------------------------------------------------
    const double finalAqi = server.getAnalyzer().computeAQI();
    std::cout << "\n[5] Recapitulatif final :\n";
    std::cout << "    AQI moyen      : " << finalAqi << " / 100\n";
    std::cout << "    CO2 moyen      : "
        << server.getAnalyzer().getAverage("co2") << " ppm\n";
    std::cout << "    Temperature moy: "
        << server.getAnalyzer().getAverage("temperature") << " C\n";
    std::cout << "    Humidite moy   : "
        << server.getAnalyzer().getAverage("humidity") << " %\n";

    // --------------------------------------------------------
    // 7. Fermeture
    // --------------------------------------------------------
    server.closeLogs();

    std::cout << "\n[6] Simulation terminee\n";
    std::cout << "    Logs dans : " << Server::logDir() << "\n";
    std::cout << "    - temperature.csv, humidity.csv, co2.csv\n";
    std::cout << "    - sound.csv, light.csv\n";
    std::cout << "    - predictions.csv (AQI + TTT)\n";
    std::cout << "    - actuators.csv (ON/OFF)\n";

    return 0;
}