#include "database.h"
#include "../models/task.h"
#include "../models/task_step.h"
#include "../models/tag.h"
#include "../models/notification.h"
#include "../models/folder.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDir>
#include <QDebug>
#include <QDateTime>
#include <functional>

namespace {
bool columnExists(QSqlDatabase &database, const QString &tableName, const QString &columnName)
{
    QSqlQuery query(database);
    query.prepare(QString("PRAGMA table_info(%1)").arg(tableName));
    if (!query.exec()) {
        return false;
    }

    while (query.next()) {
        if (query.value(1).toString().compare(columnName, Qt::CaseInsensitive) == 0) {
            return true;
        }
    }

    return false;
}

bool ensureTasksFilePathColumn(QSqlDatabase &database)
{
    if (columnExists(database, "tasks", "file_path")) {
        return true;
    }

    QSqlQuery query(database);
    if (!query.exec("ALTER TABLE tasks ADD COLUMN file_path TEXT")) {
        qDebug() << "Failed to add file_path column:" << query.lastError().text();
        return false;
    }

    return true;
}

QList<QString> loadTaskFilePaths(QSqlDatabase &database, int taskId)
{
    QList<QString> filePaths;
    QSqlQuery query(database);
    query.prepare("SELECT file_path FROM task_files WHERE task_id = ? ORDER BY created_at ASC");
    query.addBindValue(taskId);

    if (query.exec()) {
        while (query.next()) {
            filePaths.append(query.value(0).toString());
        }
    }

    return filePaths;
}
} // namespace

Database& Database::instance()
{
    static Database instance;
    return instance;
}

Database::Database()
{
    m_databasePath = QDir::currentPath() + "/data/todolist.db";
}

Database::~Database()
{
    if (m_database.isOpen()) {
        m_database.close();
    }
}

bool Database::open()
{
    QDir dataDir(QDir::currentPath() + "/data");
    if (!dataDir.exists()) {
        dataDir.mkpath(".");
    }

    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(m_databasePath);

    if (!m_database.open()) {
        qDebug() << "Database error:" << m_database.lastError().text();
        return false;
    }

    if (!createTables()) {
        return false;
    }

    if (!createIndexes()) {
        return false;
    }

    if (!createFTS5Table()) {
        return false;
    }

    return true;
}

void Database::close()
{
    if (m_database.isOpen()) {
        m_database.close();
    }
}

QSqlDatabase& Database::database()
{
    return m_database;
}

bool Database::createTables()
{
    QSqlQuery query(m_database);

    QString tasksTable = R"(
        CREATE TABLE IF NOT EXISTS tasks (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            title TEXT NOT NULL,
            description TEXT,
            file_path TEXT,
            priority INTEGER DEFAULT 1,
            due_date TEXT,
            completed INTEGER DEFAULT 0,
            progress REAL DEFAULT 0.0,
            parent_id INTEGER,
            is_deleted INTEGER DEFAULT 0,
            deleted_at TEXT,
            created_at TEXT DEFAULT CURRENT_TIMESTAMP,
            updated_at TEXT DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (parent_id) REFERENCES tasks(id)
        )
    )";

    QString taskStepsTable = R"(
        CREATE TABLE IF NOT EXISTS task_steps (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            task_id INTEGER NOT NULL,
            title TEXT NOT NULL,
            completed INTEGER DEFAULT 0,
            position INTEGER DEFAULT 0,
            created_at TEXT DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (task_id) REFERENCES tasks(id) ON DELETE CASCADE
        )
    )";

    QString tagsTable = R"(
        CREATE TABLE IF NOT EXISTS tags (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT UNIQUE NOT NULL,
            color TEXT DEFAULT '#3B82F6',
            created_at TEXT DEFAULT CURRENT_TIMESTAMP
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
            created_at TEXT DEFAULT CURRENT_TIMESTAMP,
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
            created_at TEXT DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (task_id) REFERENCES tasks(id) ON DELETE CASCADE
        )
    )";

    QString foldersTable = R"(
        CREATE TABLE IF NOT EXISTS folders (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT UNIQUE NOT NULL,
            color TEXT DEFAULT '#64748B',
            position INTEGER DEFAULT 0,
            created_at TEXT DEFAULT CURRENT_TIMESTAMP
        )
    )";

    QString taskFoldersTable = R"(
        CREATE TABLE IF NOT EXISTS task_folders (
            task_id INTEGER,
            folder_id INTEGER,
            PRIMARY KEY (task_id, folder_id),
            FOREIGN KEY (task_id) REFERENCES tasks(id) ON DELETE CASCADE,
            FOREIGN KEY (folder_id) REFERENCES folders(id) ON DELETE CASCADE
        )
    )";

    QString notificationsTable = R"(
        CREATE TABLE IF NOT EXISTS notifications (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            type TEXT NOT NULL,
            title TEXT NOT NULL,
            message TEXT,
            task_id INTEGER,
            read INTEGER DEFAULT 0,
            created_at TEXT DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (task_id) REFERENCES tasks(id) ON DELETE CASCADE
        )
    )";

    QString settingsTable = R"(
        CREATE TABLE IF NOT EXISTS settings (
            key TEXT PRIMARY KEY,
            value TEXT NOT NULL,
            updated_at TEXT DEFAULT CURRENT_TIMESTAMP
        )
    )";

    QString backupHistoryTable = R"(
        CREATE TABLE IF NOT EXISTS backup_history (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            file_path TEXT NOT NULL,
            file_size INTEGER,
            backup_time TEXT DEFAULT CURRENT_TIMESTAMP,
            description TEXT
        )
    )";

    QStringList tables = {
        tasksTable, taskStepsTable, tagsTable, taskTagsTable,
        taskDependenciesTable, taskFilesTable, foldersTable,
        taskFoldersTable, notificationsTable, settingsTable,
        backupHistoryTable
    };

    for (const QString &tableSql : tables) {
        if (!query.exec(tableSql)) {
            qDebug() << "Failed to create table:" << query.lastError().text();
            return false;
        }
    }

    if (!ensureTasksFilePathColumn(m_database)) {
        return false;
    }

    return true;
}

