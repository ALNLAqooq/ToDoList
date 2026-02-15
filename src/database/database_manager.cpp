#include "database_manager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QDir>

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::DatabaseManager()
{
}

DatabaseManager::~DatabaseManager()
{
    if (m_database.isOpen()) {
        m_database.close();
    }
}

bool DatabaseManager::open()
{
    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName("todolist.db");

    if (!m_database.open()) {
        qDebug() << "Database error:" << m_database.lastError().text();
        return false;
    }

    return createTables();
}

void DatabaseManager::close()
{
    if (m_database.isOpen()) {
        m_database.close();
    }
}

bool DatabaseManager::createTables()
{
    QSqlQuery query(m_database);

    QString tasksTable = R"(
        CREATE TABLE IF NOT EXISTS tasks (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            title TEXT NOT NULL,
            description TEXT,
            priority INTEGER DEFAULT 1,
            due_date TEXT,
            completed INTEGER DEFAULT 0,
            parent_id INTEGER,
            progress REAL DEFAULT 0.0,
            created_at TEXT DEFAULT CURRENT_TIMESTAMP,
            updated_at TEXT DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (parent_id) REFERENCES tasks(id)
        )
    )";

    QString tagsTable = R"(
        CREATE TABLE IF NOT EXISTS tags (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT UNIQUE NOT NULL,
            color TEXT DEFAULT '#3B82F6'
        )
    )";

    QString taskTagsTable = R"(
        CREATE TABLE IF NOT EXISTS task_tags (
            task_id INTEGER,
            tag_id INTEGER,
            PRIMARY KEY (task_id, tag_id),
            FOREIGN KEY (task_id) REFERENCES tasks(id) ON DELETE CASCADE,
            FOREIGN KEY (tag_id) REFERENCES tags(id) ON DELETE CASCADE
        )
    )";

    QString taskDependenciesTable = R"(
        CREATE TABLE IF NOT EXISTS task_dependencies (
            task_id INTEGER,
            depends_on_id INTEGER,
            PRIMARY KEY (task_id, depends_on_id),
            FOREIGN KEY (task_id) REFERENCES tasks(id) ON DELETE CASCADE,
            FOREIGN KEY (depends_on_id) REFERENCES tasks(id) ON DELETE CASCADE
        )
    )";

    QString taskFilesTable = R"(
        CREATE TABLE IF NOT EXISTS task_files (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            task_id INTEGER NOT NULL,
            file_path TEXT NOT NULL,
            file_name TEXT NOT NULL,
            FOREIGN KEY (task_id) REFERENCES tasks(id) ON DELETE CASCADE
        )
    )";

    if (!query.exec(tasksTable)) {
        qDebug() << "Failed to create tasks table:" << query.lastError().text();
        return false;
    }

    if (!query.exec(tagsTable)) {
        qDebug() << "Failed to create tags table:" << query.lastError().text();
        return false;
    }

    if (!query.exec(taskTagsTable)) {
        qDebug() << "Failed to create task_tags table:" << query.lastError().text();
        return false;
    }

    if (!query.exec(taskDependenciesTable)) {
        qDebug() << "Failed to create task_dependencies table:" << query.lastError().text();
        return false;
    }

    if (!query.exec(taskFilesTable)) {
        qDebug() << "Failed to create task_files table:" << query.lastError().text();
        return false;
    }

    return true;
}

