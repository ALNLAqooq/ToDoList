#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QDateTime>
#include <QStringList>
#include <QList>

class Task;
class Tag;

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
    bool insertTask(Task &task);
    bool updateTask(const Task &task);
    bool deleteTask(int id);
    double calculateProgress(int taskId);

    QList<Tag> getAllTags();
    bool insertTag(Tag &tag);
    bool updateTag(const Tag &tag);
    bool deleteTag(int id);
    bool assignTagToTask(int taskId, int tagId);
    bool removeTagFromTask(int taskId, int tagId);
    bool addDependency(int taskId, int dependsOnId);
    bool removeDependency(int taskId, int dependsOnId);
    bool addFileToTask(int taskId, const QString &filePath, const QString &fileName);
    bool removeFileFromTask(int fileId);

private:
    Database();
    ~Database();
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    QSqlDatabase m_database;
    QString m_databasePath;
};

#endif // DATABASE_H