bool Database::createIndexes()
{
    QSqlQuery query(m_database);

    QStringList indexes = {
        "CREATE INDEX IF NOT EXISTS idx_tasks_parent_id ON tasks(parent_id)",
        "CREATE INDEX IF NOT EXISTS idx_tasks_due_date ON tasks(due_date)",
        "CREATE INDEX IF NOT EXISTS idx_tasks_priority ON tasks(priority)",
        "CREATE INDEX IF NOT EXISTS idx_tasks_is_deleted ON tasks(is_deleted)",
        "CREATE INDEX IF NOT EXISTS idx_tasks_completed ON tasks(completed)",
        "CREATE INDEX IF NOT EXISTS idx_tasks_created_at ON tasks(created_at)",
        "CREATE INDEX IF NOT EXISTS idx_task_steps_task_id ON task_steps(task_id)",
        "CREATE INDEX IF NOT EXISTS idx_notifications_read ON notifications(read)",
        "CREATE INDEX IF NOT EXISTS idx_notifications_created_at ON notifications(created_at)",
        "CREATE INDEX IF NOT EXISTS idx_task_tags_tag_id ON task_tags(tag_id)",
        "CREATE INDEX IF NOT EXISTS idx_task_folders_folder_id ON task_folders(folder_id)"
    };

    for (const QString &indexSql : indexes) {
        if (!query.exec(indexSql)) {
            qDebug() << "Failed to create index:" << query.lastError().text();
        }
    }

    return true;
}

bool Database::createFTS5Table()
{
    QSqlQuery query(m_database);
    bool existed = false;
    {
        QSqlQuery checkQuery(m_database);
        checkQuery.prepare("SELECT 1 FROM sqlite_master WHERE type='table' AND name='tasks_fts'");
        if (checkQuery.exec() && checkQuery.next()) {
            existed = true;
        }
    }

    QString ftsTable = R"(
        CREATE VIRTUAL TABLE IF NOT EXISTS tasks_fts USING fts5(
            title,
            description,
            content=tasks,
            content_rowid=id
        )
    )";

    if (!query.exec(ftsTable)) {
        qDebug() << "Failed to create FTS5 table:" << query.lastError().text();
        return false;
    }

    if (!existed) {
        if (!query.exec("INSERT INTO tasks_fts(tasks_fts) VALUES('rebuild')")) {
            qDebug() << "Failed to rebuild FTS5 table:" << query.lastError().text();
        }
    }

    QString triggers[] = {
        R"(
            CREATE TRIGGER IF NOT EXISTS tasks_ai AFTER INSERT ON tasks BEGIN
                INSERT INTO tasks_fts(rowid, title, description)
                VALUES (new.id, new.title, new.description);
            END
        )",
        R"(
            CREATE TRIGGER IF NOT EXISTS tasks_ad AFTER DELETE ON tasks BEGIN
                DELETE FROM tasks_fts WHERE rowid = old.id;
            END
        )",
        R"(
            CREATE TRIGGER IF NOT EXISTS tasks_au AFTER UPDATE ON tasks BEGIN
                UPDATE tasks_fts SET title = new.title, description = new.description
                WHERE rowid = new.id;
            END
        )"
    };

    for (const QString &trigger : triggers) {
        if (!query.exec(trigger)) {
            qDebug() << "Failed to create FTS5 trigger:" << query.lastError().text();
        }
    }

    return true;
}

bool Database::setSetting(const QString &key, const QString &value)
{
    QSqlQuery query(m_database);

    query.prepare(R"(
        INSERT OR REPLACE INTO settings (key, value, updated_at)
        VALUES (?, ?, ?)
    )");
    query.addBindValue(key);
    query.addBindValue(value);
    query.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));

    return query.exec();
}

QString Database::getSetting(const QString &key, const QString &defaultValue)
{
    QSqlQuery query(m_database);

    query.prepare("SELECT value FROM settings WHERE key = ?");
    query.addBindValue(key);

    if (!query.exec() || !query.next()) {
        return defaultValue;
    }

    return query.value(0).toString();
}

void Database::vacuum()
{
    QSqlQuery query(m_database);
    query.exec("VACUUM");
}

QList<Task> Database::getAllTasks()
{
    QList<Task> tasks;
    QSqlQuery query(m_database);
    query.prepare(R"(
        SELECT t.id, t.title, t.description, t.priority, t.due_date, t.completed, t.progress, t.parent_id, t.created_at, t.updated_at,
               CASE WHEN EXISTS(SELECT 1 FROM tasks child WHERE child.parent_id = t.id AND child.is_deleted = 0) THEN 1 ELSE 0 END as has_children
        FROM tasks t
        WHERE t.is_deleted = 0
        ORDER BY t.created_at DESC
    )");

    if (query.exec()) {
        while (query.next()) {
            Task task;
            task.setId(query.value(0).toInt());
            task.setTitle(query.value(1).toString());
            task.setDescription(query.value(2).toString());
            task.setPriority(query.value(3).toInt());
            task.setDueDate(QDateTime::fromString(query.value(4).toString(), Qt::ISODate));
            task.setCompleted(query.value(5).toBool());
            task.setProgress(query.value(6).toDouble());
            task.setParentId(query.value(7).toInt());
            task.setCreatedAt(QDateTime::fromString(query.value(8).toString(), Qt::ISODate));
            task.setUpdatedAt(QDateTime::fromString(query.value(9).toString(), Qt::ISODate));
            task.setHasChildren(query.value(10).toBool());
            tasks.append(task);
        }
    }

    return tasks;
}

