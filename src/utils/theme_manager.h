#ifndef THEME_MANAGER_H
#define THEME_MANAGER_H

#include <QObject>
#include <QApplication>

class ThemeManager : public QObject
{
    Q_OBJECT

public:
    enum Theme {
        Light,
        Dark
    };

    static ThemeManager& instance();

    Theme currentTheme() const { return m_currentTheme; }
    void setTheme(Theme theme);
    void toggleTheme();

    QString getStyleSheet() const;

signals:
    void themeChanged(Theme theme);

private:
    ThemeManager(QObject *parent = nullptr);
    ~ThemeManager();
    ThemeManager(const ThemeManager&) = delete;
    ThemeManager& operator=(const ThemeManager&) = delete;

    void loadLightTheme();
    void loadDarkTheme();

    Theme m_currentTheme;
    QString m_lightStyleSheet;
    QString m_darkStyleSheet;
};

#endif // THEME_MANAGER_H
