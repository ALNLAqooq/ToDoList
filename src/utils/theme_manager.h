#ifndef THEME_MANAGER_H
#define THEME_MANAGER_H

#include <QObject>
#include <QApplication>
#include <QSettings>
#include <QColor>

class ThemeManager : public QObject
{
    Q_OBJECT

public:
    enum Theme {
        Light,
        Dark,
        System
    };

    enum Priority {
        High,
        Medium,
        Low
    };

    static ThemeManager& instance();

    Theme currentTheme() const { return m_currentTheme; }
    void setTheme(Theme theme);
    void toggleTheme();

    QString getStyleSheet() const;
    QColor getPriorityColor(Priority priority) const;

    bool followSystem() const { return m_followSystem; }
    void setFollowSystem(bool follow);

    QString themeName(Theme theme) const;
    Theme themeFromName(const QString& name) const;

    void applyTheme(QWidget* widget = nullptr);

signals:
    void themeChanged(Theme theme);

private slots:
    void onSystemThemeChanged();

private:
    ThemeManager(QObject *parent = nullptr);
    ~ThemeManager();
    ThemeManager(const ThemeManager&) = delete;
    ThemeManager& operator=(const ThemeManager&) = delete;

    void loadThemeFromResources();
    void saveThemePreference();
    void loadThemePreference();
    Theme detectSystemTheme() const;

    Theme m_currentTheme;
    bool m_followSystem;
    QString m_lightStyleSheet;
    QString m_darkStyleSheet;

    QSettings* m_settings;
    static const QString SETTINGS_KEY_THEME;
    static const QString SETTINGS_KEY_FOLLOW_SYSTEM;
};

#endif // THEME_MANAGER_H
