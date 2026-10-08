#include "ui/ChartWidget.h"

#include <QtCharts/QAreaSeries>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

#include <QPen>

#include <algorithm>
#include <cmath>

ChartWidget::ChartWidget(const QString& title,
    const QString& unitLabel,
    double minSpanInit,
    QWidget* parent)
    : QChartView(parent)
    , series(new QLineSeries(this))
    , thresholdSeries(new QLineSeries(this))
    , bandUpper(new QLineSeries(this))
    , bandLower(new QLineSeries(this))
    , bandArea(new QAreaSeries(bandUpper, bandLower))
    , axisX(new QValueAxis(this))
    , axisY(new QValueAxis(this))
    , unit(unitLabel)
    , minSpan(minSpanInit)
    , threshold(0.0)
    , hasThreshold(false)
    , bandChanges()
{
    QChart* chart = new QChart();
    chart->setTitle(title);
    chart->legend()->hide();
    chart->setAnimationOptions(QChart::NoAnimation);
    chart->setMargins(QMargins(4, 4, 4, 4));
    chart->setBackgroundRoundness(10);

    // Ajoutees dans cet ordre pour que la courbe soit dessinee par dessus
    chart->addSeries(bandArea);
    chart->addSeries(thresholdSeries);
    chart->addSeries(series);

    axisX->setTitleText("Temps écoulé (h)");
    axisX->setLabelFormat("%.0f");
    axisX->setRange(0, INITIAL_X_SPAN);
    chart->addAxis(axisX, Qt::AlignBottom);

    axisY->setTitleText(unit);
    axisY->setLabelFormat("%.0f");
    axisY->setTickCount(5);
    chart->addAxis(axisY, Qt::AlignLeft);

    for (QAbstractSeries* s : { static_cast<QAbstractSeries*>(bandArea),
                                static_cast<QAbstractSeries*>(thresholdSeries),
                                static_cast<QAbstractSeries*>(series) }) {
        s->attachAxis(axisX);
        s->attachAxis(axisY);
    }

    thresholdSeries->setVisible(false);

    setChart(chart);
    setRenderHint(QPainter::Antialiasing);
    setMinimumHeight(150);

    setTheme(Theme::darkTheme(), Theme::darkTheme().co2);
}

// ------------------------------------------------------------
// Donnees
// ------------------------------------------------------------

void ChartWidget::addPoint(double x, double y)
{
    series->append(x, y);

    // On ne garde que les MAX_POINTS derniers points
    while (series->count() > MAX_POINTS) {
        series->remove(0);
    }

    updateAxes();
}

void ChartWidget::setThreshold(double y)
{
    threshold = y;
    hasThreshold = true;
    thresholdSeries->setVisible(true);
    updateAxes();
}

void ChartWidget::setBandState(double x, bool on)
{
    const bool current = !bandChanges.isEmpty() && bandChanges.last().second;
    if (on != current) {
        bandChanges.append({ x, on });
    }
}

void ChartWidget::clearData()
{
    series->clear();
    bandUpper->clear();
    bandLower->clear();
    bandChanges.clear();
    axisX->setRange(0, INITIAL_X_SPAN);
    updateAxes();
}

// ------------------------------------------------------------
// Axes, seuil et bandes
// ------------------------------------------------------------

void ChartWidget::updateAxes()
{
    double xMin = 0.0;
    double xMax = INITIAL_X_SPAN;
    double yMin = 0.0;
    double yMax = minSpan;
    bool   nonNegative = true;

    const auto points = series->points();
    if (!points.isEmpty()) {
        xMin = points.first().x();
        xMax = std::max(xMin + INITIAL_X_SPAN, points.last().x());

        auto [lo, hi] = std::minmax_element(points.begin(), points.end(),
            [](const QPointF& a, const QPointF& b) { return a.y() < b.y(); });
        yMin = lo->y();
        yMax = hi->y();
        nonNegative = (yMin >= 0.0);
    }

    // On veut toujours voir la ligne de seuil
    if (hasThreshold) {
        yMin = std::min(yMin, threshold);
        yMax = std::max(yMax, threshold);
    }

    // Hauteur mini pour ne pas zoomer sur le bruit, + 10 % de marge
    const double center = (yMin + yMax) / 2.0;
    const double span = std::max(yMax - yMin, minSpan) * 1.1;
    yMin = center - span / 2.0;
    yMax = center + span / 2.0;
    if (nonNegative && yMin < 0.0) {
        yMax -= yMin;   // pas d'axe negatif si toutes les valeurs sont positives
        yMin = 0.0;
    }

    axisX->setRange(xMin, xMax);
    axisY->setRange(yMin, yMax);
    axisY->applyNiceNumbers();
    yMin = axisY->min();
    yMax = axisY->max();

    if (hasThreshold) {
        thresholdSeries->replace({ QPointF(xMin, threshold), QPointF(xMax, threshold) });
    }
    rebuildBands(xMin, xMax, yMin, yMax);
}

void ChartWidget::rebuildBands(double xMin, double xMax, double yMin, double yMax)
{
    // On oublie ce qui est sorti du graphique (sauf le dernier etat avant xMin)
    while (bandChanges.size() > 1 && bandChanges[1].first <= xMin) {
        bandChanges.removeFirst();
    }

    // Courbe en escalier : en haut quand c'est allume, en bas sinon
    QList<QPointF> upper;
    QList<QPointF> lower;
    bool state = false;
    double x = xMin;

    for (const auto& [changeX, on] : bandChanges) {
        if (changeX <= xMin) {
            state = on;
            continue;
        }
        upper.append({ x, state ? yMax : yMin });
        upper.append({ changeX, state ? yMax : yMin });
        x = changeX;
        state = on;
    }
    upper.append({ x, state ? yMax : yMin });
    upper.append({ xMax, state ? yMax : yMin });

    for (const QPointF& p : upper) {
        lower.append({ p.x(), yMin });
    }

    bandUpper->replace(upper);
    bandLower->replace(lower);
}

// ------------------------------------------------------------
// Apparence
// ------------------------------------------------------------

void ChartWidget::setCompact(bool compact)
{
    axisX->setTitleVisible(!compact);
    axisY->setTitleVisible(!compact);
    axisY->setTickCount(compact ? 3 : 5);
    setMinimumHeight(compact ? 130 : 150);
}

void ChartWidget::setTheme(const Theme& theme, const QColor& seriesColor)
{
    QChart* c = chart();
    c->setBackgroundBrush(theme.surface);
    c->setBackgroundPen(QPen(theme.border));
    c->setPlotAreaBackgroundVisible(false);
    c->setTitleBrush(theme.text);

    QFont titleFont = c->titleFont();
    titleFont.setPointSizeF(9.5);
    titleFont.setBold(true);
    c->setTitleFont(titleFont);

    for (QValueAxis* axis : { axisX, axisY }) {
        axis->setLabelsColor(theme.textMuted);
        axis->setTitleBrush(theme.textMuted);
        axis->setLinePenColor(theme.border);
        axis->setGridLineColor(theme.border);
        QFont f = axis->labelsFont();
        f.setPointSizeF(8);
        axis->setLabelsFont(f);
        axis->setTitleFont(f);
    }

    QPen seriesPen(seriesColor, 2.2);
    series->setPen(seriesPen);

    QPen thresholdPen(theme.danger, 1.4, Qt::DashLine);
    thresholdSeries->setPen(thresholdPen);

    bandArea->setBrush(theme.band);
    bandArea->setPen(Qt::NoPen);

    setBackgroundBrush(theme.window);
}
