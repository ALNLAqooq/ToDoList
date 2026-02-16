#include "app.h"
#include "controllers/database.h"
#include "utils/logger.h"
#include "views/mainwindow.h"
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
    QSettings settings;
    QString theme = settings.value("theme", "dark").toString();

    LOG_INFO("Theme", QString("Theme set to: %1").arg(theme));
}

void App::initWindow()
{
    MainWindow *window = new MainWindow();
    window->show();

    LOG_INFO("Window", "Main window created and shown");
}
