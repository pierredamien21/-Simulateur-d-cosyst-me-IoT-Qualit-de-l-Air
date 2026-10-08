#include "ui/Theme.h"

// ------------------------------------------------------------
// Couleur -> texte pour le QSS (avec la transparence si besoin)
// ------------------------------------------------------------
static QString css(const QColor& c)
{
    if (c.alpha() == 255) {
        return c.name();
    }
    return QString("rgba(%1, %2, %3, %4)")
        .arg(c.red()).arg(c.green()).arg(c.blue()).arg(c.alpha());
}

static QColor withAlpha(QColor c, int alpha)
{
    c.setAlpha(alpha);
    return c;
}

// ------------------------------------------------------------
// Palettes
// ------------------------------------------------------------

Theme Theme::darkTheme()
{
    Theme t;
    t.dark        = true;
    t.window      = QColor("#15171c");
    t.surface     = QColor("#1e2129");
    t.surfaceAlt  = QColor("#2a2e38");
    t.border      = QColor("#343947");
    t.text        = QColor("#e8eaf0");
    t.textMuted   = QColor("#9aa1b2");
    t.accent      = QColor("#5b9cf0");
    t.success     = QColor("#5fc27e");
    t.warning     = QColor("#e8b04a");
    t.danger      = QColor("#ef6b6b");
    t.co2         = QColor("#5b9cf0");
    t.temperature = QColor("#f0875b");
    t.humidity    = QColor("#3ec1a6");
    t.sound       = QColor("#a594f0");
    t.band        = withAlpha(QColor("#5b9cf0"), 45);
    return t;
}

Theme Theme::lightTheme()
{
    Theme t;
    t.dark        = false;
    t.window      = QColor("#f2f3f6");
    t.surface     = QColor("#ffffff");
    t.surfaceAlt  = QColor("#eceef2");
    t.border      = QColor("#dcdfe6");
    t.text        = QColor("#1d2230");
    t.textMuted   = QColor("#646b7d");
    t.accent      = QColor("#2f6fd0");
    t.success     = QColor("#2e8a4c");
    t.warning     = QColor("#b07812");
    t.danger      = QColor("#c9373a");
    t.co2         = QColor("#2f6fd0");
    t.temperature = QColor("#d4602f");
    t.humidity    = QColor("#14917a");
    t.sound       = QColor("#6a55c9");
    t.band        = withAlpha(QColor("#2f6fd0"), 35);
    return t;
}

// ------------------------------------------------------------
// QSS de toute la fenetre
// ------------------------------------------------------------

QString Theme::styleSheet() const
{
    const int tintAlpha = dark ? 50 : 35;

    QString qss = R"(
QWidget { background: %WINDOW%; color: %TEXT%; font-family: "Segoe UI"; font-size: 10pt; }
QLabel { background: transparent; }

QFrame#topBar, QFrame#card { background: %SURFACE%; border: 1px solid %BORDER%; border-radius: 10px; }

QLabel#cardTitle { color: %MUTED%; font-size: 9pt; }
QLabel#cardValue { font-size: 17pt; font-weight: 600; }
QLabel#cardSub   { color: %MUTED%; font-size: 9pt; }
QLabel#cardSub[tone="bad"]  { color: %DANGER%; }
QLabel#cardSub[tone="warn"] { color: %WARNING%; }
QLabel#cardSub[tone="good"] { color: %SUCCESS%; }

QLabel#clock  { font-size: 16pt; font-weight: 600; }
QLabel#status { color: %MUTED%; }
QLabel#status[state="on"] { color: %SUCCESS%; }

QLabel#badge { border-radius: 9px; padding: 2px 10px; font-weight: 600;
               background: %SURFACE_ALT%; color: %MUTED%; }
QLabel#badge[state="on"]    { background: %SUCCESS_TINT%; color: %SUCCESS%; }
QLabel#badge[state="day"]   { background: %WARNING_TINT%; color: %WARNING%; }
QLabel#badge[state="night"] { background: %ACCENT_TINT%;  color: %ACCENT%; }

QPushButton { background: %SURFACE_ALT%; border: 1px solid %BORDER%; border-radius: 6px;
              padding: 5px 12px; }
QPushButton:hover   { border-color: %ACCENT%; }
QPushButton:pressed { background: %BORDER%; }

QSlider { background: transparent; }
QSlider::groove:horizontal   { height: 4px; background: %BORDER%; border-radius: 2px; }
QSlider::sub-page:horizontal { background: %ACCENT%; border-radius: 2px; }
QSlider::handle:horizontal   { background: %ACCENT%; width: 14px; margin: -6px 0; border-radius: 7px; }

QListWidget { background: transparent; border: none; }
QListWidget::item { padding: 4px 2px; border-bottom: 1px solid %BORDER%; }
QListWidget::item:selected { background: %SURFACE_ALT%; color: %TEXT%; }

QScrollBar:vertical { background: transparent; width: 8px; margin: 0; }
QScrollBar::handle:vertical { background: %BORDER%; border-radius: 4px; min-height: 24px; }
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: transparent; }
)";

    qss.replace("%WINDOW%",       css(window))
       .replace("%SURFACE_ALT%",  css(surfaceAlt))
       .replace("%SURFACE%",      css(surface))
       .replace("%BORDER%",       css(border))
       .replace("%TEXT%",         css(text))
       .replace("%MUTED%",        css(textMuted))
       .replace("%SUCCESS_TINT%", css(withAlpha(success, tintAlpha)))
       .replace("%WARNING_TINT%", css(withAlpha(warning, tintAlpha)))
       .replace("%ACCENT_TINT%",  css(withAlpha(accent, tintAlpha)))
       .replace("%ACCENT%",       css(accent))
       .replace("%SUCCESS%",      css(success))
       .replace("%WARNING%",      css(warning))
       .replace("%DANGER%",       css(danger));
    return qss;
}