QList<Task> Database::getTasksByParentId(int parentId)
{
    QList<Task> tasks;
    QSqlQuery query(m_database);
    query.prepare(R"(
        SELECT t.id, t.title, t.description, t.priority, t.due_date, t.completed, t.progress, t.parent_id, t.created_at, t.updated_at,
               CASE WHEN EXISTS(SELECT 1 FROM tasks child WHERE child.parent_id = t.id AND child.is_deleted = 0) THEN 1 ELSE 0 END as has_children
        FROM tasks t
        WHERE t.is_deleted = 0 AND t.parent_id = ?
        ORDER BY t.created_at ASC
    )");
    query.addBindValue(parentId);

    if (query.exec()) {
        while (query.next()) {
            Task task;
            task.setId(query.value(0).toInt());
            task.setTitle(query.value(1).toString());
            task.setDescription(query.value(2).toString());
            task.setPriority(query.value(3).toInt());
            task.setDueDate(QDateTime::fromString(query.value(4).toString(), Qt::ISODate));
            task.setCompleted(query.value(5).toBool());
            task.setProgress(query.value(6).toDouble());
            task.setParentId(query.value(7).toInt());
            task.setCreatedAt(QDateTime::fromString(query.value(8).toString(), Qt::ISODate));
            task.setUpdatedAt(QDateTime::fromString(query.value(9).toString(), Qt::ISODate));
            task.setHasChildren(query.value(10).toBool());
            tasks.append(task);
        }
    }

    return tasks;
}

Task Database::getTaskById(int id, bool includeDeleted)
{
    Task task;
    QSqlQuery query(m_database);
    QString deletedClause = includeDeleted ? "" : "AND t.is_deleted = 0";
    QString queryStr = QString(R"(
        SELECT t.id, t.title, t.description, t.priority, t.due_date, t.completed, t.progress, t.parent_id, t.created_at, t.updated_at, t.file_path,
               CASE WHEN EXISTS(SELECT 1 FROM tasks child WHERE child.parent_id = t.id AND child.is_deleted = 0) THEN 1 ELSE 0 END as has_children
        FROM tasks t
        WHERE t.id = ? %1
    )").arg(deletedClause);
    query.prepare(queryStr);
    query.addBindValue(id);

    if (query.exec() && query.next()) {
        task.setId(query.value(0).toInt());
        task.setTitle(query.value(1).toString());
        task.setDescription(query.value(2).toString());
        task.setPriority(query.value(3).toInt());
        task.setDueDate(QDateTime::fromString(query.value(4).toString(), Qt::ISODate));
        task.setCompleted(query.value(5).toBool());
        task.setProgress(query.value(6).toDouble());
        task.setParentId(query.value(7).toInt());
        task.setCreatedAt(QDateTime::fromString(query.value(8).toString(), Qt::ISODate));
        task.setUpdatedAt(QDateTime::fromString(query.value(9).toString(), Qt::ISODate));
        QString primaryFilePath = query.value(10).toString();
        task.setHasChildren(query.value(11).toBool());

        QList<QString> filePaths = loadTaskFilePaths(m_database, task.id());
        if (filePaths.isEmpty() && !primaryFilePath.isEmpty()) {
            filePaths.append(primaryFilePath);
        }
        task.setFilePaths(filePaths);
    }

    return task;
}

QList<Task> Database::getTaskHierarchy(int rootId)
{
    QList<Task> tasks;
    QSqlQuery query(m_database);
    
    QString recursiveQuery = QString::fromUtf8(
        "WITH RECURSIVE task_tree AS ("
        "SELECT id, title, description, priority, due_date, completed, progress, parent_id, created_at, updated_at, 0 as level "
        "FROM tasks "
        "WHERE is_deleted = 0 AND (parent_id = ? OR (? = 0 AND parent_id = 0)) "
        "UNION ALL "
        "SELECT t.id, t.title, t.description, t.priority, t.due_date, t.completed, t.progress, t.parent_id, t.created_at, t.updated_at, tt.level + 1 "
        "FROM tasks t "
        "INNER JOIN task_tree tt ON t.parent_id = tt.id "
        "WHERE t.is_deleted = 0 "
        ") "
        "SELECT id, title, description, priority, due_date, completed, progress, parent_id, created_at, updated_at, level, "
        "CASE WHEN EXISTS(SELECT 1 FROM tasks child WHERE child.parent_id = id AND child.is_deleted = 0) THEN 1 ELSE 0 END as has_children "
        "FROM task_tree "
        "ORDER BY level, created_at"
    );
    
    query.prepare(recursiveQuery);
    query.addBindValue(rootId);
    query.addBindValue(rootId);
    
    if (query.exec()) {
        while (query.next()) {
            Task task;
            task.setId(query.value(0).toInt());
            task.setTitle(query.value(1).toString());
            task.setDescription(query.value(2).toString());
            task.setPriority(query.value(3).toInt());
            task.setDueDate(QDateTime::fromString(query.value(4).toString(), Qt::ISODate));
            task.setCompleted(query.value(5).toBool());
            task.setProgress(query.value(6).toDouble());
            task.setParentId(query.value(7).toInt());
            task.setCreatedAt(QDateTime::fromString(query.value(8).toString(), Qt::ISODate));
            task.setUpdatedAt(QDateTime::fromString(query.value(9).toString(), Qt::ISODate));
            task.setHasChildren(query.value(11).toBool());
            tasks.append(task);
        }
    }
    
    return tasks;
}

bool Database::insertTask(Task &task)
{
    QString primaryFilePath = task.filePaths().isEmpty() ? QString() : task.filePaths().first();
    QSqlQuery query(m_database);
    query.prepare(R"(
        INSERT INTO tasks (title, description, file_path, priority, due_date, completed, progress, parent_id, created_at, updated_at)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
    )");
    query.addBindValue(task.title());
    query.addBindValue(task.description());
    query.addBindValue(primaryFilePath);
    query.addBindValue(static_cast<int>(task.priority()));
    query.addBindValue(task.dueDate().toString(Qt::ISODate));
    query.addBindValue(task.isCompleted() ? 1 : 0);
    query.addBindValue(task.progress());
    query.addBindValue(task.parentId());
    query.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    query.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));

    if (query.exec()) {
        task.setId(query.lastInsertId().toInt());
        return true;
    }

    return false;
}

