#include "ui/DashboardView.h"
#include "ui/AqiGauge.h"
#include "ui/ChartWidget.h"
#include "ui/KpiCard.h"

#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QSlider>
#include <QStyle>
#include <QVBoxLayout>
#include <QWidget>

#include <cmath>

// ------------------------------------------------------------
// Constructeur / destructeur
// ------------------------------------------------------------

DashboardView::DashboardView(QWidget* parent)
    : QMainWindow(parent)
    , theme(Theme::darkTheme())
    , btnPlay(nullptr)
    , btnPause(nullptr)
    , btnReset(nullptr)
    , btnTheme(nullptr)
    , slider(nullptr)
    , speedValue(nullptr)
    , statusLabel(nullptr)
    , clockLabel(nullptr)
    , periodBadge(nullptr)
    , gauge(nullptr)
    , aqiLevel(nullptr)
    , co2Card(nullptr)
    , tttCard(nullptr)
    , tempCard(nullptr)
    , humCard(nullptr)
    , ventilationBadge(nullptr)
    , purifierBadge(nullptr)
    , alertList(nullptr)
    , charts()
{
    buildUI();
    applyTheme(theme);
    resetDisplay();
}

DashboardView::~DashboardView() = default;

// ------------------------------------------------------------
// Construction de la fenetre
// ------------------------------------------------------------

void DashboardView::buildUI()
{
    setWindowTitle("Simulateur IoT - Qualité de l'Air");
    resize(1200, 740);
    setMinimumSize(980, 620);

    QWidget* central = new QWidget(this);
    QVBoxLayout* root = new QVBoxLayout(central);
    root->setContentsMargins(12, 12, 12, 12);
    root->setSpacing(10);

    root->addWidget(buildTopBar());
    root->addWidget(buildKpiRow());

    QHBoxLayout* body = new QHBoxLayout();
    body->setSpacing(10);
    body->addWidget(buildCharts(), 3);
    body->addWidget(buildSidePanel(), 1);
    root->addLayout(body, 1);

    setCentralWidget(central);
}

QWidget* DashboardView::buildTopBar()
{
    QFrame* bar = new QFrame(this);
    bar->setObjectName("topBar");
    QHBoxLayout* layout = new QHBoxLayout(bar);
    layout->setContentsMargins(12, 8, 12, 8);
    layout->setSpacing(8);

    // Des caracteres plutot que des icones : ils prennent la couleur du theme
    btnPlay  = new QPushButton("▶  Lecture", bar);
    btnPause = new QPushButton("❚❚  Pause", bar);
    btnReset = new QPushButton("⟲  Reset", bar);
    btnPlay->setToolTip("Lancer la simulation");
    btnPause->setToolTip("Mettre en pause");
    btnReset->setToolTip("Remettre la simulation a zero (temps, piece, analyse, logs)");

    statusLabel = new QLabel(bar);
    statusLabel->setObjectName("status");

    QLabel* speedTitle = new QLabel("Vitesse", bar);
    slider = new QSlider(Qt::Horizontal, bar);
    slider->setRange(0, 3);           // x0.5, x1, x2, x4
    slider->setValue(1);
    slider->setPageStep(1);
    slider->setFixedWidth(110);
    speedValue = new QLabel(bar);
    speedValue->setMinimumWidth(36);

    btnTheme = new QPushButton(bar);
    connect(btnTheme, &QPushButton::clicked, this, &DashboardView::toggleTheme);

    clockLabel = new QLabel("--:--", bar);
    clockLabel->setObjectName("clock");
    periodBadge = new QLabel(bar);
    periodBadge->setObjectName("badge");

    layout->addWidget(btnPlay);
    layout->addWidget(btnPause);
    layout->addWidget(btnReset);
    layout->addSpacing(8);
    layout->addWidget(statusLabel);
    layout->addSpacing(16);
    layout->addWidget(speedTitle);
    layout->addWidget(slider);
    layout->addWidget(speedValue);
    layout->addStretch();
    layout->addWidget(btnTheme);
    layout->addSpacing(12);
    layout->addWidget(clockLabel);
    layout->addWidget(periodBadge);

    return bar;
}

