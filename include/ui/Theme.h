#ifndef AIRQ_THEME_H
#define AIRQ_THEME_H

#include <QColor>
#include <QString>

// Toutes les couleurs de l'interface, en version claire ou sombre.
// Comme ca aucune couleur n'est ecrite en dur dans les widgets.
// styleSheet() genere le QSS pour toute la fenetre.
struct Theme {
    bool   dark;

    QColor window;       // fond
    QColor surface;      // cartes
    QColor surfaceAlt;   // boutons, badges eteints
    QColor border;
    QColor text;
    QColor textMuted;

    QColor accent;
    QColor success;
    QColor warning;
    QColor danger;

    // Une couleur par courbe
    QColor co2;
    QColor temperature;
    QColor humidity;
    QColor sound;
    QColor band;         // bandes "ventilation ON"

    static Theme darkTheme();
    static Theme lightTheme();

    QString styleSheet() const;
};

#endif // AIRQ_THEME_H
