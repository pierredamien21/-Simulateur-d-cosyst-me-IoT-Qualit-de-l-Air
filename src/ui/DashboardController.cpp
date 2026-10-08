#include "ui/DashboardController.h"
#include "ui/DashboardView.h"

#include "core/Server.h"
#include "core/Scheduler.h"
#include "core/RoomState.h"
#include "core/DataAnalyzer.h"
#include "core/SimClock.h"

#include <QPushButton>
#include <QSlider>
#include <QString>
#include <QTimer>

DashboardController::DashboardController(Server* srv,
    DashboardView* v,
    QObject* parent)
    : QObject(parent)
    , server(srv)
    , view(v)
    , timer(new QTimer(this))
{
    connect(view->playButton(), &QPushButton::clicked,
        this, &DashboardController::onPlayClicked);
    connect(view->pauseButton(), &QPushButton::clicked,
        this, &DashboardController::onPauseClicked);
    connect(view->resetButton(), &QPushButton::clicked,
        this, &DashboardController::onResetClicked);

    connect(view->speedSlider(), &QSlider::valueChanged,
        this, &DashboardController::onSpeedChanged);

    connect(timer, &QTimer::timeout,
        this, &DashboardController::onSimulationStep);

    onSpeedChanged(view->speedSlider()->value());
    view->setRunning(false);

    server->attach(this);
}

DashboardController::~DashboardController()
{
    server->detach(this);
}

// ------------------------------------------------------------
// Alertes recues du serveur
// ------------------------------------------------------------

void DashboardController::onNotify(const Notification& n)
{
    view->addAlert(QString::fromStdString(n.timestamp),
        QString::fromStdString(n.category),
        QString::fromStdString(n.message));
}

// ------------------------------------------------------------
// A chaque image
// ------------------------------------------------------------

void DashboardController::onSimulationStep()
{
    // 1. On avance la simulation
    Scheduler& scheduler = Scheduler::getInstance();
    scheduler.simulation(TICKS_PER_FRAME);

    // 2. Le serveur analyse
    server->process();

    // 3. On recupere les valeurs
    const DataAnalyzer& analyzer = server->getAnalyzer();

    const double hours = static_cast<double>(scheduler.getCurrentTick())
                       / SimClock::TICKS_PER_HOUR;

    // 4. Heure + indicateurs
    view->updateClock(
        QString::fromStdString(SimClock::toHourMinute(scheduler.getSimulatedTime())),
        scheduler.isDay());
    view->updateKPIs(analyzer.computeAQI(), server->getLastTTT());

    // 5. Cartes et courbes. Si un capteur n'a encore rien mesure
    //    on attend (sinon la courbe part de 0).
    for (const char* key : { "co2", "temperature", "humidity", "sound" }) {
        if (analyzer.getSampleCount(key) == 0) {
            continue;
        }
        view->updateMeasure(key, analyzer.getLast(key), analyzer.getAverage(key));
        view->updateChart(key, hours, analyzer.getLast(key));
    }

    // 6. Actionneurs (+ bandes bleues sur le graphique du CO2)
    for (const char* name : { "VentilationSystem", "AirPurifier" }) {
        view->updateActuatorState(name, server->isActuatorActive(name), hours);
    }
}

// ------------------------------------------------------------
// Boutons
// ------------------------------------------------------------

void DashboardController::onPlayClicked()
{
    if (!timer->isActive()) {
        timer->start();
    }
    view->setRunning(true);
}

void DashboardController::onPauseClicked()
{
    if (timer->isActive()) {
        timer->stop();
    }
    view->setRunning(false);
}

void DashboardController::onResetClicked()
{
    timer->stop();

    // On remet tout le modele a zero, pas juste l'affichage
    server->reset();                       // analyse, actionneurs, logs
    RoomState::getInstance().reset();      // la piece
    Scheduler::getInstance().reset();      // l'heure et les capteurs

    view->resetDisplay();
    view->setRunning(false);
}

void DashboardController::onSpeedChanged(int index)
{
    // On change juste la frequence du timer, pas le nombre de ticks
    // par image : comme ca la simulation est la meme a toutes les vitesses.
    static const double factors[] = { 0.5, 1.0, 2.0, 4.0 };
    const double factor = factors[qBound(0, index, 3)];

    timer->setInterval(static_cast<int>(FRAME_INTERVAL_MS / factor));
    view->setSpeedLabel("x" + QString::number(factor));
}