QList<Task> DatabaseManager::getAllTasks()
{
    QList<Task> tasks;
    QSqlQuery query(m_database);

    query.exec("SELECT id, title, description, priority, due_date, completed, parent_id, progress, created_at, updated_at FROM tasks ORDER BY priority DESC, created_at DESC");

    while (query.next()) {
        Task task;
        task.setId(query.value(0).toInt());
        task.setTitle(query.value(1).toString());
        task.setDescription(query.value(2).toString());
        task.setPriority(query.value(3).toInt());
        task.setDueDate(QDateTime::fromString(query.value(4).toString(), Qt::ISODate));
        task.setCompleted(query.value(5).toBool());
        task.setParentId(query.value(6).toInt());
        task.setProgress(query.value(7).toDouble());
        task.setCreatedAt(QDateTime::fromString(query.value(8).toString(), Qt::ISODate));
        task.setUpdatedAt(QDateTime::fromString(query.value(9).toString(), Qt::ISODate));

        task.setTagIds(getTagIdsForTask(task.id()));
        task.setDependencyIds(getDependencyIdsForTask(task.id()));
        task.setFilePaths(getFilePathsForTask(task.id()));

        tasks.append(task);
    }

    return tasks;
}

QList<Task> DatabaseManager::getTasksByParentId(int parentId)
{
    QList<Task> tasks;
    QSqlQuery query(m_database);

    query.prepare("SELECT id, title, description, priority, due_date, completed, parent_id, progress, created_at, updated_at FROM tasks WHERE parent_id = ? ORDER BY priority DESC, created_at DESC");
    query.addBindValue(parentId);

    if (!query.exec()) {
        qDebug() << "Failed to get tasks by parent id:" << query.lastError().text();
        return tasks;
    }

    while (query.next()) {
        Task task;
        task.setId(query.value(0).toInt());
        task.setTitle(query.value(1).toString());
        task.setDescription(query.value(2).toString());
        task.setPriority(query.value(3).toInt());
        task.setDueDate(QDateTime::fromString(query.value(4).toString(), Qt::ISODate));
        task.setCompleted(query.value(5).toBool());
        task.setParentId(query.value(6).toInt());
        task.setProgress(query.value(7).toDouble());
        task.setCreatedAt(QDateTime::fromString(query.value(8).toString(), Qt::ISODate));
        task.setUpdatedAt(QDateTime::fromString(query.value(9).toString(), Qt::ISODate));

        task.setTagIds(getTagIdsForTask(task.id()));
        task.setDependencyIds(getDependencyIdsForTask(task.id()));
        task.setFilePaths(getFilePathsForTask(task.id()));

        tasks.append(task);
    }

    return tasks;
}

Task DatabaseManager::getTaskById(int id)
{
    Task task;
    QSqlQuery query(m_database);

    query.prepare("SELECT id, title, description, priority, due_date, completed, parent_id, progress, created_at, updated_at FROM tasks WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec() || !query.next()) {
        qDebug() << "Failed to get task by id:" << query.lastError().text();
        return task;
    }

    task.setId(query.value(0).toInt());
    task.setTitle(query.value(1).toString());
    task.setDescription(query.value(2).toString());
    task.setPriority(query.value(3).toInt());
    task.setDueDate(QDateTime::fromString(query.value(4).toString(), Qt::ISODate));
    task.setCompleted(query.value(5).toBool());
    task.setParentId(query.value(6).toInt());
    task.setProgress(query.value(7).toDouble());
    task.setCreatedAt(QDateTime::fromString(query.value(8).toString(), Qt::ISODate));
    task.setUpdatedAt(QDateTime::fromString(query.value(9).toString(), Qt::ISODate));

    task.setTagIds(getTagIdsForTask(task.id()));
    task.setDependencyIds(getDependencyIdsForTask(task.id()));
    task.setFilePaths(getFilePathsForTask(task.id()));

    return task;
}