bool Database::updateTask(const Task &task)
{
    QString primaryFilePath = task.filePaths().isEmpty() ? QString() : task.filePaths().first();
    QSqlQuery query(m_database);
    query.prepare(R"(
        UPDATE tasks
        SET title = ?, description = ?, file_path = ?, priority = ?, due_date = ?, completed = ?, progress = ?, parent_id = ?, updated_at = ?
        WHERE id = ?
    )");
    query.addBindValue(task.title());
    query.addBindValue(task.description());
    query.addBindValue(primaryFilePath);
    query.addBindValue(static_cast<int>(task.priority()));
    query.addBindValue(task.dueDate().toString(Qt::ISODate));
    query.addBindValue(task.isCompleted() ? 1 : 0);
    query.addBindValue(task.progress());
    query.addBindValue(task.parentId());
    query.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    query.addBindValue(task.id());

    return query.exec();
}

bool Database::deleteTask(int id)
{
    const int parentAction = getSetting("delete_parent_action", "0").toInt();
    const QString deletedAt = QDateTime::currentDateTime().toString(Qt::ISODate);

    auto markDeleted = [&](int taskId) -> bool {
        QSqlQuery updateQuery(m_database);
        updateQuery.prepare("UPDATE tasks SET is_deleted = 1, deleted_at = ? WHERE id = ? AND is_deleted = 0");
        updateQuery.addBindValue(deletedAt);
        updateQuery.addBindValue(taskId);
        return updateQuery.exec();
    };

    std::function<bool(int)> cascadeDelete = [&](int taskId) -> bool {
        QSqlQuery childQuery(m_database);
        childQuery.prepare("SELECT id FROM tasks WHERE parent_id = ? AND is_deleted = 0");
        childQuery.addBindValue(taskId);
        if (childQuery.exec()) {
            while (childQuery.next()) {
                int childId = childQuery.value(0).toInt();
                if (!cascadeDelete(childId)) {
                    return false;
                }
            }
        }
        return markDeleted(taskId);
    };

    if (parentAction == 1) {
        return cascadeDelete(id);
    }

    int parentId = 0;
    {
        QSqlQuery parentQuery(m_database);
        parentQuery.prepare("SELECT parent_id FROM tasks WHERE id = ? AND is_deleted = 0");
        parentQuery.addBindValue(id);
        if (parentQuery.exec() && parentQuery.next()) {
            parentId = parentQuery.value(0).toInt();
            if (parentId < 0) {
                parentId = 0;
            }
        }
    }

    if (parentAction == 0) {
        QSqlQuery reparentQuery(m_database);
        reparentQuery.prepare("UPDATE tasks SET parent_id = ?, updated_at = ? WHERE parent_id = ? AND is_deleted = 0");
        reparentQuery.addBindValue(parentId);
        reparentQuery.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
        reparentQuery.addBindValue(id);
        if (!reparentQuery.exec()) {
            return false;
        }
    }

    return markDeleted(id);
}

bool Database::restoreTask(int id)
{
    std::function<bool(int)> restoreRecursive = [&](int taskId) -> bool {
        int parentId = 0;
        {
            QSqlQuery parentQuery(m_database);
            parentQuery.prepare("SELECT parent_id FROM tasks WHERE id = ? AND is_deleted = 1");
            parentQuery.addBindValue(taskId);
            if (parentQuery.exec() && parentQuery.next()) {
                parentId = parentQuery.value(0).toInt();
            }
        }

        if (parentId > 0) {
            QSqlQuery parentCheck(m_database);
            parentCheck.prepare("SELECT is_deleted FROM tasks WHERE id = ?");
            parentCheck.addBindValue(parentId);
            if (!(parentCheck.exec() && parentCheck.next()) || parentCheck.value(0).toInt() == 1) {
                parentId = 0;
            }
        }

        QSqlQuery query(m_database);
        query.prepare("UPDATE tasks SET is_deleted = 0, deleted_at = NULL, parent_id = ?, updated_at = ? WHERE id = ?");
        query.addBindValue(parentId);
        query.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
        query.addBindValue(taskId);
        if (!query.exec()) {
            return false;
        }

        QSqlQuery childQuery(m_database);
        childQuery.prepare("SELECT id FROM tasks WHERE parent_id = ? AND is_deleted = 1");
        childQuery.addBindValue(taskId);
        if (childQuery.exec()) {
            while (childQuery.next()) {
                int childId = childQuery.value(0).toInt();
                if (!restoreRecursive(childId)) {
                    return false;
                }
            }
        }

        return true;
    };

    return restoreRecursive(id);
}

bool Database::permanentlyDeleteTask(int id, int parentAction)
{
    int action = parentAction;
    if (action < 0) {
        action = getSetting("delete_parent_action", "0").toInt();
    }

    int parentId = 0;
    {
        QSqlQuery parentQuery(m_database);
        parentQuery.prepare("SELECT parent_id FROM tasks WHERE id = ?");
        parentQuery.addBindValue(id);
        if (parentQuery.exec() && parentQuery.next()) {
            parentId = parentQuery.value(0).toInt();
            if (parentId < 0) {
                parentId = 0;
            }
        }
    }

    if (action == 1) {
        QSqlQuery childQuery(m_database);
        childQuery.prepare("SELECT id FROM tasks WHERE parent_id = ?");
        childQuery.addBindValue(id);
        if (childQuery.exec()) {
            while (childQuery.next()) {
                int childId = childQuery.value(0).toInt();
                if (!permanentlyDeleteTask(childId, action)) {
                    return false;
                }
            }
        }
    } else if (action == 0) {
        QSqlQuery reparentQuery(m_database);
        reparentQuery.prepare("UPDATE tasks SET parent_id = ?, updated_at = ? WHERE parent_id = ?");
        reparentQuery.addBindValue(parentId);
        reparentQuery.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
        reparentQuery.addBindValue(id);
        if (!reparentQuery.exec()) {
            return false;
        }
    } else {
        QSqlQuery reparentQuery(m_database);
        reparentQuery.prepare("UPDATE tasks SET parent_id = 0, updated_at = ? WHERE parent_id = ?");
        reparentQuery.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
        reparentQuery.addBindValue(id);
        if (!reparentQuery.exec()) {
            return false;
        }
    }

    QSqlQuery cleanupQuery(m_database);
    cleanupQuery.prepare("DELETE FROM task_steps WHERE task_id = ?");
    cleanupQuery.addBindValue(id);
    cleanupQuery.exec();

    cleanupQuery.prepare("DELETE FROM task_tags WHERE task_id = ?");
    cleanupQuery.addBindValue(id);
    cleanupQuery.exec();

    cleanupQuery.prepare("DELETE FROM task_files WHERE task_id = ?");
    cleanupQuery.addBindValue(id);
    cleanupQuery.exec();

    cleanupQuery.prepare("DELETE FROM task_folders WHERE task_id = ?");
    cleanupQuery.addBindValue(id);
    cleanupQuery.exec();

    cleanupQuery.prepare("DELETE FROM task_dependencies WHERE task_id = ? OR depends_on_id = ?");
    cleanupQuery.addBindValue(id);
    cleanupQuery.addBindValue(id);
    cleanupQuery.exec();

    cleanupQuery.prepare("DELETE FROM notifications WHERE task_id = ?");
    cleanupQuery.addBindValue(id);
    cleanupQuery.exec();

    QSqlQuery query(m_database);
    query.prepare("DELETE FROM tasks WHERE id = ?");
    query.addBindValue(id);

    return query.exec();
}

