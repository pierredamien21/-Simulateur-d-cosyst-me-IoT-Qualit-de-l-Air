#include "ui/DashboardView.h"
#include "ui/ChartWidget.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>

// ------------------------------------------------------------
// Constructeur / destructeur
// ------------------------------------------------------------

DashboardView::DashboardView(QWidget* parent)
    : QMainWindow(parent)
    , aqiValue(nullptr)
    , co2Value(nullptr)
    , tttValue(nullptr)
    , tempValue(nullptr)
    , liveCo2(nullptr)          // <-- AJOUT
    , liveTemp(nullptr)         // <-- AJOUT
    , liveHum(nullptr)          // <-- AJOUT
    , liveSound(nullptr)        // <-- AJOUT
    , liveLight(nullptr)        // <-- AJOUT
    , ventilationState(nullptr)
    , purifierState(nullptr)
    , charts()
    , btnPlay(nullptr)
    , btnPause(nullptr)
    , btnReset(nullptr)
{
    buildUI();
}

DashboardView::~DashboardView() = default;

// ------------------------------------------------------------
// Construction de l'UI
// ------------------------------------------------------------

void DashboardView::buildUI()
{
    setWindowTitle("AP4A - Simulateur IoT Qualite de l'Air");
    resize(1400, 800);

    // Barre d'outils
    QToolBar* toolbar = addToolBar("Commandes");
    toolbar->setMovable(false);

    btnPlay = new QPushButton("Play", this);
    btnPause = new QPushButton("Pause", this);
    btnReset = new QPushButton("Reset", this);
    toolbar->addWidget(btnPlay);
    toolbar->addWidget(btnPause);
    toolbar->addWidget(btnReset);

    // Central
    QWidget* central = new QWidget(this);
    QHBoxLayout* mainLayout = new QHBoxLayout(central);

    // Colonne gauche : KPI + mesures en direct + actionneurs
    QVBoxLayout* leftLayout = new QVBoxLayout();
    buildKPIPanel(leftLayout);
    buildLiveMeasuresPanel(leftLayout);   // <-- AJOUT
    buildActuatorPanel(leftLayout);
    leftLayout->addStretch();

    // Colonne droite : grille de graphiques
    QGridLayout* gridLayout = new QGridLayout();
    buildChartsGrid(gridLayout);

    mainLayout->addLayout(leftLayout, 1);
    mainLayout->addLayout(gridLayout, 4);

    setCentralWidget(central);
}

// ------------------------------------------------------------
// Panneau KPI (moyennes + AQI)
// ------------------------------------------------------------

void DashboardView::buildKPIPanel(QVBoxLayout* parentLayout)
{
    QGroupBox* kpiBox = new QGroupBox("Indicateurs", this);
    QVBoxLayout* kpiLayout = new QVBoxLayout(kpiBox);

    aqiValue = new QLabel("-- / 100", kpiBox);
    co2Value = new QLabel("-- ppm", kpiBox);
    tttValue = new QLabel("-- ticks", kpiBox);
    tempValue = new QLabel("-- C", kpiBox);

    QFont bigFont; bigFont.setPointSize(14); bigFont.setBold(true);
    aqiValue->setFont(bigFont);
    co2Value->setFont(bigFont);

    kpiLayout->addWidget(new QLabel("AQI (qualite de l'air) :", kpiBox));
    kpiLayout->addWidget(aqiValue);
    kpiLayout->addSpacing(8);
    kpiLayout->addWidget(new QLabel("CO2 moyen :", kpiBox));
    kpiLayout->addWidget(co2Value);
    kpiLayout->addSpacing(8);
    kpiLayout->addWidget(new QLabel("Time-to-Threshold :", kpiBox));
    kpiLayout->addWidget(tttValue);
    kpiLayout->addSpacing(8);
    kpiLayout->addWidget(new QLabel("Temperature moyenne :", kpiBox));
    kpiLayout->addWidget(tempValue);
    kpiLayout->addStretch();

    parentLayout->addWidget(kpiBox);
}

// ------------------------------------------------------------
// Panneau "Mesures en direct" (5 capteurs, valeurs instantanees)
// ------------------------------------------------------------

void DashboardView::buildLiveMeasuresPanel(QVBoxLayout* parentLayout)
{
    QGroupBox* box = new QGroupBox("Mesures en direct", this);
    QVBoxLayout* layout = new QVBoxLayout(box);

    liveCo2 = new QLabel("-- ppm", box);
    liveTemp = new QLabel("-- C", box);
    liveHum = new QLabel("-- %", box);
    liveSound = new QLabel("-- dB", box);
    liveLight = new QLabel("--", box);

    QFont valFont; valFont.setPointSize(11); valFont.setBold(true);
    liveCo2->setFont(valFont);
    liveTemp->setFont(valFont);
    liveHum->setFont(valFont);
    liveSound->setFont(valFont);
    liveLight->setFont(valFont);

    // Petit helper : libelle + valeur sur la meme ligne
    auto addRow = [&](const QString& label, QLabel* value) {
        QHBoxLayout* row = new QHBoxLayout();
        QLabel* lbl = new QLabel(label, box);
        lbl->setMinimumWidth(110);
        row->addWidget(lbl);
        row->addWidget(value);
        row->addStretch();
        layout->addLayout(row);
        };

    addRow("CO2 :", liveCo2);
    addRow("Température :", liveTemp);
    addRow("Humidité :", liveHum);
    addRow("Son :", liveSound);
    addRow("Lumière :", liveLight);

    parentLayout->addWidget(box);
}

