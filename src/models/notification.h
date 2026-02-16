#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QString>
#include <QDateTime>

class Notification
{
public:
    enum Type {
        DeleteWarning,
        Deadline,
        Backup,
        System
    };

    Notification();
    Notification(int id, Type type, const QString &title);

    int id() const { return m_id; }
    void setId(int id) { m_id = id; }

    Type type() const { return m_type; }
    void setType(Type type) { m_type = type; }
    void setType(int type) { m_type = static_cast<Type>(type); }

    QString title() const { return m_title; }
    void setTitle(const QString &title) { m_title = title; }

    QString message() const { return m_message; }
    void setMessage(const QString &message) { m_message = message; }

    int taskId() const { return m_taskId; }
    void setTaskId(int taskId) { m_taskId = taskId; }

    bool isRead() const { return m_read; }
    void setRead(bool read) { m_read = read; }

    QDateTime createdAt() const { return m_createdAt; }
    void setCreatedAt(const QDateTime &createdAt) { m_createdAt = createdAt; }

    QString typeString() const;
    QString typeDisplayName() const;

private:
    int m_id;
    Type m_type;
    QString m_title;
    QString m_message;
    int m_taskId;
    bool m_read;
    QDateTime m_createdAt;
};

#endif // NOTIFICATION_H