int Database::cleanupDeletedTasks(int days)
{
    if (days <= 0) {
        return 0;
    }

    QString threshold = QDateTime::currentDateTime().addDays(-days).toString(Qt::ISODate);
    QSqlQuery query(m_database);
    query.prepare("SELECT id FROM tasks WHERE is_deleted = 1 AND deleted_at IS NOT NULL AND deleted_at <= ?");
    query.addBindValue(threshold);

    QList<int> ids;
    if (query.exec()) {
        while (query.next()) {
            ids.append(query.value(0).toInt());
        }
    }

    int deletedCount = 0;
    for (int id : ids) {
        if (permanentlyDeleteTask(id)) {
            deletedCount++;
        }
    }

    return deletedCount;
}

double Database::calculateProgress(int taskId)
{
    QSqlQuery query(m_database);
    query.prepare("SELECT id FROM tasks WHERE is_deleted = 0 AND parent_id = ?");
    query.addBindValue(taskId);

    QList<int> childIds;
    if (query.exec()) {
        while (query.next()) {
            childIds.append(query.value(0).toInt());
        }
    }

    double progress = 0.0;
    if (childIds.isEmpty()) {
        QSqlQuery leafQuery(m_database);
        leafQuery.prepare("SELECT completed FROM tasks WHERE id = ? AND is_deleted = 0");
        leafQuery.addBindValue(taskId);
        if (leafQuery.exec() && leafQuery.next()) {
            bool completed = leafQuery.value(0).toBool();
            progress = completed ? 1.0 : 0.0;
        }
    } else {
        double total = 0.0;
        for (int childId : childIds) {
            total += calculateProgress(childId);
        }
        progress = total / static_cast<double>(childIds.size());
    }

    QSqlQuery updateQuery(m_database);
    updateQuery.prepare("UPDATE tasks SET progress = ?, updated_at = ? WHERE id = ?");
    updateQuery.addBindValue(progress);
    updateQuery.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    updateQuery.addBindValue(taskId);
    updateQuery.exec();

    return progress;
}

double Database::calculateParentProgress(int taskId)
{
    double progress = calculateProgress(taskId);

    QSqlQuery parentQuery(m_database);
    parentQuery.prepare("SELECT parent_id FROM tasks WHERE id = ? AND is_deleted = 0");
    parentQuery.addBindValue(taskId);
    if (parentQuery.exec() && parentQuery.next()) {
        int parentId = parentQuery.value(0).toInt();
        if (parentId > 0) {
            calculateParentProgress(parentId);
        }
    }

    return progress;
}

QList<TaskStep> Database::getTaskSteps(int taskId)
{
    QList<TaskStep> steps;
    QSqlQuery query(m_database);
    query.prepare("SELECT id, task_id, title, completed, position FROM task_steps WHERE task_id = ? ORDER BY position ASC");
    query.addBindValue(taskId);

    if (query.exec()) {
        while (query.next()) {
            TaskStep step;
            step.setId(query.value(0).toInt());
            step.setTaskId(query.value(1).toInt());
            step.setTitle(query.value(2).toString());
            step.setCompleted(query.value(3).toBool());
            step.setPosition(query.value(4).toInt());
            steps.append(step);
        }
    }

    return steps;
}

bool Database::insertTaskStep(TaskStep &step)
{
    QSqlQuery query(m_database);
    query.prepare("INSERT INTO task_steps (task_id, title, completed, position) VALUES (?, ?, ?, ?)");
    query.addBindValue(step.taskId());
    query.addBindValue(step.title());
    query.addBindValue(step.isCompleted() ? 1 : 0);
    query.addBindValue(step.position());

    if (query.exec()) {
        step.setId(query.lastInsertId().toInt());
        return true;
    }

    return false;
}

bool Database::updateTaskStep(const TaskStep &step)
{
    QSqlQuery query(m_database);
    query.prepare("UPDATE task_steps SET title = ?, completed = ?, position = ? WHERE id = ?");
    query.addBindValue(step.title());
    query.addBindValue(step.isCompleted() ? 1 : 0);
    query.addBindValue(step.position());
    query.addBindValue(step.id());

    return query.exec();
}

bool Database::deleteTaskStep(int stepId)
{
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM task_steps WHERE id = ?");
    query.addBindValue(stepId);

    return query.exec();
}

bool Database::deleteTaskSteps(int taskId)
{
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM task_steps WHERE task_id = ?");
    query.addBindValue(taskId);

    return query.exec();
}

QList<Tag> Database::getAllTags()
{
    QList<Tag> tags;
    QSqlQuery query(m_database);
    query.prepare("SELECT id, name, color FROM tags ORDER BY created_at ASC");

    if (query.exec()) {
        while (query.next()) {
            Tag tag;
            tag.setId(query.value(0).toInt());
            tag.setName(query.value(1).toString());
            tag.setColor(query.value(2).toString());
            tags.append(tag);
        }
    }

    return tags;
}

