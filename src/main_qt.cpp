#include "ui/DashboardView.h"
#include "ui/DashboardController.h"

#include "core/Server.h"
#include "core/Sensor.h"
#include "core/Scheduler.h"
#include "core/SensorFactory.h"

#include "core/VentilationSystem.h"
#include "core/AirPurifier.h"
#include "core/LinearRegressionModel.h"
#include "core/AnomalyDetectorModel.h"

#include <QApplication>
#include <QTimer>

#include <memory>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    // Fusion = meme rendu sous Windows et Linux,
    // les couleurs viennent du theme (QSS)
    QApplication::setStyle("Fusion");

    // --------------------------------------------------------
    // 1. Modele
    // --------------------------------------------------------
    Server server;
    server.openLogs();
    server.setPredictor(std::make_unique<LinearRegressionModel>());
    server.setAnomalyDetector(std::make_unique<AnomalyDetectorModel>());
    server.addActuator(std::make_unique<VentilationSystem>());
    server.addActuator(std::make_unique<AirPurifier>());

    Scheduler& scheduler = Scheduler::getInstance();
    for (const char* type : { "Temperature", "Humidity", "CO2", "Sound", "Light" }) {
        scheduler.addSensor(SensorFactory::create(type, &server));
    }

    // --------------------------------------------------------
    // 2. Vue
    // --------------------------------------------------------
    DashboardView view;
    view.show();

    // --------------------------------------------------------
    // 3. Controleur (il s'inscrit aux alertes du serveur)
    // --------------------------------------------------------
    DashboardController controller(&server, &view);

    // --------------------------------------------------------
    // 4. On lance direct
    // --------------------------------------------------------
    QTimer::singleShot(0, &controller, &DashboardController::onPlayClicked);

    return app.exec();
}