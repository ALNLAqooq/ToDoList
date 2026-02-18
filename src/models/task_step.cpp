#include "task_step.h"

TaskStep::TaskStep()
    : m_id(0)
    , m_taskId(0)
    , m_title("")
    , m_completed(false)
    , m_position(0)
{
}

TaskStep::TaskStep(int id, int taskId, const QString &title)
    : m_id(id)
    , m_taskId(taskId)
    , m_title(title)
    , m_completed(false)
    , m_position(0)
{
}
