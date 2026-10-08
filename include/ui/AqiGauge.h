#ifndef AIRQ_AQI_GAUGE_H
#define AIRQ_AQI_GAUGE_H

#include "ui/Theme.h"

#include <QWidget>

// Jauge ronde pour l'AQI (0 a 100), dessinee a la main avec QPainter.
// Vert si c'est bon, orange si moyen, rouge si mauvais.
class AqiGauge : public QWidget {
    Q_OBJECT

public:
    static constexpr double GOOD_LEVEL = 70.0;
    static constexpr double FAIR_LEVEL = 40.0;

    explicit AqiGauge(QWidget* parent = nullptr);

    void setValue(double aqi);   // valeur negative = rien a afficher
    void setTheme(const Theme& theme);

    QSize sizeHint() const override;

    // "Bon" / "Moyen" / "Mauvais" et le tone qui va avec (la vue s'en sert aussi)
    static QString levelLabel(double aqi);
    static QString levelTone(double aqi);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    double value;
    Theme  theme;
};

#endif // AIRQ_AQI_GAUGE_H