QWidget* DashboardView::buildKpiRow()
{
    QWidget* row = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(row);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);

    // Carte AQI : la jauge + le texte a cote
    QFrame* aqiCard = new QFrame(row);
    aqiCard->setObjectName("card");
    QHBoxLayout* aqiLayout = new QHBoxLayout(aqiCard);
    aqiLayout->setContentsMargins(10, 6, 14, 6);

    gauge = new AqiGauge(aqiCard);
    QVBoxLayout* aqiText = new QVBoxLayout();
    QLabel* aqiTitle = new QLabel("Qualité de l'air", aqiCard);
    aqiTitle->setObjectName("cardTitle");
    aqiLevel = new QLabel(aqiCard);
    aqiLevel->setObjectName("cardSub");
    QFont levelFont = aqiLevel->font();
    levelFont.setPointSize(13);
    levelFont.setBold(true);
    aqiLevel->setFont(levelFont);
    aqiText->addStretch();
    aqiText->addWidget(aqiTitle);
    aqiText->addWidget(aqiLevel);
    aqiText->addStretch();

    aqiLayout->addWidget(gauge);
    aqiLayout->addLayout(aqiText);

    co2Card  = new KpiCard("CO2", row);
    tttCard  = new KpiCard("Seuil 1000 ppm atteint dans", row);
    tempCard = new KpiCard("Température", row);
    humCard  = new KpiCard("Humidité", row);

    layout->addWidget(aqiCard, 5);
    layout->addWidget(co2Card, 4);
    layout->addWidget(tttCard, 4);
    layout->addWidget(tempCard, 4);
    layout->addWidget(humCard, 4);

    return row;
}

QWidget* DashboardView::buildCharts()
{
    QWidget* area = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(area);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);

    ChartWidget* co2 = new ChartWidget("CO2 (ppm)  -  bandes : ventilation active", "ppm", 300.0, area);
    co2->setThreshold(CO2_THRESHOLD);
    charts["co2"] = co2;
    layout->addWidget(co2, 3);

    const struct {
        QString key;
        QString title;
        QString unit;
        double  minSpan;
    } small[] = {
        { "temperature", "Température (°C)", "°C", 4.0  },
        { "humidity",    "Humidité (%)",     "%",  10.0 },
        { "sound",       "Son (dB)",         "dB", 20.0 },
    };

    QHBoxLayout* row = new QHBoxLayout();
    row->setSpacing(10);
    for (const auto& spec : small) {
        ChartWidget* chart = new ChartWidget(spec.title, spec.unit, spec.minSpan, area);
        chart->setCompact(true);
        charts[spec.key] = chart;
        row->addWidget(chart);
    }
    layout->addLayout(row, 2);

    return area;
}