QList<Tag> Database::getTagsByTaskId(int taskId)
{
    QList<Tag> tags;
    QSqlQuery query(m_database);
    query.prepare(R"(
        SELECT t.id, t.name, t.color
        FROM tags t
        INNER JOIN task_tags tt ON t.id = tt.tag_id
        WHERE tt.task_id = ?
        ORDER BY t.created_at ASC
    )");
    query.addBindValue(taskId);

    if (query.exec()) {
        while (query.next()) {
            Tag tag;
            tag.setId(query.value(0).toInt());
            tag.setName(query.value(1).toString());
            tag.setColor(query.value(2).toString());
            tags.append(tag);
        }
    }

    return tags;
}

bool Database::insertTag(Tag &tag)
{
    QSqlQuery query(m_database);
    query.prepare("INSERT INTO tags (name, color, created_at) VALUES (?, ?, ?)");
    query.addBindValue(tag.name());
    query.addBindValue(tag.color());
    query.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));

    if (query.exec()) {
        tag.setId(query.lastInsertId().toInt());
        return true;
    }

    return false;
}

bool Database::updateTag(const Tag &tag)
{
    QSqlQuery query(m_database);
    query.prepare("UPDATE tags SET name = ?, color = ? WHERE id = ?");
    query.addBindValue(tag.name());
    query.addBindValue(tag.color());
    query.addBindValue(tag.id());

    return query.exec();
}

bool Database::deleteTag(int id)
{
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM tags WHERE id = ?");
    query.addBindValue(id);

    return query.exec();
}

bool Database::assignTagToTask(int taskId, int tagId)
{
    QSqlQuery query(m_database);
    query.prepare("INSERT OR IGNORE INTO task_tags (task_id, tag_id) VALUES (?, ?)");
    query.addBindValue(taskId);
    query.addBindValue(tagId);

    return query.exec();
}

bool Database::removeTagFromTask(int taskId, int tagId)
{
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM task_tags WHERE task_id = ? AND tag_id = ?");
    query.addBindValue(taskId);
    query.addBindValue(tagId);

    return query.exec();
}

bool Database::removeAllTagsFromTask(int taskId)
{
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM task_tags WHERE task_id = ?");
    query.addBindValue(taskId);

    return query.exec();
}

bool Database::addDependency(int taskId, int dependsOnId)
{
    QSqlQuery query(m_database);
    query.prepare("INSERT OR IGNORE INTO task_dependencies (task_id, depends_on_id, created_at) VALUES (?, ?, ?)");
    query.addBindValue(taskId);
    query.addBindValue(dependsOnId);
    query.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));

    return query.exec();
}

bool Database::removeDependency(int taskId, int dependsOnId)
{
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM task_dependencies WHERE task_id = ? AND depends_on_id = ?");
    query.addBindValue(taskId);
    query.addBindValue(dependsOnId);

    return query.exec();
}

bool Database::removeAllDependenciesFromTask(int taskId)
{
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM task_dependencies WHERE task_id = ?");
    query.addBindValue(taskId);

    return query.exec();
}

QList<int> Database::getDependencyIdsForTask(int taskId)
{
    QList<int> dependencyIds;
    QSqlQuery query(m_database);
    query.prepare(R"(
        SELECT td.depends_on_id
        FROM task_dependencies td
        INNER JOIN tasks t ON t.id = td.depends_on_id
        WHERE td.task_id = ? AND t.is_deleted = 0
        ORDER BY td.created_at ASC
    )");
    query.addBindValue(taskId);

    if (query.exec()) {
        while (query.next()) {
            dependencyIds.append(query.value(0).toInt());
        }
    }

    return dependencyIds;
}

QList<Task> Database::getDependenciesForTask(int taskId)
{
    QList<Task> tasks;
    QSqlQuery query(m_database);
    query.prepare(R"(
        SELECT t.id, t.title, t.description, t.priority, t.due_date, t.completed, t.progress, t.parent_id, t.created_at, t.updated_at,
               CASE WHEN EXISTS(SELECT 1 FROM tasks child WHERE child.parent_id = t.id AND child.is_deleted = 0) THEN 1 ELSE 0 END as has_children
        FROM task_dependencies td
        INNER JOIN tasks t ON t.id = td.depends_on_id
        WHERE td.task_id = ? AND t.is_deleted = 0
        ORDER BY td.created_at ASC
    )");
    query.addBindValue(taskId);

    if (query.exec()) {
        while (query.next()) {
            Task task;
            task.setId(query.value(0).toInt());
            task.setTitle(query.value(1).toString());
            task.setDescription(query.value(2).toString());
            task.setPriority(query.value(3).toInt());
            task.setDueDate(QDateTime::fromString(query.value(4).toString(), Qt::ISODate));
            task.setCompleted(query.value(5).toBool());
            task.setProgress(query.value(6).toDouble());
            task.setParentId(query.value(7).toInt());
            task.setCreatedAt(QDateTime::fromString(query.value(8).toString(), Qt::ISODate));
            task.setUpdatedAt(QDateTime::fromString(query.value(9).toString(), Qt::ISODate));
            task.setHasChildren(query.value(10).toBool());
            tasks.append(task);
        }
    }

    return tasks;
}

bool Database::wouldCreateCircularDependency(int taskId, int dependsOnId)
{
    if (taskId <= 0 || dependsOnId <= 0) {
        return false;
    }

    if (taskId == dependsOnId) {
        return true;
    }

    QSqlQuery query(m_database);
    query.prepare(R"(
        WITH RECURSIVE dep_chain(id) AS (
            SELECT td.depends_on_id
            FROM task_dependencies td
            INNER JOIN tasks t ON t.id = td.depends_on_id
            WHERE td.task_id = ? AND t.is_deleted = 0
            UNION ALL
            SELECT td.depends_on_id
            FROM task_dependencies td
            INNER JOIN tasks t ON t.id = td.depends_on_id
            INNER JOIN dep_chain dc ON td.task_id = dc.id
            WHERE t.is_deleted = 0
        )
        SELECT 1 FROM dep_chain WHERE id = ? LIMIT 1
    )");
    query.addBindValue(dependsOnId);
    query.addBindValue(taskId);

    if (query.exec() && query.next()) {
        return true;
    }

    return false;
}

