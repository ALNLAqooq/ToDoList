#include "app.h"
#include "controllers/database.h"
#include "utils/logger.h"
#include "utils/theme_manager.h"
#include "views/mainwindow.h"
#include "controllers/notificationmanager.h"
#include <QApplication>
#include <QSettings>
#include <QDateTime>
#include <QTimer>
#include <QMessageBox>
#include <QCoreApplication>

App::App(QObject *parent)
    : QObject(parent)
    , m_maintenanceTimer(new QTimer(this))
{
    m_maintenanceTimer->setInterval(6 * 60 * 60 * 1000);
    connect(m_maintenanceTimer, &QTimer::timeout, this, &App::runMaintenance);
}

App::~App()
{
}

void App::init()
{
    initLogger();
    if (!initDatabase()) {
        return;
    }
    initSettings();
    initTheme();
    initWindow();
    runMaintenance();
    scheduleMaintenance();

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

bool App::initDatabase()
{
    Database &db = Database::instance();

    if (!db.open()) {
        QString message;
        if (db.isCorrupted()) {
            message = "数据库可能已损坏，无法打开。\n请尝试从备份恢复或删除数据库后重新启动。";
        } else {
            message = "无法打开数据库。\n请检查磁盘权限或路径是否可用。";
        }
        if (!db.lastError().isEmpty()) {
            message += "\n\n详情: " + db.lastError();
        }
        QMessageBox::critical(nullptr, "数据库错误", message);
        LOG_CRITICAL("App", "Failed to open database");
        QCoreApplication::exit(1);
        return false;
    }

    LOG_INFO("Database", "Database initialized successfully");
    return true;
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

void App::runMaintenance()
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

    int notificationRetentionDays = db.getSetting("notifications_cleanup_days", "30").toInt();
    if (notificationRetentionDays > 0) {
        int removedNotifications = db.cleanupOldNotifications(notificationRetentionDays);
        if (removedNotifications > 0) {
            LOG_INFO("App", QString("Auto-cleaned %1 old notifications").arg(removedNotifications));
        }
    }

    const QDateTime now = QDateTime::currentDateTime();
    const QString lastVacuumStr = db.getSetting("db_last_vacuum", "");
    const QDateTime lastVacuum = QDateTime::fromString(lastVacuumStr, Qt::ISODate);
    if (!lastVacuum.isValid() || lastVacuum.daysTo(now) >= 7) {
        db.vacuum();
        db.setSetting("db_last_vacuum", now.toString(Qt::ISODate));
        LOG_INFO("App", "Database vacuum completed");
    }
}

void App::scheduleMaintenance()
{
    if (!m_maintenanceTimer->isActive()) {
        m_maintenanceTimer->start();
    }
}
