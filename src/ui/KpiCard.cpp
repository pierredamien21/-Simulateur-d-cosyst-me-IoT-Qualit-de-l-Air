#include "ui/KpiCard.h"

#include <QLabel>
#include <QStyle>
#include <QVBoxLayout>

KpiCard::KpiCard(const QString& title, QWidget* parent)
    : QFrame(parent)
    , titleLabel(new QLabel(title, this))
    , valueLabel(new QLabel(this))
    , subLabel(new QLabel(this))
{
    setObjectName("card");
    titleLabel->setObjectName("cardTitle");
    valueLabel->setObjectName("cardValue");
    subLabel->setObjectName("cardSub");

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(14, 10, 14, 10);
    layout->setSpacing(2);
    layout->addWidget(titleLabel);
    layout->addWidget(valueLabel);
    layout->addWidget(subLabel);
    layout->addStretch();

    clear();
}

void KpiCard::setValue(const QString& value)
{
    valueLabel->setText(value);
}

void KpiCard::setSubtitle(const QString& text, const QString& tone)
{
    subLabel->setText(text);
    if (subLabel->property("tone").toString() != tone) {
        subLabel->setProperty("tone", tone);
        // Sinon Qt ne reapplique pas le style
        subLabel->style()->unpolish(subLabel);
        subLabel->style()->polish(subLabel);
    }
}

void KpiCard::clear()
{
    setValue("--");
    setSubtitle(" ");
}
