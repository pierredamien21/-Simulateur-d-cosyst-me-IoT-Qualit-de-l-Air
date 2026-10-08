#ifndef AIRQ_KPI_CARD_H
#define AIRQ_KPI_CARD_H

#include <QFrame>
#include <QString>

class QLabel;

// Petite carte avec un titre, une grosse valeur et une ligne en dessous.
// La couleur de la ligne du bas depend du "tone" (neutral, good,
// warn, bad), c'est le QSS du theme qui choisit la couleur.
class KpiCard : public QFrame {
    Q_OBJECT

public:
    explicit KpiCard(const QString& title, QWidget* parent = nullptr);

    void setValue(const QString& value);
    void setSubtitle(const QString& text, const QString& tone = "neutral");
    void clear();

private:
    QLabel* titleLabel;
    QLabel* valueLabel;
    QLabel* subLabel;
};

#endif // AIRQ_KPI_CARD_H
