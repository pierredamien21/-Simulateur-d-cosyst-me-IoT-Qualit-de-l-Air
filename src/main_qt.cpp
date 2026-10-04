#include "ui/DashboardView.h"
#include "ui/DashboardController.h"

#include "core/Server.h"
#include "core/Sensor.h"          // <-- AJOUT
#include "core/Scheduler.h"
#include "core/SensorFactory.h"

#include "core/VentilationSystem.h"
#include "core/AirPurifier.h"
#include "core/LinearRegressionModel.h"

#include <QApplication>
#include <QTimer>

#include <memory>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    // --------------------------------------------------------
    // 1. Modele
    // --------------------------------------------------------
    Server server;
    server.openLogs();
    server.setPredictor(std::make_unique<LinearRegressionModel>());
    server.addActuator(std::make_unique<VentilationSystem>());
    server.addActuator(std::make_unique<AirPurifier>());

    Scheduler& scheduler = Scheduler::getInstance();
    scheduler.addSensor(SensorFactory::create("Temperature", &server));
    scheduler.addSensor(SensorFactory::create("Humidity", &server));
    scheduler.addSensor(SensorFactory::create("CO2", &server));
    scheduler.addSensor(SensorFactory::create("Sound", &server));
    scheduler.addSensor(SensorFactory::create("Light", &server));

    // --------------------------------------------------------
    // 2. Vue
    // --------------------------------------------------------
    DashboardView view;
    view.show();

    // --------------------------------------------------------
    // 3. Controleur
    // --------------------------------------------------------
    DashboardController controller(&server, &view);

    // --------------------------------------------------------
    // 4. Lancement automatique
    // --------------------------------------------------------
    QTimer::singleShot(0, &controller, &DashboardController::onPlayClicked);

    return app.exec();
}