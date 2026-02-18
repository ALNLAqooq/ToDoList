#include "notificationpanel.h"
#include "../utils/logger.h"
#include <QDateTime>

NotificationItem::NotificationItem(const Notification &notification, QWidget *parent)
    : QFrame(parent)
    , m_type(notification.type())
    , m_notificationId(notification.id())
    , m_isRead(notification.isRead())
{
    setupUI();

    m_typeIcon->setText(getTypeIcon(notification.type()));
    m_typeIcon->setStyleSheet(QString("QLabel#notificationTypeIcon { background: %1; }")
                                .arg(getTypeColor(notification.type())));
    m_titleLabel->setText(notification.title());
    m_messageLabel->setText(notification.message());
    m_timeLabel->setText(notification.createdAt().toString("yyyy-MM-dd hh:mm"));

    if (m_isRead) {
        setRead(true);
    }
}

void NotificationItem::setupUI()
{
    setObjectName("notificationItem");
    setMinimumHeight(88);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    setContentsMargins(0, 0, 0, 0);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 12, 16, 12);
    mainLayout->setSpacing(8);

    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->setSpacing(12);

    m_typeIcon = new QLabel(this);
    m_typeIcon->setFixedSize(32, 32);
    m_typeIcon->setAlignment(Qt::AlignCenter);
    m_typeIcon->setObjectName("notificationTypeIcon");
    topLayout->addWidget(m_typeIcon);

    QVBoxLayout *textLayout = new QVBoxLayout();
    textLayout->setSpacing(4);

    m_titleLabel = new QLabel(this);
    m_titleLabel->setObjectName("notificationTitle");
    textLayout->addWidget(m_titleLabel);

    m_messageLabel = new QLabel(this);
    m_messageLabel->setObjectName("notificationMessage");
    m_messageLabel->setWordWrap(true);
    m_messageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    textLayout->addWidget(m_messageLabel);

    topLayout->addLayout(textLayout, 1);

    m_timeLabel = new QLabel(this);
    m_timeLabel->setObjectName("notificationTime");
    m_timeLabel->setAlignment(Qt::AlignRight);
    topLayout->addWidget(m_timeLabel);

    mainLayout->addLayout(topLayout);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 0, 0, 0);

    buttonLayout->addStretch();

    m_markReadButton = new QPushButton(this);
    m_markReadButton->setText("标记为已读");
    m_markReadButton->setObjectName("notificationMarkRead");
    connect(m_markReadButton, &QPushButton::clicked, this, &NotificationItem::onMarkAsRead);
    buttonLayout->addWidget(m_markReadButton);

    m_deleteButton = new QPushButton(this);
    m_deleteButton->setText("删除");
    m_deleteButton->setObjectName("notificationDelete");
    connect(m_deleteButton, &QPushButton::clicked, this, &NotificationItem::onDelete);
    buttonLayout->addWidget(m_deleteButton);

    mainLayout->addLayout(buttonLayout);
}

QString NotificationItem::getTypeIcon(Notification::Type type) const
{
    switch (type) {
        case Notification::DeleteWarning:
            return "⚠️";
        case Notification::Deadline:
            return "⏰";
        case Notification::Backup:
            return "💾";
        case Notification::System:
        default:
            return "ℹ️";
    }
}

QString NotificationItem::getTypeColor(Notification::Type type) const
{
    switch (type) {
        case Notification::DeleteWarning:
            return "#F59E0B";
        case Notification::Deadline:
            return "#EF4444";
        case Notification::Backup:
            return "#10B981";
        case Notification::System:
        default:
            return "#3B82F6";
    }
}

void NotificationItem::setRead(bool read)
{
    m_isRead = read;
    if (read) {
        m_markReadButton->setEnabled(false);
    }
}

void NotificationItem::onMarkAsRead()
{
    emit markAsReadClicked(m_notificationId);
}

void NotificationItem::onDelete()
{
    emit deleteClicked(m_notificationId);
}

NotificationPanel::NotificationPanel(QWidget *parent)
    : QWidget(parent)
    , m_manager(NotificationManager::instance())
{
    setupUI();

    connect(&m_manager, &NotificationManager::notificationAdded, this, &NotificationPanel::onNotificationAdded);
    connect(&m_manager, &NotificationManager::notificationRead, this, &NotificationPanel::onNotificationRead);
    connect(&m_manager, &NotificationManager::notificationDeleted, this, &NotificationPanel::onNotificationDeleted);
    connect(&m_manager, &NotificationManager::unreadCountChanged, this, &NotificationPanel::onUnreadCountChanged);
    connect(&m_manager, &NotificationManager::notificationsUpdated, this, [this]() { loadNotifications(); });

    loadNotifications();
    updateEmptyState();
    updateHeader();
}

NotificationPanel::~NotificationPanel()
{
}

