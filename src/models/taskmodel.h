#ifndef TASKMODEL_H
#define TASKMODEL_H

#include <QStandardItemModel>
#include <QHash>
#include <QSet>

class Task;
class Database;

enum TaskModelRoles {
    IdRole = Qt::UserRole + 1,
    TitleRole,
    DescriptionRole,
    PriorityRole,
    DueDateRole,
    CompletedRole,
    ProgressRole,
    ParentIdRole,
    CreatedAtRole,
    UpdatedAtRole,
    TagIdsRole,
    DependencyIdsRole,
    FilePathsRole
};

class TaskModel : public QStandardItemModel
{
    Q_OBJECT

public:
    explicit TaskModel(QObject *parent = nullptr);
    ~TaskModel();

    QHash<int, QByteArray> roleNames() const override;

    void loadTasks();
    void loadTasksByParentId(int parentId);
    void loadTaskHierarchy();
    void reload();

    bool addTask(const Task &task);
    bool updateTask(const Task &task);
    bool deleteTask(int taskId);
    bool setTaskCompleted(int taskId, bool completed);
    bool setTaskProgress(int taskId, double progress);

    Task getTaskById(int taskId) const;
    QStandardItem* getTaskItem(int taskId) const;
    QModelIndex getTaskIndex(int taskId) const;

    void setFilter(const QString &filter);
    void setFilterByPriority(int priority);
    void setFilterByCompleted(bool completed);
    void clearFilters();

    void sortByPriority(Qt::SortOrder order = Qt::DescendingOrder);
    void sortByDueDate(Qt::SortOrder order = Qt::AscendingOrder);
    void sortByCreatedDate(Qt::SortOrder order = Qt::DescendingOrder);

signals:
    void taskAdded(int taskId);
    void taskUpdated(int taskId);
    void taskDeleted(int taskId);
    void taskCompletedChanged(int taskId, bool completed);
    void taskProgressChanged(int taskId, double progress);
    void modelLoaded();

private:
    QStandardItem* createTaskItem(const Task &task);
    void addChildTasks(int parentId, QStandardItem *parentItem);
    void buildTaskHierarchy(int parentId, QStandardItem *parentItem, const QHash<int, QList<Task>> &tasksByParent);
    void applyFilters();
    bool matchesFilters(const Task &task) const;

    Database *m_database;
    QString m_textFilter;
    int m_priorityFilter;
    int m_completedFilter;
    QHash<int, QStandardItem*> m_taskItems;
    QSet<int> m_expandedTasks;
};

#endif // TASKMODEL_H