QWidget* DashboardView::buildSidePanel()
{
    QWidget* panel = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(panel);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);

    // Actionneurs
    QFrame* actCard = new QFrame(panel);
    actCard->setObjectName("card");
    QGridLayout* grid = new QGridLayout(actCard);
    grid->setContentsMargins(14, 10, 14, 12);
    grid->setVerticalSpacing(8);

    QLabel* actTitle = new QLabel("Actionneurs", actCard);
    actTitle->setObjectName("cardTitle");
    ventilationBadge = new QLabel(actCard);
    ventilationBadge->setObjectName("badge");
    purifierBadge = new QLabel(actCard);
    purifierBadge->setObjectName("badge");

    grid->addWidget(actTitle, 0, 0, 1, 2);
    grid->addWidget(new QLabel("Ventilation", actCard), 1, 0);
    grid->addWidget(ventilationBadge, 1, 1, Qt::AlignRight);
    grid->addWidget(new QLabel("Purificateur", actCard), 2, 0);
    grid->addWidget(purifierBadge, 2, 1, Qt::AlignRight);

    // Alertes
    QFrame* alertCard = new QFrame(panel);
    alertCard->setObjectName("card");
    QVBoxLayout* alertLayout = new QVBoxLayout(alertCard);
    alertLayout->setContentsMargins(14, 10, 8, 10);

    QLabel* alertTitle = new QLabel("Alertes", alertCard);
    alertTitle->setObjectName("cardTitle");
    alertList = new QListWidget(alertCard);
    alertList->setWordWrap(true);
    alertList->setFocusPolicy(Qt::NoFocus);
    alertList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    alertLayout->addWidget(alertTitle);
    alertLayout->addWidget(alertList, 1);

    layout->addWidget(actCard);
    layout->addWidget(alertCard, 1);
    panel->setMinimumWidth(240);

    return panel;
}

// ------------------------------------------------------------
// Theme
// ------------------------------------------------------------

void DashboardView::toggleTheme()
{
    applyTheme(theme.dark ? Theme::lightTheme() : Theme::darkTheme());
}

void DashboardView::applyTheme(const Theme& t)
{
    theme = t;
    setStyleSheet(theme.styleSheet());

    btnTheme->setText(theme.dark ? "Thème clair" : "Thème sombre");
    gauge->setTheme(theme);

    const QMap<QString, QColor> colors = {
        { "co2", theme.co2 }, { "temperature", theme.temperature },
        { "humidity", theme.humidity }, { "sound", theme.sound },
    };
    for (auto it = charts.begin(); it != charts.end(); ++it) {
        it.value()->setTheme(theme, colors.value(it.key(), theme.accent));
    }

    for (int i = 0; i < alertList->count(); ++i) {
        colorAlert(i);
    }
}

void DashboardView::setStyleState(QWidget* w, const char* property, const QString& value)
{
    if (w->property(property).toString() == value) {
        return;
    }
    w->setProperty(property, value);
    w->style()->unpolish(w);
    w->style()->polish(w);
}

// ------------------------------------------------------------
// Mises a jour
// ------------------------------------------------------------

void DashboardView::updateClock(const QString& time, bool day)
{
    clockLabel->setText(time);
    periodBadge->setText(day ? "Jour" : "Nuit");
    setStyleState(periodBadge, "state", day ? "day" : "night");
}

void DashboardView::updateKPIs(double aqi, double ttt)
{
    gauge->setValue(aqi);
    aqiLevel->setText(AqiGauge::levelLabel(aqi));
    setStyleState(aqiLevel, "tone", AqiGauge::levelTone(aqi));

    if (ttt < 0.0) {
        tttCard->setValue("Stable");
        tttCard->setSubtitle("aucun dépassement prévu", "good");
    }
    else {
        tttCard->setValue(QString::number(ttt, 'f', 0) + " min");
        tttCard->setSubtitle("prévision par régression", ttt < 60.0 ? "bad" : "warn");
    }
}

void DashboardView::updateMeasure(const QString& key, double last, double average)
{
    struct Format { KpiCard* card; QString unit; int decimals; double tolerance; bool risingIsBad; };
    const QMap<QString, Format> formats = {
        { "co2",         { co2Card,  " ppm", 0, 15.0, true  } },
        { "temperature", { tempCard, " °C",  1, 0.2,  false } },
        { "humidity",    { humCard,  " %",   0, 1.0,  false } },
    };
    if (!formats.contains(key)) {
        return;
    }
    const Format f = formats.value(key);

    f.card->setValue(QString::number(last, 'f', f.decimals) + f.unit);

    // Tendance : on compare la derniere valeur a la moyenne
    const double delta = last - average;
    QString arrow = "→ stable";
    QString tone = "neutral";
    if (std::fabs(delta) > f.tolerance) {
        arrow = delta > 0 ? "↑ en hausse" : "↓ en baisse";
        if (f.risingIsBad) {
            tone = delta > 0 ? "bad" : "good";
        }
    }
    f.card->setSubtitle(arrow + "  ·  moy. "
        + QString::number(average, 'f', f.decimals) + f.unit, tone);
}

