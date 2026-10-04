#ifndef AP4A_CHART_WIDGET_H
#define AP4A_CHART_WIDGET_H

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

class QDateTimeAxis;
class QValueAxis;

/**
 * @brief Widget de courbe temps reel (Qt Charts).
 *
 * Affiche une seule serie de donnees sur un axe X = tick
 * et un axe Y = valeur. Limite le nombre de points affiches
 * (fenetre glissante) pour eviter la surcharge graphique.
 *
 * Le widget ne contient AUCUNE logique metier : il ne fait
 * qu'afficher les donnees qu'on lui pousse via addPoint().
 */
class ChartWidget : public QChartView {
    Q_OBJECT

public:
    static constexpr int MAX_POINTS = 100;

    explicit ChartWidget(const QString& title,
        const QString& yLabel,
        double maxY = 100.0,        // <-- NOUVEAU
        QWidget* parent = nullptr);

    // Ajoute un point (x = tick, y = valeur)
    void addPoint(double x, double y);

    // Efface toutes les donnees
    void clearData();

private:
    QLineSeries* series;
    QValueAxis* axisX;
    QValueAxis* axisY;
    double        maxY;
};

#endif // AP4A_CHART_WIDGET_H