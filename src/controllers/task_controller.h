#ifndef TASK_CONTROLLER_H
#define TASK_CONTROLLER_H

#include <QObject>
#include "../models/task.h"
#include "../models/tag.h"

class TaskController : public QObject
{
    Q_OBJECT

public:
    explicit TaskController(QObject *parent = nullptr);
    ~TaskController();

    QList<Task> getAllTasks();
    QList<Task> getSubTasks(int parentId);
    Task getTaskById(int id);
    Task getTaskByIdIncludingDeleted(int id);
    QList<Task> getTaskHierarchy(int rootId = 0);

    bool addTask(Task &task);
    bool updateTask(const Task &task);
    bool deleteTask(int id);
    bool restoreTask(int id);
    bool permanentlyDeleteTask(int id);
    bool toggleTaskCompletion(int id);

    QList<Tag> getAllTags();
    QList<Tag> getTagsByTaskId(int taskId);
    bool addTag(Tag &tag);
    bool updateTag(const Tag &tag);
    bool deleteTag(int id);
    bool assignTagToTask(int taskId, int tagId);
    bool removeTagFromTask(int taskId, int tagId);

    bool addDependency(int taskId, int dependsOnId);
    bool removeDependency(int taskId, int dependsOnId);
    QList<int> getDependencyIdsForTask(int taskId);
    QList<Task> getDependenciesForTask(int taskId);
    bool wouldCreateCircularDependency(int taskId, int dependsOnId);
    QList<Task> getCircularDependencies(int taskId);

    bool addFileToTask(int taskId, const QString &filePath);
    bool removeFileFromTask(int fileId);

    double updateProgress(int taskId);
    double updateParentProgress(int taskId);

signals:
    void taskAdded(const Task &task);
    void taskUpdated(const Task &task);
    void taskDeleted(int taskId);
    void taskCompletionChanged(int taskId, bool completed);
    void tagsChanged();
    void dependenciesChanged();
    void filesChanged();
};

#endif // TASK_CONTROLLER_H
