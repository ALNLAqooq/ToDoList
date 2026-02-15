#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <QSqlDatabase>
#include <QSqlError>
#include "../models/task.h"
#include "../models/tag.h"

class DatabaseManager
{
public:
    static DatabaseManager& instance();

    bool open();
    void close();

    QList<Task> getAllTasks();
    QList<Task> getTasksByParentId(int parentId);
    Task getTaskById(int id);
    bool insertTask(Task &task);
    bool updateTask(const Task &task);
    bool deleteTask(int id);

    QList<Tag> getAllTags();
    Tag getTagById(int id);
    bool insertTag(Tag &tag);
    bool updateTag(const Tag &tag);
    bool deleteTag(int id);
    bool assignTagToTask(int taskId, int tagId);
    bool removeTagFromTask(int taskId, int tagId);
    QList<int> getTagIdsForTask(int taskId);

    QList<int> getDependencyIdsForTask(int taskId);
    bool addDependency(int taskId, int dependsOnId);
    bool removeDependency(int taskId, int dependsOnId);

    QList<QString> getFilePathsForTask(int taskId);
    bool addFileToTask(int taskId, const QString &filePath, const QString &fileName);
    bool removeFileFromTask(int fileId);

    double calculateProgress(int taskId);

private:
    DatabaseManager();
    ~DatabaseManager();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    bool createTables();
    QSqlDatabase m_database;
};

#endif // DATABASE_MANAGER_H