QList<Task> Database::getCircularDependencies(int taskId)
{
    QList<Task> tasks;
    if (taskId <= 0) {
        return tasks;
    }

    QSqlQuery query(m_database);
    query.prepare(R"(
        WITH RECURSIVE
        forward(id) AS (
            SELECT td.depends_on_id
            FROM task_dependencies td
            INNER JOIN tasks t ON t.id = td.depends_on_id
            WHERE td.task_id = ? AND t.is_deleted = 0
            UNION ALL
            SELECT td.depends_on_id
            FROM task_dependencies td
            INNER JOIN tasks t ON t.id = td.depends_on_id
            INNER JOIN forward f ON td.task_id = f.id
            WHERE t.is_deleted = 0
        ),
        reverse(id) AS (
            SELECT td.task_id
            FROM task_dependencies td
            INNER JOIN tasks t ON t.id = td.task_id
            WHERE td.depends_on_id = ? AND t.is_deleted = 0
            UNION ALL
            SELECT td.task_id
            FROM task_dependencies td
            INNER JOIN tasks t ON t.id = td.task_id
            INNER JOIN reverse r ON td.depends_on_id = r.id
            WHERE t.is_deleted = 0
        ),
        cycle_ids(id) AS (
            SELECT id FROM forward
            INTERSECT
            SELECT id FROM reverse
        )
        SELECT t.id, t.title, t.description, t.priority, t.due_date, t.completed, t.progress, t.parent_id, t.created_at, t.updated_at,
               CASE WHEN EXISTS(SELECT 1 FROM tasks child WHERE child.parent_id = t.id AND child.is_deleted = 0) THEN 1 ELSE 0 END as has_children
        FROM tasks t
        WHERE t.id IN (SELECT id FROM cycle_ids) AND t.id != ? AND t.is_deleted = 0
        ORDER BY t.title ASC
    )");
    query.addBindValue(taskId);
    query.addBindValue(taskId);
    query.addBindValue(taskId);

    if (query.exec()) {
        while (query.next()) {
            Task task;
            task.setId(query.value(0).toInt());
            task.setTitle(query.value(1).toString());
            task.setDescription(query.value(2).toString());
            task.setPriority(query.value(3).toInt());
            task.setDueDate(QDateTime::fromString(query.value(4).toString(), Qt::ISODate));
            task.setCompleted(query.value(5).toBool());
            task.setProgress(query.value(6).toDouble());
            task.setParentId(query.value(7).toInt());
            task.setCreatedAt(QDateTime::fromString(query.value(8).toString(), Qt::ISODate));
            task.setUpdatedAt(QDateTime::fromString(query.value(9).toString(), Qt::ISODate));
            task.setHasChildren(query.value(10).toBool());
            tasks.append(task);
        }
    }

    return tasks;
}

bool Database::addFileToTask(int taskId, const QString &filePath, const QString &fileName)
{
    QSqlQuery query(m_database);
    query.prepare("INSERT INTO task_files (task_id, file_path, file_name, created_at) VALUES (?, ?, ?, ?)");
    query.addBindValue(taskId);
    query.addBindValue(filePath);
    query.addBindValue(fileName);
    query.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));

    return query.exec();
}

bool Database::removeFileFromTask(int fileId)
{
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM task_files WHERE id = ?");
    query.addBindValue(fileId);

    return query.exec();
}

bool Database::removeAllFilesFromTask(int taskId)
{
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM task_files WHERE task_id = ?");
    query.addBindValue(taskId);

    return query.exec();
}

QList<Notification> Database::getAllNotifications()
{
    QList<Notification> notifications;
    QSqlQuery query(m_database);
    query.prepare("SELECT id, type, title, message, task_id, read, created_at FROM notifications ORDER BY created_at DESC");

    if (query.exec()) {
        while (query.next()) {
            Notification notification;
            notification.setId(query.value(0).toInt());
            notification.setType(query.value(1).toInt());
            notification.setTitle(query.value(2).toString());
            notification.setMessage(query.value(3).toString());
            notification.setTaskId(query.value(4).toInt());
            notification.setRead(query.value(5).toBool());
            notification.setCreatedAt(QDateTime::fromString(query.value(6).toString(), Qt::ISODate));
            notifications.append(notification);
        }
    }

    return notifications;
}

QList<Notification> Database::getUnreadNotifications()
{
    QList<Notification> notifications;
    QSqlQuery query(m_database);
    query.prepare("SELECT id, type, title, message, task_id, read, created_at FROM notifications WHERE read = 0 ORDER BY created_at DESC");

    if (query.exec()) {
        while (query.next()) {
            Notification notification;
            notification.setId(query.value(0).toInt());
            notification.setType(query.value(1).toInt());
            notification.setTitle(query.value(2).toString());
            notification.setMessage(query.value(3).toString());
            notification.setTaskId(query.value(4).toInt());
            notification.setRead(query.value(5).toBool());
            notification.setCreatedAt(QDateTime::fromString(query.value(6).toString(), Qt::ISODate));
            notifications.append(notification);
        }
    }

    return notifications;
}

QList<Notification> Database::getNotificationsByType(int type)
{
    QList<Notification> notifications;
    QSqlQuery query(m_database);
    query.prepare("SELECT id, type, title, message, task_id, read, created_at FROM notifications WHERE type = ? ORDER BY created_at DESC");
    query.addBindValue(type);

    if (query.exec()) {
        while (query.next()) {
            Notification notification;
            notification.setId(query.value(0).toInt());
            notification.setType(query.value(1).toInt());
            notification.setTitle(query.value(2).toString());
            notification.setMessage(query.value(3).toString());
            notification.setTaskId(query.value(4).toInt());
            notification.setRead(query.value(5).toBool());
            notification.setCreatedAt(QDateTime::fromString(query.value(6).toString(), Qt::ISODate));
            notifications.append(notification);
        }
    }

    return notifications;
}

