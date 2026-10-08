#ifndef AIRQ_DASHBOARD_CONTROLLER_H
#define AIRQ_DASHBOARD_CONTROLLER_H

#include "core/ServerObserver.h"

#include <QObject>

class DashboardView;
class Server;
class QTimer;

// Le C du MVC : un QTimer fait avancer la simulation, puis le
// controleur lit les valeurs du serveur et les envoie a la vue.
// Il recoit aussi les alertes du serveur (Observer) et gere
// les boutons. Il ne possede ni le serveur ni la vue.
class DashboardController : public QObject, public ServerObserver {
    Q_OBJECT

public:
    static constexpr int FRAME_INTERVAL_MS = 100;   // 10 images/s en vitesse x1
    static constexpr int TICKS_PER_FRAME   = 10;    // = 6 min simulees par image

    DashboardController(Server* server,
        DashboardView* view,
        QObject* parent = nullptr);

    ~DashboardController() override;

    void onNotify(const Notification& notification) override;

public slots:
    void onSimulationStep();   // a chaque coup de timer
    void onPlayClicked();
    void onPauseClicked();
    void onResetClicked();
    void onSpeedChanged(int index);   // 0 a 3 -> x0.5, x1, x2, x4

private:
    Server* server;
    DashboardView* view;

    QTimer* timer;
};

#endif // AIRQ_DASHBOARD_CONTROLLER_H
