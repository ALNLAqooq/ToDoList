#include "notification.h"

Notification::Notification()
    : m_id(0)
    , m_type(System)
    , m_taskId(0)
    , m_read(false)
{
}

Notification::Notification(int id, Type type, const QString &title)
    : m_id(id)
    , m_type(type)
    , m_title(title)
    , m_taskId(0)
    , m_read(false)
{
}

QString Notification::typeString() const
{
    switch (m_type) {
        case DeleteWarning:
            return "delete_warning";
        case Deadline:
            return "deadline";
        case Backup:
            return "backup";
        case System:
        default:
            return "system";
    }
}

QString Notification::typeDisplayName() const
{
    switch (m_type) {
        case DeleteWarning:
            return "Delete Warning";
        case Deadline:
            return "Deadline";
        case Backup:
            return "Backup";
        case System:
        default:
            return "System";
    }
}
