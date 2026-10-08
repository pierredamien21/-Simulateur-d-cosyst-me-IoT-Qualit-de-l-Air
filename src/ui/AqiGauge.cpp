#include "ui/AqiGauge.h"

#include <QPainter>
#include <QPen>

#include <algorithm>

AqiGauge::AqiGauge(QWidget* parent)
    : QWidget(parent)
    , value(-1.0)
    , theme(Theme::darkTheme())
{
    setMinimumSize(84, 84);
}

void AqiGauge::setValue(double aqi)
{
    value = aqi;
    update();
}

void AqiGauge::setTheme(const Theme& t)
{
    theme = t;
    update();
}

QSize AqiGauge::sizeHint() const
{
    return QSize(92, 92);
}

QString AqiGauge::levelLabel(double aqi)
{
    if (aqi >= GOOD_LEVEL) return "Bon";
    if (aqi >= FAIR_LEVEL) return "Moyen";
    return "Mauvais";
}

QString AqiGauge::levelTone(double aqi)
{
    if (aqi >= GOOD_LEVEL) return "good";
    if (aqi >= FAIR_LEVEL) return "warn";
    return "bad";
}

void AqiGauge::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const int side = std::min(width(), height()) - 12;
    const QRectF arcRect((width() - side) / 2.0, (height() - side) / 2.0, side, side);

    // Arc de 270 degres, ouvert en bas (Qt compte en 1/16 de degre)
    constexpr int START = 225 * 16;
    constexpr int SPAN  = -270 * 16;

    QPen pen(theme.surfaceAlt, 9, Qt::SolidLine, Qt::RoundCap);
    p.setPen(pen);
    p.drawArc(arcRect, START, SPAN);

    QString text = "--";
    if (value >= 0.0) {
        const double ratio = std::clamp(value / 100.0, 0.0, 1.0);
        QColor color = theme.danger;
        if (value >= GOOD_LEVEL)      color = theme.success;
        else if (value >= FAIR_LEVEL) color = theme.warning;

        pen.setColor(color);
        p.setPen(pen);
        p.drawArc(arcRect, START, static_cast<int>(SPAN * ratio));
        text = QString::number(value, 'f', 0);
    }

    // Valeur au centre
    QFont font = p.font();
    font.setPointSizeF(16);
    font.setBold(true);
    p.setFont(font);
    p.setPen(theme.text);
    p.drawText(arcRect, Qt::AlignCenter, text);

    font.setPointSizeF(7.5);
    font.setBold(false);
    p.setFont(font);
    p.setPen(theme.textMuted);
    p.drawText(arcRect.adjusted(0, 0, 0, -6), Qt::AlignHCenter | Qt::AlignBottom, "/ 100");
}
