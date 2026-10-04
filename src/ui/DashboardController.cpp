#include "ui/DashboardController.h"
#include "ui/DashboardView.h"

#include "core/Server.h"
#include "core/Scheduler.h"
#include "core/DataAnalyzer.h"

#include <QPushButton>
#include <QTimer>

DashboardController::DashboardController(Server* srv,
    DashboardView* v,
    QObject* parent)
    : QObject(parent)
    , server(srv)
    , view(v)
    , timer(new QTimer(this))
    , tickCount(0.0)
{
    connect(view->playButton(), &QPushButton::clicked,
        this, &DashboardController::onPlayClicked);
    connect(view->pauseButton(), &QPushButton::clicked,
        this, &DashboardController::onPauseClicked);
    connect(view->resetButton(), &QPushButton::clicked,
        this, &DashboardController::onResetClicked);

    connect(timer, &QTimer::timeout,
        this, &DashboardController::onSimulationStep);

    timer->setInterval(200);
}

DashboardController::~DashboardController() = default;

// ------------------------------------------------------------
// Boucle d'animation
// ------------------------------------------------------------

void DashboardController::onSimulationStep()
{
    // 1. Avancer la simulation (50 ticks par frame)
    Scheduler& scheduler = Scheduler::getInstance();
    scheduler.simulation(50);

    // 2. Analyser les donnees
    server->process();

    // 3. Recuperer les valeurs
    const DataAnalyzer& analyzer = server->getAnalyzer();

    const double aqi = analyzer.computeAQI();
    const double co2Avg = analyzer.getAverage("co2");
    const double temp = analyzer.getAverage("temperature");
    const double ttt = server->getLastTTT();

    // 4. KPI (moyennes + AQI)
    view->updateKPIs(aqi, co2Avg, ttt, temp);

    // 5. Mesures en direct (valeurs instantanees des 5 capteurs)
    view->updateLiveMeasures(
        analyzer.getLast("co2"),
        analyzer.getLast("temperature"),
        analyzer.getLast("humidity"),
        analyzer.getLast("sound"),
        analyzer.getLast("light")
    );

    // 6. Courbes (5 graphiques)
    view->updateChart("co2", tickCount, analyzer.getLast("co2"));
    view->updateChart("temperature", tickCount, analyzer.getLast("temperature"));
    view->updateChart("humidity", tickCount, analyzer.getLast("humidity"));
    view->updateChart("sound", tickCount, analyzer.getLast("sound"));
    view->updateChart("light", tickCount, analyzer.getLast("light"));

    // 7. Actionneurs
    view->updateActuatorState("VentilationSystem",
        server->isActuatorActive("VentilationSystem"));
    view->updateActuatorState("AirPurifier",
        server->isActuatorActive("AirPurifier"));

    ++tickCount;
}

// ------------------------------------------------------------
// Boutons
// ------------------------------------------------------------

void DashboardController::onPlayClicked()
{
    if (!timer->isActive()) {
        timer->start();
    }
}

void DashboardController::onPauseClicked()
{
    if (timer->isActive()) {
        timer->stop();
    }
}

void DashboardController::onResetClicked()
{
    timer->stop();
    tickCount = 0.0;
    view->clearChart();
}