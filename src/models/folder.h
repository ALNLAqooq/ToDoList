#ifndef FOLDER_H
#define FOLDER_H

#include <QString>
#include <QDateTime>
#include <QList>

class Folder
{
public:
    Folder();
    Folder(int id, const QString &name);

    int id() const { return m_id; }
    void setId(int id) { m_id = id; }

    QString name() const { return m_name; }
    void setName(const QString &name) { m_name = name; }

    QString color() const { return m_color; }
    void setColor(const QString &color) { m_color = color; }

    int position() const { return m_position; }
    void setPosition(int position) { m_position = position; }

    QDateTime createdAt() const { return m_createdAt; }
    void setCreatedAt(const QDateTime &createdAt) { m_createdAt = createdAt; }

    QList<int> taskIds() const { return m_taskIds; }
    void setTaskIds(const QList<int> &taskIds) { m_taskIds = taskIds; }
    void addTaskId(int taskId) { m_taskIds.append(taskId); }
    void removeTaskId(int taskId) { m_taskIds.removeAll(taskId); }

    int taskCount() const { return m_taskIds.count(); }

private:
    int m_id;
    QString m_name;
    QString m_color;
    int m_position;
    QDateTime m_createdAt;
    QList<int> m_taskIds;
};

#endif // FOLDER_H
