#ifndef AP4A_DASHBOARD_VIEW_H
#define AP4A_DASHBOARD_VIEW_H

#include <QMainWindow>
#include <QMap>
#include <QString>

class ChartWidget;
class QLabel;
class QPushButton;
class QTimer;

/**
 * @brief Fenetre principale de l'interface graphique.
 *
 * Affiche :
 *  - 4 KPI (AQI, CO2 moyen, TTT, Temperature moyenne)
 *  - 5 mesures en direct (CO2, Temp, Hum, Son, Lumiere)
 *  - une grille 2x3 de graphiques (CO2, Temp, Hum, Son, Lumiere)
 *  - l'etat des actionneurs (Ventilation, Purificateur)
 *  - 3 boutons (Play / Pause / Reset)
 */
class DashboardView : public QMainWindow {
    Q_OBJECT

public:
    explicit DashboardView(QWidget* parent = nullptr);
    ~DashboardView() override;

    // --------------------------------------------------------
    // Mises a jour (appelees par le controller)
    // --------------------------------------------------------
    void updateKPIs(double aqi, double co2, double ttt, double temperature);
    void updateLiveMeasures(double co2, double temp, double hum,
        double sound, double light);        // <-- NOUVEAU
    void updateChart(const QString& name, double x, double y);
    void updateActuatorState(const QString& name, bool active);

    // --------------------------------------------------------
    // Accesseurs pour le controller
    // --------------------------------------------------------
    QPushButton* playButton()  const;
    QPushButton* pauseButton() const;
    QPushButton* resetButton() const;

    void clearChart();

private:
    void buildUI();
    void buildKPIPanel(class QVBoxLayout* parentLayout);
    void buildLiveMeasuresPanel(class QVBoxLayout* parentLayout);  // <-- NOUVEAU
    void buildActuatorPanel(class QVBoxLayout* parentLayout);
    void buildChartsGrid(class QGridLayout* gridLayout);

    // KPI (valeurs moyennes + AQI)
    QLabel* aqiValue;
    QLabel* co2Value;
    QLabel* tttValue;
    QLabel* tempValue;

    // Mesures en direct (5 capteurs, valeurs instantanees)
    QLabel* liveCo2;      // <-- NOUVEAU
    QLabel* liveTemp;     // <-- NOUVEAU
    QLabel* liveHum;      // <-- NOUVEAU
    QLabel* liveSound;    // <-- NOUVEAU
    QLabel* liveLight;    // <-- NOUVEAU

    // Actionneurs
    QLabel* ventilationState;
    QLabel* purifierState;

    // Grille de graphiques : nom -> widget
    QMap<QString, ChartWidget*> charts;

    // Boutons
    QPushButton* btnPlay;
    QPushButton* btnPause;
    QPushButton* btnReset;
};

#endif // AP4A_DASHBOARD_VIEW_H