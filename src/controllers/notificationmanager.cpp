#include "notificationmanager.h"
#include "../utils/logger.h"
#include <QSqlQuery>
#include <QDateTime>

NotificationManager& NotificationManager::instance()
{
    static NotificationManager instance;
    return instance;
}

NotificationManager::NotificationManager(QObject *parent)
    : QObject(parent)
    , m_database(Database::instance())
    , m_unreadCount(0)
{
    refresh();
    LOG_INFO("NotificationManager", "NotificationManager initialized");
}

NotificationManager::~NotificationManager()
{
}

bool NotificationManager::addNotification(Notification::Type type, const QString &title, const QString &message, int taskId)
{
    Notification notification(0, type, title);
    notification.setMessage(message);
    notification.setTaskId(taskId);

    return addNotification(notification);
}

bool NotificationManager::addNotification(const Notification &notification)
{
    Notification notif = notification;

    if (m_database.insertNotification(notif)) {
        updateUnreadCount();
        emit notificationAdded(notif);
        emit notificationsUpdated();
        LOG_INFO("NotificationManager", QString("Notification added: %1").arg(notification.title()));
        return true;
    }

    LOG_ERROR("NotificationManager", QString("Failed to add notification: %1").arg(notification.title()));
    return false;
}

QList<Notification> NotificationManager::getAllNotifications()
{
    return m_database.getAllNotifications();
}

QList<Notification> NotificationManager::getUnreadNotifications()
{
    return m_database.getUnreadNotifications();
}

QList<Notification> NotificationManager::getNotificationsByType(Notification::Type type)
{
    return m_database.getNotificationsByType(static_cast<int>(type));
}

QList<Notification> NotificationManager::getRecentNotifications(int limit)
{
    QList<Notification> all = m_database.getAllNotifications();

    if (all.size() <= limit) {
        return all;
    }

    return all.mid(0, limit);
}

bool NotificationManager::markAsRead(int notificationId)
{
    if (m_database.markNotificationAsRead(notificationId)) {
        updateUnreadCount();
        emit notificationRead(notificationId);
        emit notificationsUpdated();
        LOG_INFO("NotificationManager", QString("Notification marked as read: %1").arg(notificationId));
        return true;
    }

    LOG_ERROR("NotificationManager", QString("Failed to mark notification as read: %1").arg(notificationId));
    return false;
}

bool NotificationManager::markAllAsRead()
{
    if (m_database.markAllNotificationsAsRead()) {
        updateUnreadCount();
        emit notificationsUpdated();
        LOG_INFO("NotificationManager", "All notifications marked as read");
        return true;
    }

    LOG_ERROR("NotificationManager", "Failed to mark all notifications as read");
    return false;
}

bool NotificationManager::deleteNotification(int notificationId)
{
    if (m_database.deleteNotification(notificationId)) {
        updateUnreadCount();
        emit notificationDeleted(notificationId);
        emit notificationsUpdated();
        LOG_INFO("NotificationManager", QString("Notification deleted: %1").arg(notificationId));
        return true;
    }

    LOG_ERROR("NotificationManager", QString("Failed to delete notification: %1").arg(notificationId));
    return false;
}

bool NotificationManager::clearAllNotifications()
{
    QList<Notification> all = m_database.getAllNotifications();
    bool success = true;

    for (const Notification &notif : all) {
        if (!m_database.deleteNotification(notif.id())) {
            success = false;
        }
    }

    if (success) {
        updateUnreadCount();
        emit notificationsUpdated();
        LOG_INFO("NotificationManager", "All notifications cleared");
        return true;
    }

    LOG_ERROR("NotificationManager", "Failed to clear all notifications");
    return false;
}

int NotificationManager::unreadCount() const
{
    return m_unreadCount;
}

void NotificationManager::refresh()
{
    updateUnreadCount();
    emit notificationsUpdated();
}

void NotificationManager::checkDeletionWarnings(int cleanupDays)
{
    if (cleanupDays <= 0) {
        return;
    }

    QSqlQuery query(m_database.database());
    query.prepare("SELECT id, title, deleted_at FROM tasks WHERE is_deleted = 1 AND deleted_at IS NOT NULL");
    if (!query.exec()) {
        return;
    }

    const QDateTime now = QDateTime::currentDateTime();
    while (query.next()) {
        int taskId = query.value(0).toInt();
        QString title = query.value(1).toString();
        QString deletedAtStr = query.value(2).toString();
        QDateTime deletedAt = QDateTime::fromString(deletedAtStr, Qt::ISODate);
        if (!deletedAt.isValid()) {
            continue;
        }

        int daysElapsed = deletedAt.daysTo(now);
        int remaining = cleanupDays - daysElapsed;
        if (remaining != 3 && remaining != 1) {
            continue;
        }

        QString message = QString("任务 \"%1\" 将在 %2 天后永久删除。").arg(title).arg(remaining);
        QSqlQuery existsQuery(m_database.database());
        existsQuery.prepare("SELECT 1 FROM notifications WHERE type = ? AND task_id = ? AND message = ? LIMIT 1");
        existsQuery.addBindValue(static_cast<int>(Notification::DeleteWarning));
        existsQuery.addBindValue(taskId);
        existsQuery.addBindValue(message);
        if (existsQuery.exec() && existsQuery.next()) {
            continue;
        }

        addNotification(Notification::DeleteWarning, "删除提醒", message, taskId);
    }
}

void NotificationManager::updateUnreadCount()
{
    int oldCount = m_unreadCount;
    m_unreadCount = m_database.getUnreadNotificationCount();

    if (oldCount != m_unreadCount) {
        emit unreadCountChanged(m_unreadCount);
    }
}
