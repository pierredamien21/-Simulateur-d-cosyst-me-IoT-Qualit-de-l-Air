#include "core/Server.h"
#include "core/Sensor.h"
#include "core/Scheduler.h"
#include "core/SensorFactory.h"
#include "core/SimClock.h"

#include "core/VentilationSystem.h"
#include "core/AirPurifier.h"

#include "core/LinearRegressionModel.h"
#include "core/AnomalyDetectorModel.h"
#include "core/ConsoleAlertObserver.h"

#include <iomanip>
#include <iostream>
#include <memory>

// ------------------------------------------------------------
// Version console du simulateur.
// On cree le serveur, les capteurs et les actionneurs,
// puis on simule une journee complete en affichant
// un resume toutes les heures.
// ------------------------------------------------------------
int main()
{
    std::cout << "==============================================\n";
    std::cout << " Simulateur IoT - Qualite de l'Air\n";
    std::cout << " Version console\n";
    std::cout << "==============================================\n\n";

    // --------------------------------------------------------
    // 1. Serveur
    // --------------------------------------------------------
    Server server;
    server.openLogs();
    std::cout << "[1] Server initialise : "
        << server.getLogCount() << " fichiers de logs\n";

    // --------------------------------------------------------
    // 2. Algos, actionneurs et affichage des alertes
    // --------------------------------------------------------
    ConsoleAlertObserver alerts(std::cout);

    server.setPredictor(std::make_unique<LinearRegressionModel>());
    server.setAnomalyDetector(std::make_unique<AnomalyDetectorModel>());
    server.addActuator(std::make_unique<VentilationSystem>());
    server.addActuator(std::make_unique<AirPurifier>());
    server.attach(&alerts);
    std::cout << "[2] IA + actionneurs + observateur injectes\n";
    std::cout << "    " << server << "\n";

    // --------------------------------------------------------
    // 3. Scheduler
    // --------------------------------------------------------
    Scheduler& scheduler = Scheduler::getInstance();

    // --------------------------------------------------------
    // 4. Capteurs (avec la factory)
    // --------------------------------------------------------
    for (const char* type : { "Temperature", "Humidity", "CO2", "Sound", "Light" }) {
        scheduler.addSensor(SensorFactory::create(type, &server));
    }
    std::cout << "[3] " << scheduler.getSensorCount()
        << " capteurs crees via SensorFactory\n\n";

    // --------------------------------------------------------
    // 5. Une journee, analyse toutes les 15 min
    // --------------------------------------------------------
    constexpr int TOTAL_TICKS   = SimClock::TICKS_PER_DAY;     // 24 h
    constexpr int ANALYZE_EVERY = SimClock::TICKS_PER_HOUR / 4; // 15 min
    constexpr int PRINT_EVERY   = SimClock::TICKS_PER_HOUR;     // 1 h

    std::cout << "[4] Simulation de " << TOTAL_TICKS << " ticks (24 h simulees, "
        << "1 tick = " << SimClock::SECONDS_PER_TICK << " s)\n";
    std::cout << "    Analyse IA toutes les 15 min simulees\n\n";

    std::cout << "    Heure | Periode |   AQI  |  CO2 (ppm) |  TTT (min) | Vent. | Purif.\n";
    std::cout << "    ------+---------+--------+------------+------------+-------+-------\n";

    std::cout << std::fixed;
    for (int tick = 0; tick < TOTAL_TICKS; tick += ANALYZE_EVERY) {
        scheduler.simulation(ANALYZE_EVERY);
        server.process();

        if (scheduler.getCurrentTick() % PRINT_EVERY != 0) {
            continue;
        }

        const DataAnalyzer& an = server.getAnalyzer();
        const double ttt = server.getLastTTT();

        std::cout << "    "
            << SimClock::toHourMinute(scheduler.getSimulatedTime())
            << " | " << (scheduler.isDay() ? " jour  " : " nuit  ")
            << " | " << std::setw(6) << std::setprecision(1) << an.computeAQI()
            << " | " << std::setw(10) << std::setprecision(1) << an.getAverage("co2")
            << " | ";
        if (ttt >= 0.0) {
            std::cout << std::setw(10) << std::setprecision(1) << ttt;
        }
        else {
            std::cout << std::setw(10) << "stable";
        }
        std::cout << " | " << (server.isActuatorActive("VentilationSystem") ? "  ON " : "  OFF")
            << " | " << (server.isActuatorActive("AirPurifier") ? "  ON " : "  OFF")
            << "\n";
    }

    // --------------------------------------------------------
    // 6. Resume
    // --------------------------------------------------------
    const DataAnalyzer& an = server.getAnalyzer();
    std::cout << std::setprecision(2);
    std::cout << "\n[5] Recapitulatif final :\n";
    std::cout << "    AQI             : " << an.computeAQI() << " / 100\n";
    std::cout << "    CO2 moyen       : " << an.getAverage("co2") << " ppm\n";
    std::cout << "    Temperature moy : " << an.getAverage("temperature") << " C\n";
    std::cout << "    Humidite moy    : " << an.getAverage("humidity") << " %\n";
    std::cout << "    Alertes recues  : " << alerts.getAlertCount() << "\n";
    std::cout << "    Anomalies       : " << server.getAnomalyCount() << " mesures rejetees\n";

    // --------------------------------------------------------
    // 7. Fin
    // --------------------------------------------------------
    server.detach(&alerts);
    server.closeLogs();

    std::cout << "\n[6] Simulation terminee\n";
    std::cout << "    Logs dans : " << Server::logDir() << "\n";
    std::cout << "    - temperature.csv, humidity.csv, co2.csv, sound.csv, light.csv\n";
    std::cout << "    - predictions.csv (AQI, TTT, anomalies)\n";
    std::cout << "    - actuators.csv (ON/OFF)\n";
    std::cout << "    - alerts.csv (notifications Observer)\n";

    return 0;
}
