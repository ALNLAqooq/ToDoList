#include "app.h"
#include "controllers/database.h"
#include "utils/logger.h"
#include "utils/theme_manager.h"
#include "views/mainwindow.h"
#include "controllers/notificationmanager.h"
#include <QApplication>
#include <QSettings>

App::App(QObject *parent)
    : QObject(parent)
{
}

App::~App()
{
}

void App::init()
{
    initLogger();
    initDatabase();
    initSettings();
    initTheme();
    initWindow();
    runDeleteMaintenance();

    LOG_INFO("App", "Application initialized successfully");
}

void App::initLogger()
{
    Logger &logger = Logger::instance();

    QSettings settings;
    QString minLevelStr = settings.value("log_level", "INFO").toString();
    Logger::Level minLevel = Logger::stringToLevel(minLevelStr);
    logger.setMinLevel(minLevel);

    LOG_INFO("Logger", QString("Logger initialized with level: %1").arg(Logger::levelToString(minLevel)));
}

void App::initDatabase()
{
    Database &db = Database::instance();

    if (!db.open()) {
        LOG_CRITICAL("App", "Failed to open database");
        return;
    }

    LOG_INFO("Database", "Database initialized successfully");
}

void App::initSettings()
{
    QSettings settings;

    if (!settings.contains("theme")) {
        settings.setValue("theme", "dark");
    }

    if (!settings.contains("window_width")) {
        settings.setValue("window_width", 1280);
    }

    if (!settings.contains("window_height")) {
        settings.setValue("window_height", 720);
    }

    LOG_INFO("Settings", "Settings initialized");
}

void App::initTheme()
{
    ThemeManager &manager = ThemeManager::instance();
    manager.applyTheme();

    LOG_INFO("Theme", QString("Theme initialized: %1").arg(manager.themeName(manager.currentTheme())));
}

void App::initWindow()
{
    MainWindow *window = new MainWindow();
    window->show();

    LOG_INFO("Window", "Main window created and shown");
}

void App::runDeleteMaintenance()
{
    Database &db = Database::instance();
    int cleanupDays = db.getSetting("delete_cleanup_days", "14").toInt();
    bool autoCleanup = db.getSetting("delete_auto_cleanup", "1") == "1";

    if (autoCleanup) {
        NotificationManager::instance().checkDeletionWarnings(cleanupDays);
        int removed = db.cleanupDeletedTasks(cleanupDays);
        if (removed > 0) {
            LOG_INFO("App", QString("Auto-cleaned %1 deleted tasks").arg(removed));
        }
    }
}