Notification Database::getNotificationById(int id)
{
    Notification notification;
    QSqlQuery query(m_database);
    query.prepare("SELECT id, type, title, message, task_id, read, created_at FROM notifications WHERE id = ?");
    query.addBindValue(id);

    if (query.exec() && query.next()) {
        notification.setId(query.value(0).toInt());
        notification.setType(query.value(1).toInt());
        notification.setTitle(query.value(2).toString());
        notification.setMessage(query.value(3).toString());
        notification.setTaskId(query.value(4).toInt());
        notification.setRead(query.value(5).toBool());
        notification.setCreatedAt(QDateTime::fromString(query.value(6).toString(), Qt::ISODate));
    }

    return notification;
}

bool Database::insertNotification(Notification &notification)
{
    QSqlQuery query(m_database);
    query.prepare("INSERT INTO notifications (type, title, message, task_id, read, created_at) VALUES (?, ?, ?, ?, ?, ?)");
    query.addBindValue(static_cast<int>(notification.type()));
    query.addBindValue(notification.title());
    query.addBindValue(notification.message());
    query.addBindValue(notification.taskId());
    query.addBindValue(notification.isRead() ? 1 : 0);
    query.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));

    if (query.exec()) {
        notification.setId(query.lastInsertId().toInt());
        return true;
    }

    return false;
}

bool Database::updateNotification(const Notification &notification)
{
    QSqlQuery query(m_database);
    query.prepare("UPDATE notifications SET title = ?, message = ?, read = ? WHERE id = ?");
    query.addBindValue(notification.title());
    query.addBindValue(notification.message());
    query.addBindValue(notification.isRead() ? 1 : 0);
    query.addBindValue(notification.id());

    return query.exec();
}

bool Database::deleteNotification(int id)
{
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM notifications WHERE id = ?");
    query.addBindValue(id);

    return query.exec();
}

bool Database::markNotificationAsRead(int id)
{
    QSqlQuery query(m_database);
    query.prepare("UPDATE notifications SET read = 1 WHERE id = ?");
    query.addBindValue(id);

    return query.exec();
}

bool Database::markAllNotificationsAsRead()
{
    QSqlQuery query(m_database);
    query.prepare("UPDATE notifications SET read = 1");

    return query.exec();
}

int Database::getUnreadNotificationCount()
{
    QSqlQuery query(m_database);
    query.prepare("SELECT COUNT(*) FROM notifications WHERE read = 0");

    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }

    return 0;
}

QList<Folder> Database::getAllFolders()
{
    QList<Folder> folders;
    QSqlQuery query(m_database);
    query.prepare("SELECT id, name, color, position, created_at FROM folders ORDER BY position ASC");

    if (query.exec()) {
        while (query.next()) {
            Folder folder;
            folder.setId(query.value(0).toInt());
            folder.setName(query.value(1).toString());
            folder.setColor(query.value(2).toString());
            folder.setPosition(query.value(3).toInt());
            folder.setCreatedAt(QDateTime::fromString(query.value(4).toString(), Qt::ISODate));

            QList<int> taskIds = getTaskIdsByFolder(folder.id());
            folder.setTaskIds(taskIds);

            folders.append(folder);
        }
    }

    return folders;
}

Folder Database::getFolderById(int id)
{
    Folder folder;
    QSqlQuery query(m_database);
    query.prepare("SELECT id, name, color, position, created_at FROM folders WHERE id = ?");
    query.addBindValue(id);

    if (query.exec() && query.next()) {
        folder.setId(query.value(0).toInt());
        folder.setName(query.value(1).toString());
        folder.setColor(query.value(2).toString());
        folder.setPosition(query.value(3).toInt());
        folder.setCreatedAt(QDateTime::fromString(query.value(4).toString(), Qt::ISODate));

        QList<int> taskIds = getTaskIdsByFolder(folder.id());
        folder.setTaskIds(taskIds);
    }

    return folder;
}

bool Database::insertFolder(Folder &folder)
{
    QSqlQuery query(m_database);
    query.prepare("INSERT INTO folders (name, color, position, created_at) VALUES (?, ?, ?, ?)");
    query.addBindValue(folder.name());
    query.addBindValue(folder.color());
    query.addBindValue(folder.position());
    query.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));

    if (query.exec()) {
        folder.setId(query.lastInsertId().toInt());
        return true;
    }

    return false;
}

bool Database::updateFolder(const Folder &folder)
{
    QSqlQuery query(m_database);
    query.prepare("UPDATE folders SET name = ?, color = ?, position = ? WHERE id = ?");
    query.addBindValue(folder.name());
    query.addBindValue(folder.color());
    query.addBindValue(folder.position());
    query.addBindValue(folder.id());

    return query.exec();
}

bool Database::deleteFolder(int id)
{
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM folders WHERE id = ?");
    query.addBindValue(id);

    return query.exec();
}

bool Database::assignTaskToFolder(int taskId, int folderId)
{
    QSqlQuery query(m_database);
    query.prepare("INSERT OR IGNORE INTO task_folders (task_id, folder_id) VALUES (?, ?)");
    query.addBindValue(taskId);
    query.addBindValue(folderId);

    return query.exec();
}

bool Database::removeTaskFromFolder(int taskId, int folderId)
{
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM task_folders WHERE task_id = ? AND folder_id = ?");
    query.addBindValue(taskId);
    query.addBindValue(folderId);

    return query.exec();
}

QList<int> Database::getTaskIdsByFolder(int folderId)
{
    QList<int> taskIds;
    QSqlQuery query(m_database);
    query.prepare("SELECT task_id FROM task_folders WHERE folder_id = ?");
    query.addBindValue(folderId);

    if (query.exec()) {
        while (query.next()) {
            taskIds.append(query.value(0).toInt());
        }
    }

    return taskIds;
}