void NotificationPanel::setupUI()
{
    setFixedWidth(460);
    setMinimumHeight(520);
    setMaximumHeight(720);
    setObjectName("notificationPanel");

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    QWidget *headerWidget = new QWidget(this);
    headerWidget->setFixedHeight(60);
    headerWidget->setObjectName("notificationHeader");

    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(16, 0, 16, 0);

    m_headerLabel = new QLabel(this);
    m_headerLabel->setObjectName("notificationHeaderLabel");
    headerLayout->addWidget(m_headerLabel);

    headerLayout->addStretch();

    m_markAllButton = new QPushButton(this);
    m_markAllButton->setText("全部标记");
    m_markAllButton->setObjectName("notificationMarkAll");
    connect(m_markAllButton, &QPushButton::clicked, this, &NotificationPanel::onMarkAllAsRead);
    headerLayout->addWidget(m_markAllButton);

    m_clearAllButton = new QPushButton(this);
    m_clearAllButton->setText("清空");
    m_clearAllButton->setObjectName("notificationClearAll");
    connect(m_clearAllButton, &QPushButton::clicked, this, &NotificationPanel::onClearAll);
    headerLayout->addWidget(m_clearAllButton);

    m_mainLayout->addWidget(headerWidget);

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setObjectName("notificationScroll");

    m_contentWidget = new QWidget(this);
    m_contentLayout = new QVBoxLayout(m_contentWidget);
    m_contentLayout->setContentsMargins(12, 12, 12, 12);
    m_contentLayout->setSpacing(8);

    m_emptyLabel = new QLabel(this);
    m_emptyLabel->setText("暂无通知");
    m_emptyLabel->setAlignment(Qt::AlignCenter);
    m_emptyLabel->setObjectName("notificationEmpty");
    m_contentLayout->addWidget(m_emptyLabel);

    m_scrollArea->setWidget(m_contentWidget);
    m_mainLayout->addWidget(m_scrollArea);

    setLayout(m_mainLayout);
}

void NotificationPanel::refresh()
{
    loadNotifications();
    updateEmptyState();
    updateHeader();
}

void NotificationPanel::loadNotifications()
{
    m_notificationItems.clear();

    while (QLayoutItem *item = m_contentLayout->takeAt(0)) {
        QWidget *widget = item->widget();
        if (widget && widget != m_emptyLabel) {
            widget->deleteLater();
        }
        delete item;
    }

    QList<Notification> notifications = m_manager.getAllNotifications();

    m_contentLayout->addWidget(m_emptyLabel);

    for (const Notification &notif : notifications) {
        NotificationItem *item = new NotificationItem(notif, this);
        connect(item, &NotificationItem::markAsReadClicked, this, [this](int id) {
            m_manager.markAsRead(id);
        });
        connect(item, &NotificationItem::deleteClicked, this, [this](int id) {
            m_manager.deleteNotification(id);
        });
        m_contentLayout->addWidget(item);
        m_notificationItems.append(item);
    }

    m_contentLayout->addStretch();
    updateEmptyState();
    updateHeader();
}

void NotificationPanel::updateEmptyState()
{
    m_emptyLabel->setVisible(m_notificationItems.isEmpty());
}

void NotificationPanel::updateHeader()
{
    int unread = m_manager.unreadCount();
    int total = m_manager.getAllNotifications().size();

    if (unread > 0) {
        m_headerLabel->setText(QString("通知 (%1)").arg(unread));
    } else {
        m_headerLabel->setText(QString("通知"));
    }

    m_markAllButton->setEnabled(unread > 0);
    m_clearAllButton->setEnabled(total > 0);

    emit notificationCountChanged(unread);
}

void NotificationPanel::onNotificationAdded(const Notification &notification)
{
    loadNotifications();
    updateHeader();
    LOG_INFO("NotificationPanel", QString("Notification added to panel: %1").arg(notification.title()));
}

void NotificationPanel::onNotificationRead(int notificationId)
{
    for (NotificationItem *item : m_notificationItems) {
        if (item->notificationId() == notificationId) {
            item->setRead(true);
            break;
        }
    }
    updateHeader();
    LOG_INFO("NotificationPanel", QString("Notification marked as read: %1").arg(notificationId));
}

void NotificationPanel::onNotificationDeleted(int notificationId)
{
    for (NotificationItem *item : m_notificationItems) {
        if (item->notificationId() == notificationId) {
            m_contentLayout->removeWidget(item);
            m_notificationItems.removeOne(item);
            delete item;
            break;
        }
    }
    updateEmptyState();
    updateHeader();
    LOG_INFO("NotificationPanel", QString("Notification deleted from panel: %1").arg(notificationId));
}

void NotificationPanel::onUnreadCountChanged(int count)
{
    updateHeader();
    emit notificationCountChanged(count);
}

void NotificationPanel::onMarkAllAsRead()
{
    m_manager.markAllAsRead();
    for (NotificationItem *item : m_notificationItems) {
        item->setRead(true);
    }
    LOG_INFO("NotificationPanel", "All notifications marked as read");
}

void NotificationPanel::onClearAll()
{
    m_manager.clearAllNotifications();
    qDeleteAll(m_notificationItems);
    m_notificationItems.clear();
    updateEmptyState();
    updateHeader();
    LOG_INFO("NotificationPanel", "All notifications cleared");
}
