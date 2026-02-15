#include "task.h"

Task::Task()
    : m_id(0)
    , m_priority(Medium)
    , m_completed(false)
    , m_parentId(0)
    , m_progress(0.0)
{
}

Task::Task(int id, const QString &title)
    : m_id(id)
    , m_title(title)
    , m_priority(Medium)
    , m_completed(false)
    , m_parentId(0)
    , m_progress(0.0)
{
}

double Task::progress() const
{
    return m_progress;
}
