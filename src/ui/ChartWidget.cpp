#include "ui/ChartWidget.h"

#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>
#include <algorithm>

ChartWidget::ChartWidget(const QString& title,
    const QString& yLabel,
    double maxYInit,
    QWidget* parent)
    : QChartView(parent)
    , series(new QLineSeries(this))
    , axisX(new QValueAxis(this))
    , axisY(new QValueAxis(this))
    , maxY(maxYInit)                    // <-- init depuis parametre
{
    // --- Serie ---
    series->setName(title);

    // --- Graphique ---
    QChart* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(title);
    chart->legend()->hide();
    chart->setAnimationOptions(QChart::NoAnimation);

    // --- Axe X (tick) ---
    axisX->setTitleText("Ticks");
    axisX->setRange(0, MAX_POINTS);
    axisX->setLabelFormat("%d");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    // --- Axe Y (valeur) ---
    axisY->setTitleText(yLabel);
    axisY->setRange(0, maxY);
    axisY->setLabelFormat("%.2f");      // 2 decimales pour la lumiere
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    // --- Application ---
    setChart(chart);
    setRenderHint(QPainter::Antialiasing);

    // Fond
    setBackgroundBrush(QBrush(QColor(250, 250, 250)));
}

void ChartWidget::addPoint(double x, double y)
{
    series->append(x, y);

    // Fenetre glissante : supprime les points les plus anciens
    while (series->count() > MAX_POINTS) {
        series->remove(0);
    }

    // Ajuste la plage de l'axe X pour suivre les derniers points
    const qreal xMin = series->at(0).x();
    const qreal xMax = series->at(series->count() - 1).x();
    axisX->setRange(std::max(0.0, xMin), xMax + 1.0);

    // Ajuste l'axe Y si on depasse la plage actuelle
    if (y > maxY) {
        maxY = y * 1.2;
        axisY->setRange(0, maxY);
    }
}

void ChartWidget::clearData()
{
    series->clear();
    axisX->setRange(0, MAX_POINTS);
    // NOTE : on ne reinitialise PAS maxY : chaque graphique garde
    // son echelle adaptee (fixee a la construction).
    axisY->setRange(0, maxY);
}