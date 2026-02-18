#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QDateTime>
#include <QStringList>
#include <QList>

class Task;
class Tag;
class Notification;
class Folder;
class TaskStep;

class Database
{
public:
    static Database& instance();

    bool open();
    void close();

    bool createTables();
    bool createIndexes();
    bool createFTS5Table();

    QSqlDatabase& database();

    bool setSetting(const QString &key, const QString &value);
    QString getSetting(const QString &key, const QString &defaultValue = QString());

    void vacuum();

    QList<Task> getAllTasks();
    QList<Task> getTasksByParentId(int parentId);
    Task getTaskById(int id);
    QList<Task> getTaskHierarchy(int rootId = 0);
    bool insertTask(Task &task);
    bool updateTask(const Task &task);
    bool deleteTask(int id);
    double calculateProgress(int taskId);
    double calculateParentProgress(int taskId);

    QList<TaskStep> getTaskSteps(int taskId);
    bool insertTaskStep(TaskStep &step);
    bool updateTaskStep(const TaskStep &step);
    bool deleteTaskStep(int stepId);
    bool deleteTaskSteps(int taskId);

    QList<Tag> getAllTags();
    QList<Tag> getTagsByTaskId(int taskId);
    bool insertTag(Tag &tag);
    bool updateTag(const Tag &tag);
    bool deleteTag(int id);
    bool assignTagToTask(int taskId, int tagId);
    bool removeTagFromTask(int taskId, int tagId);
    bool removeAllTagsFromTask(int taskId);
    bool addDependency(int taskId, int dependsOnId);
    bool removeDependency(int taskId, int dependsOnId);
    bool removeAllDependenciesFromTask(int taskId);
    bool addFileToTask(int taskId, const QString &filePath, const QString &fileName);
    bool removeFileFromTask(int fileId);
    bool removeAllFilesFromTask(int taskId);

    QList<Notification> getAllNotifications();
    QList<Notification> getUnreadNotifications();
    QList<Notification> getNotificationsByType(int type);
    Notification getNotificationById(int id);
    bool insertNotification(Notification &notification);
    bool updateNotification(const Notification &notification);
    bool deleteNotification(int id);
    bool markNotificationAsRead(int id);
    bool markAllNotificationsAsRead();
    int getUnreadNotificationCount();

    QList<Folder> getAllFolders();
    Folder getFolderById(int id);
    bool insertFolder(Folder &folder);
    bool updateFolder(const Folder &folder);
    bool deleteFolder(int id);
    bool assignTaskToFolder(int taskId, int folderId);
    bool removeTaskFromFolder(int taskId, int folderId);
    QList<int> getTaskIdsByFolder(int folderId);

private:
    Database();
    ~Database();
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    QSqlDatabase m_database;
    QString m_databasePath;
};

#endif // DATABASE_H
