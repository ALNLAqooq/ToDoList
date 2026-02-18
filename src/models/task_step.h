#ifndef TASK_STEP_H
#define TASK_STEP_H

#include <QString>

class TaskStep
{
public:
    TaskStep();
    TaskStep(int id, int taskId, const QString &title);

    int id() const { return m_id; }
    void setId(int id) { m_id = id; }

    int taskId() const { return m_taskId; }
    void setTaskId(int taskId) { m_taskId = taskId; }

    QString title() const { return m_title; }
    void setTitle(const QString &title) { m_title = title; }

    bool isCompleted() const { return m_completed; }
    void setCompleted(bool completed) { m_completed = completed; }

    int position() const { return m_position; }
    void setPosition(int position) { m_position = position; }

private:
    int m_id;
    int m_taskId;
    QString m_title;
    bool m_completed;
    int m_position;
};

#endif // TASK_STEP_H
