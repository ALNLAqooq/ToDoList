#include "task_controller.h"
#include "../database/database_manager.h"
#include <QFileInfo>

TaskController::TaskController(QObject *parent)
    : QObject(parent)
{
}

TaskController::~TaskController()
{
}

QList<Task> TaskController::getAllTasks()
{
    return DatabaseManager::instance().getAllTasks();
}

QList<Task> TaskController::getSubTasks(int parentId)
{
    return DatabaseManager::instance().getTasksByParentId(parentId);
}

Task TaskController::getTaskById(int id)
{
    return DatabaseManager::instance().getTaskById(id);
}

bool TaskController::addTask(Task &task)
{
    if (DatabaseManager::instance().insertTask(task)) {
        emit taskAdded(task);
        return true;
    }
    return false;
}

bool TaskController::updateTask(const Task &task)
{
    if (DatabaseManager::instance().updateTask(task)) {
        emit taskUpdated(task);
        return true;
    }
    return false;
}

bool TaskController::deleteTask(int id)
{
    if (DatabaseManager::instance().deleteTask(id)) {
        emit taskDeleted(id);
        return true;
    }
    return false;
}

bool TaskController::toggleTaskCompletion(int id)
{
    Task task = getTaskById(id);
    task.setCompleted(!task.isCompleted());

    if (updateTask(task)) {
        emit taskCompletionChanged(id, task.isCompleted());
        return true;
    }
    return false;
}

QList<Tag> TaskController::getAllTags()
{
    return DatabaseManager::instance().getAllTags();
}

bool TaskController::addTag(Tag &tag)
{
    if (DatabaseManager::instance().insertTag(tag)) {
        emit tagsChanged();
        return true;
    }
    return false;
}

bool TaskController::updateTag(const Tag &tag)
{
    if (DatabaseManager::instance().updateTag(tag)) {
        emit tagsChanged();
        return true;
    }
    return false;
}

bool TaskController::deleteTag(int id)
{
    if (DatabaseManager::instance().deleteTag(id)) {
        emit tagsChanged();
        return true;
    }
    return false;
}

bool TaskController::assignTagToTask(int taskId, int tagId)
{
    if (DatabaseManager::instance().assignTagToTask(taskId, tagId)) {
        emit tagsChanged();
        return true;
    }
    return false;
}

bool TaskController::removeTagFromTask(int taskId, int tagId)
{
    if (DatabaseManager::instance().removeTagFromTask(taskId, tagId)) {
        emit tagsChanged();
        return true;
    }
    return false;
}

bool TaskController::addDependency(int taskId, int dependsOnId)
{
    if (DatabaseManager::instance().addDependency(taskId, dependsOnId)) {
        emit dependenciesChanged();
        return true;
    }
    return false;
}

bool TaskController::removeDependency(int taskId, int dependsOnId)
{
    if (DatabaseManager::instance().removeDependency(taskId, dependsOnId)) {
        emit dependenciesChanged();
        return true;
    }
    return false;
}

bool TaskController::addFileToTask(int taskId, const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    if (DatabaseManager::instance().addFileToTask(taskId, filePath, fileInfo.fileName())) {
        emit filesChanged();
        return true;
    }
    return false;
}

bool TaskController::removeFileFromTask(int fileId)
{
    if (DatabaseManager::instance().removeFileFromTask(fileId)) {
        emit filesChanged();
        return true;
    }
    return false;
}

double TaskController::updateProgress(int taskId)
{
    double progress = DatabaseManager::instance().calculateProgress(taskId);
    Task task = getTaskById(taskId);
    task.setProgress(progress);
    updateTask(task);
    return progress;
}
