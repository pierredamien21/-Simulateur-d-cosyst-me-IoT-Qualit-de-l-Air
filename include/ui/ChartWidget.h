#ifndef AIRQ_CHART_WIDGET_H
#define AIRQ_CHART_WIDGET_H

#include "ui/Theme.h"

#include <QtCharts/QChartView>

#include <QColor>
#include <QPair>
#include <QVector>

class QAreaSeries;
class QLineSeries;
class QValueAxis;

// Une courbe en temps reel (Qt Charts) qui garde les 100 derniers points.
// En plus de la courbe on peut afficher :
//  - une ligne de seuil (ex : 1000 ppm)
//  - des bandes en fond quand un truc est allume (ex : la ventilation)
// L'axe Y s'adapte aux valeurs, avec une hauteur mini (minSpan)
// pour ne pas zoomer sur le bruit.
class ChartWidget : public QChartView {
    Q_OBJECT

public:
    static constexpr int    MAX_POINTS     = 100;
    static constexpr double INITIAL_X_SPAN = 10.0;   // en heures

    ChartWidget(const QString& title,
        const QString& unit,
        double minSpan,
        QWidget* parent = nullptr);

    // x en heures simulees
    void addPoint(double x, double y);

    void setThreshold(double y);

    // A appeler a chaque image avec l'etat actuel (allume ou pas)
    void setBandState(double x, bool on);

    // Pour les petits graphiques : pas de titre sur les axes
    void setCompact(bool compact);

    void setTheme(const Theme& theme, const QColor& seriesColor);

    void clearData();

private:
    void updateAxes();
    void rebuildBands(double xMin, double xMax, double yMin, double yMax);

    QLineSeries* series;
    QLineSeries* thresholdSeries;
    QLineSeries* bandUpper;
    QLineSeries* bandLower;
    QAreaSeries* bandArea;
    QValueAxis*  axisX;
    QValueAxis*  axisY;

    QString unit;
    double  minSpan;
    double  threshold;
    bool    hasThreshold;

    // Moments ou l'etat a change : (x, allume)
    QVector<QPair<double, bool>> bandChanges;
};

#endif // AIRQ_CHART_WIDGET_H
