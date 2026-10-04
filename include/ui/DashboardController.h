#ifndef AP4A_DASHBOARD_CONTROLLER_H
#define AP4A_DASHBOARD_CONTROLLER_H

#include <QObject>

class DashboardView;
class Server;
class QTimer;

/**
 * @brief Controleur MVC reliant le Modele (Server) a la Vue (Qt).
 *
 * Responsabilites :
 *  - Possede un QTimer qui declenche un pas de simulation
 *    periodiquement (appel a Scheduler::simulation + Server::process)
 *  - Lit les valeurs du Server (AQI, moyennes) et les pousse vers
 *    la Vue via des signaux
 *  - Relie les boutons Play / Pause / Reset a des slots
 *
 * Le controller ne possede NI le Server NI la Scheduler :
 * il recoit des pointeurs non-proprietaires (fournis par main).
 */
class DashboardController : public QObject {
    Q_OBJECT

public:
    DashboardController(Server* server,
        DashboardView* view,
        QObject* parent = nullptr);

    ~DashboardController() override;

public slots:
    void onSimulationStep();   // appele a chaque tick du QTimer
    void onPlayClicked();
    void onPauseClicked();
    void onResetClicked();

private:
    // Pointeurs NON propriétaires
    Server* server;
    DashboardView* view;

    // Timer d'animation
    QTimer* timer;

    // Compteur de ticks (pour l'axe X de la courbe)
    double tickCount;
};

#endif // AP4A_DASHBOARD_CONTROLLER_H