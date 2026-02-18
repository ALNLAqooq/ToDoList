#ifndef TASK_H
#define TASK_H

#include <QString>
#include <QDateTime>
#include <QList>

class Task
{
public:
    enum Priority {
        Low = 1,
        Medium = 2,
        High = 3
    };

    Task();
    Task(int id, const QString &title);

    int id() const { return m_id; }
    void setId(int id) { m_id = id; }

    QString title() const { return m_title; }
    void setTitle(const QString &title) { m_title = title; }

    QString description() const { return m_description; }
    void setDescription(const QString &description) { m_description = description; }

    Priority priority() const { return m_priority; }
    void setPriority(Priority priority) { m_priority = priority; }
    void setPriority(int priority) { m_priority = static_cast<Priority>(priority); }

    QDateTime dueDate() const { return m_dueDate; }
    void setDueDate(const QDateTime &dueDate) { m_dueDate = dueDate; }

    bool isCompleted() const { return m_completed; }
    void setCompleted(bool completed) { m_completed = completed; }

    int parentId() const { return m_parentId; }
    void setParentId(int parentId) { m_parentId = parentId; }

    bool hasChildren() const { return m_hasChildren; }
    void setHasChildren(bool hasChildren) { m_hasChildren = hasChildren; }

    QDateTime createdAt() const { return m_createdAt; }
    void setCreatedAt(const QDateTime &createdAt) { m_createdAt = createdAt; }

    QDateTime updatedAt() const { return m_updatedAt; }
    void setUpdatedAt(const QDateTime &updatedAt) { m_updatedAt = updatedAt; }

    QList<int> tagIds() const { return m_tagIds; }
    void setTagIds(const QList<int> &tagIds) { m_tagIds = tagIds; }
    void addTagId(int tagId) { m_tagIds.append(tagId); }

    QList<int> dependencyIds() const { return m_dependencyIds; }
    void setDependencyIds(const QList<int> &dependencyIds) { m_dependencyIds = dependencyIds; }
    void addDependencyId(int dependencyId) { m_dependencyIds.append(dependencyId); }

    QList<QString> filePaths() const { return m_filePaths; }
    void setFilePaths(const QList<QString> &filePaths) { m_filePaths = filePaths; }
    void addFilePath(const QString &filePath) { m_filePaths.append(filePath); }

    double progress() const;
    void setProgress(double progress) { m_progress = progress; }

private:
    int m_id;
    QString m_title;
    QString m_description;
    Priority m_priority;
    QDateTime m_dueDate;
    bool m_completed;
    int m_parentId;
    bool m_hasChildren;
    QDateTime m_createdAt;
    QDateTime m_updatedAt;
    QList<int> m_tagIds;
    QList<int> m_dependencyIds;
    QList<QString> m_filePaths;
    double m_progress;
};

#endif // TASK_H
