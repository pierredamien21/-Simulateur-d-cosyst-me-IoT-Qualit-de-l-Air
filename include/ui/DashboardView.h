#ifndef AIRQ_DASHBOARD_VIEW_H
#define AIRQ_DASHBOARD_VIEW_H

#include "ui/Theme.h"

#include <QMainWindow>
#include <QMap>
#include <QString>

class AqiGauge;
class ChartWidget;
class KpiCard;
class QBoxLayout;
class QLabel;
class QListWidget;
class QPushButton;
class QSlider;

// Fenetre principale. De haut en bas :
//  - la barre avec les boutons, la vitesse, le theme et l'heure
//  - les indicateurs (jauge AQI + 4 cartes)
//  - le graphique du CO2 et 3 petits graphiques
//  - a droite : les actionneurs et la liste des alertes
// La vue ne calcule rien, elle affiche ce que le controleur lui donne
// (sauf le changement de theme qu'elle gere elle-meme).
class DashboardView : public QMainWindow {
    Q_OBJECT

public:
    static constexpr int    MAX_ALERTS    = 200;
    static constexpr double CO2_THRESHOLD = 1000.0;

    explicit DashboardView(QWidget* parent = nullptr);
    ~DashboardView() override;

    // --------------------------------------------------------
    // Appele par le controleur
    // --------------------------------------------------------
    void updateClock(const QString& time, bool day);
    void updateKPIs(double aqi, double ttt);
    void updateMeasure(const QString& key, double last, double average);
    void updateChart(const QString& key, double x, double y);
    void updateActuatorState(const QString& name, bool active, double x);
    void addAlert(const QString& timestamp, const QString& category,
        const QString& message);
    void setRunning(bool running);
    void setSpeedLabel(const QString& text);

    void resetDisplay();

    // Le controleur branche ses slots sur ces widgets

    QPushButton* playButton()  const;
    QPushButton* pauseButton() const;
    QPushButton* resetButton() const;
    QSlider*     speedSlider() const;

private slots:
    void toggleTheme();

private:
    void buildUI();
    QWidget* buildTopBar();
    QWidget* buildKpiRow();
    QWidget* buildCharts();
    QWidget* buildSidePanel();

    void applyTheme(const Theme& theme);
    void colorAlert(int row);

    // Change une propriete utilisee par le QSS et redessine le widget
    static void setStyleState(QWidget* w, const char* property, const QString& value);

    Theme theme;

    // Barre du haut
    QPushButton* btnPlay;
    QPushButton* btnPause;
    QPushButton* btnReset;
    QPushButton* btnTheme;
    QSlider*     slider;
    QLabel*      speedValue;
    QLabel*      statusLabel;
    QLabel*      clockLabel;
    QLabel*      periodBadge;

    // Indicateurs
    AqiGauge* gauge;
    QLabel*   aqiLevel;
    KpiCard*  co2Card;
    KpiCard*  tttCard;
    KpiCard*  tempCard;
    KpiCard*  humCard;

    // Actionneurs
    QLabel* ventilationBadge;
    QLabel* purifierBadge;

    QListWidget* alertList;

    // "co2", "temperature"... -> graphique
    QMap<QString, ChartWidget*> charts;
};

#endif // AIRQ_DASHBOARD_VIEW_H
