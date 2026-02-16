#include "theme_manager.h"
#include <QFile>
#include <QDebug>
#include <QApplication>
#include <QDir>
#include <QStyleFactory>
#include <QWidget>
#include <QStyle>

const QString ThemeManager::SETTINGS_KEY_THEME = "theme";
const QString ThemeManager::SETTINGS_KEY_FOLLOW_SYSTEM = "follow_system_theme";

ThemeManager& ThemeManager::instance()
{
    static ThemeManager instance;
    return instance;
}

ThemeManager::ThemeManager(QObject *parent)
    : QObject(parent)
    , m_currentTheme(ThemeManager::Dark)
    , m_followSystem(false)
    , m_settings(new QSettings("ToDoList", "AppSettings", this))
{
    loadThemeFromResources();
    loadThemePreference();
}

ThemeManager::~ThemeManager()
{
    saveThemePreference();
}

void ThemeManager::loadThemeFromResources()
{
    QFile darkFile(":/styles/dark.qss");
    if (darkFile.open(QIODevice::ReadOnly)) {
        m_darkStyleSheet = QString::fromUtf8(darkFile.readAll());
        darkFile.close();
    } else {
        qWarning() << "Failed to load dark.qss from resources";
    }

    QFile lightFile(":/styles/light.qss");
    if (lightFile.open(QIODevice::ReadOnly)) {
        m_lightStyleSheet = QString::fromUtf8(lightFile.readAll());
        lightFile.close();
    } else {
        qWarning() << "Failed to load light.qss from resources";
    }
}

void ThemeManager::loadThemePreference()
{
    int themeValue = m_settings->value(SETTINGS_KEY_THEME, static_cast<int>(ThemeManager::Dark)).toInt();
    m_followSystem = m_settings->value(SETTINGS_KEY_FOLLOW_SYSTEM, false).toBool();

    if (m_followSystem) {
        m_currentTheme = ThemeManager::System;
        ThemeManager::Theme detectedTheme = detectSystemTheme();
        qApp->setStyleSheet(detectedTheme == ThemeManager::Light ? m_lightStyleSheet : m_darkStyleSheet);
    } else {
        m_currentTheme = static_cast<ThemeManager::Theme>(themeValue);
        qApp->setStyleSheet(m_currentTheme == ThemeManager::Light ? m_lightStyleSheet : m_darkStyleSheet);
    }
}

void ThemeManager::saveThemePreference()
{
    m_settings->setValue(SETTINGS_KEY_THEME, static_cast<int>(m_currentTheme));
    m_settings->setValue(SETTINGS_KEY_FOLLOW_SYSTEM, m_followSystem);
    m_settings->sync();
}

ThemeManager::Theme ThemeManager::detectSystemTheme() const
{
#ifdef Q_OS_WIN
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                       QSettings::NativeFormat);
    return settings.value("AppsUseLightTheme", 1).toInt() == 1 ? ThemeManager::Light : ThemeManager::Dark;
#else
    return ThemeManager::Light;
#endif
}

void ThemeManager::setTheme(ThemeManager::Theme theme)
{
    if (m_currentTheme == theme) {
        return;
    }

    m_currentTheme = theme;

    QString styleSheet;
    if (theme == ThemeManager::System) {
        ThemeManager::Theme detectedTheme = detectSystemTheme();
        styleSheet = detectedTheme == ThemeManager::Light ? m_lightStyleSheet : m_darkStyleSheet;
    } else {
        styleSheet = theme == ThemeManager::Light ? m_lightStyleSheet : m_darkStyleSheet;
    }

    qApp->setStyleSheet(styleSheet);
    emit themeChanged(theme);
    saveThemePreference();
}

void ThemeManager::toggleTheme()
{
    if (m_currentTheme == ThemeManager::Light) {
        setTheme(ThemeManager::Dark);
    } else if (m_currentTheme == ThemeManager::Dark) {
        setTheme(ThemeManager::Light);
    } else {
        setTheme(detectSystemTheme() == ThemeManager::Light ? ThemeManager::Dark : ThemeManager::Light);
    }
}

QString ThemeManager::getStyleSheet() const
{
    if (m_currentTheme == ThemeManager::System) {
        ThemeManager::Theme detectedTheme = detectSystemTheme();
        return detectedTheme == ThemeManager::Light ? m_lightStyleSheet : m_darkStyleSheet;
    }
    return m_currentTheme == ThemeManager::Light ? m_lightStyleSheet : m_darkStyleSheet;
}

QColor ThemeManager::getPriorityColor(ThemeManager::Priority priority) const
{
    switch (priority) {
        case ThemeManager::High:
            return QColor("#EF4444");
        case ThemeManager::Medium:
            return QColor("#F59E0B");
        case ThemeManager::Low:
            return QColor("#10B981");
        default:
            return QColor("#64748B");
    }
}

void ThemeManager::setFollowSystem(bool follow)
{
    if (m_followSystem != follow) {
        m_followSystem = follow;
        if (follow) {
            m_currentTheme = ThemeManager::System;
            ThemeManager::Theme detectedTheme = detectSystemTheme();
            qApp->setStyleSheet(detectedTheme == ThemeManager::Light ? m_lightStyleSheet : m_darkStyleSheet);
        } else {
            ThemeManager::Theme newTheme = detectSystemTheme() == ThemeManager::Light ? ThemeManager::Light : ThemeManager::Dark;
            setTheme(newTheme);
        }
        emit themeChanged(m_currentTheme);
        saveThemePreference();
    }
}

QString ThemeManager::themeName(ThemeManager::Theme theme) const
{
    switch (theme) {
        case ThemeManager::Light:
            return "Light";
        case ThemeManager::Dark:
            return "Dark";
        case ThemeManager::System:
            return "System";
        default:
            return "Unknown";
    }
}

ThemeManager::Theme ThemeManager::themeFromName(const QString& name) const
{
    if (name.compare("Light", Qt::CaseInsensitive) == 0) {
        return ThemeManager::Light;
    } else if (name.compare("Dark", Qt::CaseInsensitive) == 0) {
        return ThemeManager::Dark;
    } else if (name.compare("System", Qt::CaseInsensitive) == 0) {
        return ThemeManager::System;
    }
    return ThemeManager::Dark;
}

void ThemeManager::applyTheme(QWidget* widget)
{
    if (widget) {
        widget->setStyleSheet(getStyleSheet());
        widget->style()->unpolish(widget);
        widget->style()->polish(widget);
        widget->update();
    }
}

void ThemeManager::onSystemThemeChanged()
{
    if (m_followSystem && m_currentTheme == ThemeManager::System) {
        ThemeManager::Theme detectedTheme = detectSystemTheme();
        qApp->setStyleSheet(detectedTheme == ThemeManager::Light ? m_lightStyleSheet : m_darkStyleSheet);
        emit themeChanged(m_currentTheme);
    }
}
