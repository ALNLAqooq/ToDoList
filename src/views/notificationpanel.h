#ifndef NOTIFICATIONPANEL_H
#define NOTIFICATIONPANEL_H

#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QFrame>
#include <QTimer>
#include "../models/notification.h"
#include "../controllers/notificationmanager.h"

class NotificationItem : public QFrame
{
    Q_OBJECT

public:
    explicit NotificationItem(const Notification &notification, QWidget *parent = nullptr);

    int notificationId() const { return m_notificationId; }
    void setRead(bool read);

signals:
    void markAsReadClicked(int notificationId);
    void deleteClicked(int notificationId);

private slots:
    void onMarkAsRead();
    void onDelete();

private:
    void setupUI();
    QString getTypeIcon(Notification::Type type) const;
    QString getTypeColor(Notification::Type type) const;

    Notification::Type m_type;
    int m_notificationId;
    bool m_isRead;

    QLabel *m_typeIcon;
    QLabel *m_titleLabel;
    QLabel *m_messageLabel;
    QLabel *m_timeLabel;
    QPushButton *m_markReadButton;
    QPushButton *m_deleteButton;
};

class NotificationPanel : public QWidget
{
    Q_OBJECT

public:
    explicit NotificationPanel(QWidget *parent = nullptr);
    ~NotificationPanel();

    void refresh();

signals:
    void notificationCountChanged(int count);
    void closeRequested();

private slots:
    void onNotificationAdded(const Notification &notification);
    void onNotificationRead(int notificationId);
    void onNotificationDeleted(int notificationId);
    void onUnreadCountChanged(int count);
    void onMarkAllAsRead();
    void onClearAll();

private:
    void setupUI();
    void loadNotifications();
    void updateEmptyState();
    void updateHeader();

    QVBoxLayout *m_mainLayout;
    QScrollArea *m_scrollArea;
    QWidget *m_contentWidget;
    QVBoxLayout *m_contentLayout;

    QLabel *m_headerLabel;
    QPushButton *m_markAllButton;
    QPushButton *m_clearAllButton;
    QLabel *m_emptyLabel;

    NotificationManager &m_manager;
    QList<NotificationItem*> m_notificationItems;
};

#endif // NOTIFICATIONPANEL_H