void DashboardView::updateChart(const QString& key, double x, double y)
{
    if (ChartWidget* chart = charts.value(key, nullptr)) {
        chart->addPoint(x, y);
    }
}

void DashboardView::updateActuatorState(const QString& name, bool active, double x)
{
    QLabel* badge = nullptr;
    if (name == "VentilationSystem") {
        badge = ventilationBadge;
        if (ChartWidget* co2 = charts.value("co2", nullptr)) {
            co2->setBandState(x, active);
        }
    }
    else if (name == "AirPurifier") {
        badge = purifierBadge;
    }

    if (badge) {
        badge->setText(active ? "ON" : "OFF");
        setStyleState(badge, "state", active ? "on" : "off");
    }
}

void DashboardView::addAlert(const QString& timestamp,
    const QString& category,
    const QString& message)
{
    static const QMap<QString, QString> labels = {
        { "ANOMALY", "Anomalie" }, { "CO2", "CO2" }, { "ACTUATOR", "Action" },
    };

    // On remplace les noms de classes par des noms lisibles
    QString text = message;
    if (category == "ACTUATOR") {
        const QString suffix = text.contains("VentilationSystem") ? "e" : "";   // "la" ventilation
        text.replace("VentilationSystem", "Ventilation")
            .replace("AirPurifier", "Purificateur")
            .replace(" desactive", " désactivé" + suffix)
            .replace(" active", " activé" + suffix);
    }

    // Juste l'heure, la date ne sert a rien ici
    const QString time = timestamp.mid(timestamp.indexOf('T') + 1, 5);
    QListWidgetItem* item = new QListWidgetItem(
        time + "   " + labels.value(category, category) + "\n" + text);
    item->setData(Qt::UserRole, category);
    alertList->insertItem(0, item);
    colorAlert(0);

    while (alertList->count() > MAX_ALERTS) {
        delete alertList->takeItem(alertList->count() - 1);
    }
}

void DashboardView::colorAlert(int row)
{
    QListWidgetItem* item = alertList->item(row);
    const QString category = item->data(Qt::UserRole).toString();

    QColor color = theme.text;
    if (category == "ANOMALY")  color = theme.danger;
    else if (category == "CO2") color = theme.warning;
    item->setForeground(color);
}

void DashboardView::setRunning(bool running)
{
    statusLabel->setText(running ? "● En cours" : "❚❚ En pause");
    setStyleState(statusLabel, "state", running ? "on" : "off");
}

void DashboardView::setSpeedLabel(const QString& text)
{
    speedValue->setText(text);
}

void DashboardView::resetDisplay()
{
    clockLabel->setText("--:--");
    periodBadge->setText("Jour");
    setStyleState(periodBadge, "state", "day");

    gauge->setValue(-1.0);
    aqiLevel->setText("--");
    setStyleState(aqiLevel, "tone", "neutral");

    for (KpiCard* card : { co2Card, tttCard, tempCard, humCard }) {
        card->clear();
    }

    updateActuatorState("VentilationSystem", false, 0.0);
    updateActuatorState("AirPurifier", false, 0.0);

    alertList->clear();
    for (ChartWidget* chart : charts) {
        chart->clearData();
    }
}

// ------------------------------------------------------------
// Accesseurs
// ------------------------------------------------------------

QPushButton* DashboardView::playButton()  const { return btnPlay; }
QPushButton* DashboardView::pauseButton() const { return btnPause; }
QPushButton* DashboardView::resetButton() const { return btnReset; }
QSlider*     DashboardView::speedSlider() const { return slider; }
