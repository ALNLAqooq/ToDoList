#ifndef THEME_UTILS_H
#define THEME_UTILS_H

#include <QString>
#include <QColor>
#include <QMap>

class ThemeUtils
{
public:
    enum Theme {
        Light,
        Dark
    };

    enum Priority {
        High,
        Medium,
        Low
    };

    static QString loadStyleSheet(const QString &filePath);
    static bool saveStyleSheet(const QString &filePath, const QString &styleSheet);
    static QString getDefaultLightTheme();
    static QString getDefaultDarkTheme();
    static QString getThemeForType(Theme theme);

    static QColor getPrimaryColor(Theme theme = Light);
    static QColor getSecondaryColor(Theme theme = Light);
    static QColor getBackgroundColor(Theme theme = Light);
    static QColor getSurfaceColor(Theme theme = Light);
    static QColor getTextColor(Theme theme = Light);
    static QColor getMutedTextColor(Theme theme = Light);
    static QColor getBorderColor(Theme theme = Light);

    static QColor getPriorityColor(Priority priority);
    static QString getPriorityColorName(Priority priority);
    static QString getPriorityStyleSheet(Priority priority);

    static QString getColorVariable(const QString &name, Theme theme = Light);
    static QMap<QString, QString> getColorVariables(Theme theme = Light);

    static QString applyThemeVariables(const QString &styleSheet, Theme theme = Light);
    static QString replaceColorVariables(const QString &styleSheet, const QMap<QString, QString> &variables);

    static QString formatColor(const QColor &color);
    static QColor parseColor(const QString &colorString);

    static QString getContrastColor(const QColor &backgroundColor);
    static bool isColorLight(const QColor &color);

    static QString blendColors(const QColor &color1, const QColor &color2, double ratio = 0.5);

    static QString getGlassmorphismStyle(Theme theme = Light, double opacity = 0.8);
    static QString getShadowStyle(Theme theme = Light);

    static QString getButtonStyle(Theme theme = Light);
    static QString getButtonHoverStyle(Theme theme = Light);
    static QString getButtonPressedStyle(Theme theme = Light);

    static QString getInputStyle(Theme theme = Light);
    static QString getInputFocusStyle(Theme theme = Light);

    static QString getCardStyle(Theme theme = Light);
    static QString getCardHoverStyle(Theme theme = Light);

    static QString getScrollbarStyle(Theme theme = Light);

private:
    ThemeUtils() = default;
    ~ThemeUtils() = default;

    static QMap<QString, QString> getLightColorVariables();
    static QMap<QString, QString> getDarkColorVariables();
};

#endif // THEME_UTILS_H