bool DatabaseManager::insertTask(Task &task)
{
    QSqlQuery query(m_database);

    query.prepare(R"(
        INSERT INTO tasks (title, description, priority, due_date, completed, parent_id, progress, created_at, updated_at)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?)
    )");

    query.addBindValue(task.title());
    query.addBindValue(task.description());
    query.addBindValue(static_cast<int>(task.priority()));
    query.addBindValue(task.dueDate().toString(Qt::ISODate));
    query.addBindValue(task.isCompleted() ? 1 : 0);
    query.addBindValue(task.parentId());
    query.addBindValue(task.progress());
    query.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    query.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));

    if (!query.exec()) {
        qDebug() << "Failed to insert task:" << query.lastError().text();
        return false;
    }

    task.setId(query.lastInsertId().toInt());

    for (int tagId : task.tagIds()) {
        assignTagToTask(task.id(), tagId);
    }

    for (int depId : task.dependencyIds()) {
        addDependency(task.id(), depId);
    }

    for (const QString &filePath : task.filePaths()) {
        QFileInfo fileInfo(filePath);
        addFileToTask(task.id(), filePath, fileInfo.fileName());
    }

    return true;
}

bool DatabaseManager::updateTask(const Task &task)
{
    QSqlQuery query(m_database);

    query.prepare(R"(
        UPDATE tasks SET
            title = ?,
            description = ?,
            priority = ?,
            due_date = ?,
            completed = ?,
            parent_id = ?,
            progress = ?,
            updated_at = ?
        WHERE id = ?
    )");

    query.addBindValue(task.title());
    query.addBindValue(task.description());
    query.addBindValue(static_cast<int>(task.priority()));
    query.addBindValue(task.dueDate().toString(Qt::ISODate));
    query.addBindValue(task.isCompleted() ? 1 : 0);
    query.addBindValue(task.parentId());
    query.addBindValue(task.progress());
    query.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    query.addBindValue(task.id());

    if (!query.exec()) {
        qDebug() << "Failed to update task:" << query.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::deleteTask(int id)
{
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM tasks WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec()) {
        qDebug() << "Failed to delete task:" << query.lastError().text();
        return false;
    }

    return true;
}

QList<Tag> DatabaseManager::getAllTags()
{
    QList<Tag> tags;
    QSqlQuery query(m_database);

    query.exec("SELECT id, name, color FROM tags ORDER BY name");

    while (query.next()) {
        Tag tag;
        tag.setId(query.value(0).toInt());
        tag.setName(query.value(1).toString());
        tag.setColor(query.value(2).toString());
        tags.append(tag);
    }

    return tags;
}

Tag DatabaseManager::getTagById(int id)
{
    Tag tag;
    QSqlQuery query(m_database);

    query.prepare("SELECT id, name, color FROM tags WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec() || !query.next()) {
        return tag;
    }

    tag.setId(query.value(0).toInt());
    tag.setName(query.value(1).toString());
    tag.setColor(query.value(2).toString());

    return tag;
}

bool DatabaseManager::insertTag(Tag &tag)
{
    QSqlQuery query(m_database);

    query.prepare("INSERT INTO tags (name, color) VALUES (?, ?)");
    query.addBindValue(tag.name());
    query.addBindValue(tag.color());

    if (!query.exec()) {
        qDebug() << "Failed to insert tag:" << query.lastError().text();
        return false;
    }

    tag.setId(query.lastInsertId().toInt());
    return true;
}

bool DatabaseManager::updateTag(const Tag &tag)
{
    QSqlQuery query(m_database);

    query.prepare("UPDATE tags SET name = ?, color = ? WHERE id = ?");
    query.addBindValue(tag.name());
    query.addBindValue(tag.color());
    query.addBindValue(tag.id());

    if (!query.exec()) {
        qDebug() << "Failed to update tag:" << query.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::deleteTag(int id)
{
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM tags WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec()) {
        qDebug() << "Failed to delete tag:" << query.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::assignTagToTask(int taskId, int tagId)
{
    QSqlQuery query(m_database);

    query.prepare("INSERT OR IGNORE INTO task_tags (task_id, tag_id) VALUES (?, ?)");
    query.addBindValue(taskId);
    query.addBindValue(tagId);

    if (!query.exec()) {
        qDebug() << "Failed to assign tag to task:" << query.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::removeTagFromTask(int taskId, int tagId)
{
    QSqlQuery query(m_database);

    query.prepare("DELETE FROM task_tags WHERE task_id = ? AND tag_id = ?");
    query.addBindValue(taskId);
    query.addBindValue(tagId);

    if (!query.exec()) {
        qDebug() << "Failed to remove tag from task:" << query.lastError().text();
        return false;
    }

    return true;
}

QList<int> DatabaseManager::getTagIdsForTask(int taskId)
{
    QList<int> tagIds;
    QSqlQuery query(m_database);

    query.prepare("SELECT tag_id FROM task_tags WHERE task_id = ?");
    query.addBindValue(taskId);

    if (!query.exec()) {
        qDebug() << "Failed to get tag ids for task:" << query.lastError().text();
        return tagIds;
    }

    while (query.next()) {
        tagIds.append(query.value(0).toInt());
    }

    return tagIds;
}

QList<int> DatabaseManager::getDependencyIdsForTask(int taskId)
{
    QList<int> depIds;
    QSqlQuery query(m_database);

    query.prepare("SELECT depends_on_id FROM task_dependencies WHERE task_id = ?");
    query.addBindValue(taskId);

    if (!query.exec()) {
        qDebug() << "Failed to get dependency ids for task:" << query.lastError().text();
        return depIds;
    }

    while (query.next()) {
        depIds.append(query.value(0).toInt());
    }

    return depIds;
}

bool DatabaseManager::addDependency(int taskId, int dependsOnId)
{
    QSqlQuery query(m_database);

    query.prepare("INSERT OR IGNORE INTO task_dependencies (task_id, depends_on_id) VALUES (?, ?)");
    query.addBindValue(taskId);
    query.addBindValue(dependsOnId);

    if (!query.exec()) {
        qDebug() << "Failed to add dependency:" << query.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::removeDependency(int taskId, int dependsOnId)
{
    QSqlQuery query(m_database);

    query.prepare("DELETE FROM task_dependencies WHERE task_id = ? AND depends_on_id = ?");
    query.addBindValue(taskId);
    query.addBindValue(dependsOnId);

    if (!query.exec()) {
        qDebug() << "Failed to remove dependency:" << query.lastError().text();
        return false;
    }

    return true;
}

QList<QString> DatabaseManager::getFilePathsForTask(int taskId)
{
    QList<QString> filePaths;
    QSqlQuery query(m_database);

    query.prepare("SELECT file_path FROM task_files WHERE task_id = ?");
    query.addBindValue(taskId);

    if (!query.exec()) {
        qDebug() << "Failed to get file paths for task:" << query.lastError().text();
        return filePaths;
    }

    while (query.next()) {
        filePaths.append(query.value(0).toString());
    }

    return filePaths;
}

bool DatabaseManager::addFileToTask(int taskId, const QString &filePath, const QString &fileName)
{
    QSqlQuery query(m_database);

    query.prepare("INSERT INTO task_files (task_id, file_path, file_name) VALUES (?, ?, ?)");
    query.addBindValue(taskId);
    query.addBindValue(filePath);
    query.addBindValue(fileName);

    if (!query.exec()) {
        qDebug() << "Failed to add file to task:" << query.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::removeFileFromTask(int fileId)
{
    QSqlQuery query(m_database);

    query.prepare("DELETE FROM task_files WHERE id = ?");
    query.addBindValue(fileId);

    if (!query.exec()) {
        qDebug() << "Failed to remove file from task:" << query.lastError().text();
        return false;
    }

    return true;
}

double DatabaseManager::calculateProgress(int taskId)
{
    QList<Task> subTasks = getTasksByParentId(taskId);

    if (subTasks.isEmpty()) {
        return 0.0;
    }

    int total = subTasks.size();
    int completed = 0;

    for (const Task &task : subTasks) {
        if (task.isCompleted()) {
            completed++;
        }
    }

    return static_cast<double>(completed) / static_cast<double>(total);
}
