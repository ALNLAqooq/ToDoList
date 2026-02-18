#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include <QObject>
#include <QList>
#include "../models/notification.h"
#include "database.h"

class NotificationManager : public QObject
{
    Q_OBJECT

public:
    static NotificationManager& instance();

    bool addNotification(Notification::Type type, const QString &title, const QString &message, int taskId = 0);
    bool addNotification(const Notification &notification);

    QList<Notification> getAllNotifications();
    QList<Notification> getUnreadNotifications();
    QList<Notification> getNotificationsByType(Notification::Type type);
    QList<Notification> getRecentNotifications(int limit = 20);

    bool markAsRead(int notificationId);
    bool markAllAsRead();
    bool deleteNotification(int notificationId);
    bool clearAllNotifications();

    int unreadCount() const;

    void refresh();

signals:
    void notificationAdded(const Notification &notification);
    void notificationRead(int notificationId);
    void notificationDeleted(int notificationId);
    void unreadCountChanged(int count);
    void notificationsUpdated();

private:
    explicit NotificationManager(QObject *parent = nullptr);
    ~NotificationManager();

    void updateUnreadCount();

    NotificationManager(const NotificationManager&) = delete;
    NotificationManager& operator=(const NotificationManager&) = delete;

    Database &m_database;
    int m_unreadCount;
};

#endif // NOTIFICATIONMANAGER_H