// ------------------------------------------------------------
// Panneau Actionneurs
// ------------------------------------------------------------

void DashboardView::buildActuatorPanel(QVBoxLayout* parentLayout)
{
    QGroupBox* actBox = new QGroupBox("Actionneurs", this);
    QVBoxLayout* actLayout = new QVBoxLayout(actBox);

    ventilationState = new QLabel("OFF", actBox);
    purifierState = new QLabel("OFF", actBox);

    QFont stateFont; stateFont.setPointSize(12); stateFont.setBold(true);
    ventilationState->setFont(stateFont);
    purifierState->setFont(stateFont);

    actLayout->addWidget(new QLabel("Ventilation :", actBox));
    actLayout->addWidget(ventilationState);
    actLayout->addSpacing(8);
    actLayout->addWidget(new QLabel("Purificateur d'air :", actBox));
    actLayout->addWidget(purifierState);

    parentLayout->addWidget(actBox);
}

// ------------------------------------------------------------
// Grille de graphiques 2x3 avec echelles Y adaptees
// ------------------------------------------------------------

void DashboardView::buildChartsGrid(QGridLayout* gridLayout)
{
    // Chaque graphique a une echelle Y adaptee a son type
    const struct {
        QString key;
        QString title;
        QString yLabel;
        double  maxY;
    } specs[] = {
        { "co2",         "CO2 (ppm)",         "ppm",  1500.0 },
        { "temperature", "Temperature (C)",   "C",    30.0   },
        { "humidity",    "Humidite (%)",      "%",    100.0  },
        { "sound",       "Son (dB)",          "dB",   100.0  },
        { "light",       "Lumiere (0/1)",     "bool", 1.0    },
    };

    int row = 0;
    int col = 0;
    for (const auto& spec : specs) {
        ChartWidget* chart = new ChartWidget(spec.title,
            spec.yLabel,
            spec.maxY,   // <-- echelle Y
            this);
        chart->setMinimumSize(350, 250);

        charts[spec.key] = chart;
        gridLayout->addWidget(chart, row, col);

        ++col;
        if (col >= 3) {   // 3 colonnes
            col = 0;
            ++row;
        }
    }
}

// ------------------------------------------------------------
// Mises a jour
// ------------------------------------------------------------

void DashboardView::updateKPIs(double aqi,
    double co2,
    double ttt,
    double temperature)
{
    aqiValue->setText(QString::number(aqi, 'f', 2) + " / 100");
    co2Value->setText(QString::number(co2, 'f', 1) + " ppm");
    tempValue->setText(QString::number(temperature, 'f', 2) + " C");

    if (ttt < 0.0) {
        tttValue->setText("stable (pas de depassement)");
    }
    else {
        tttValue->setText(QString::number(ttt, 'f', 1) + " ticks");
    }

    QString color;
    if (aqi >= 70.0)      color = "green";
    else if (aqi >= 40.0) color = "orange";
    else                  color = "red";

    aqiValue->setStyleSheet("color: " + color + ";");
}

void DashboardView::updateLiveMeasures(double co2, double temp, double hum,
    double sound, double light)
{
    liveCo2->setText(QString::number(co2, 'f', 1) + " ppm");
    liveTemp->setText(QString::number(temp, 'f', 2) + " C");
    liveHum->setText(QString::number(hum, 'f', 1) + " %");
    liveSound->setText(QString::number(sound, 'f', 1) + " dB");

    const bool day = (light > 0.5);
    liveLight->setText(day ? "Jour" : "Nuit");
    liveLight->setStyleSheet(day ? "color: orange; font-weight: bold;"
        : "color: gray; font-weight: bold;");
}

void DashboardView::updateChart(const QString& name, double x, double y)
{
    auto it = charts.find(name);
    if (it != charts.end() && it.value() != nullptr) {
        it.value()->addPoint(x, y);
    }
}

void DashboardView::updateActuatorState(const QString& name, bool active)
{
    const QString stateText = active ? "ON" : "OFF";
    const QString style = active
        ? "color: green; font-weight: bold;"
        : "color: gray; font-weight: bold;";

    if (name == "VentilationSystem" && ventilationState) {
        ventilationState->setText(stateText);
        ventilationState->setStyleSheet(style);
    }
    else if (name == "AirPurifier" && purifierState) {
        purifierState->setText(stateText);
        purifierState->setStyleSheet(style);
    }
}

void DashboardView::clearChart()
{
    for (auto it = charts.begin(); it != charts.end(); ++it) {
        if (it.value() != nullptr) {
            it.value()->clearData();
        }
    }
}

// ------------------------------------------------------------
// Accesseurs
// ------------------------------------------------------------

QPushButton* DashboardView::playButton()  const { return btnPlay; }
QPushButton* DashboardView::pauseButton() const { return btnPause; }
QPushButton* DashboardView::resetButton() const { return btnReset; }