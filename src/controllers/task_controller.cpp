#include "task_controller.h"
#include "database.h"
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
    return Database::instance().getAllTasks();
}

QList<Task> TaskController::getSubTasks(int parentId)
{
    return Database::instance().getTasksByParentId(parentId);
}

Task TaskController::getTaskById(int id)
{
    return Database::instance().getTaskById(id);
}

Task TaskController::getTaskByIdIncludingDeleted(int id)
{
    return Database::instance().getTaskById(id, true);
}

QList<Task> TaskController::getTaskHierarchy(int rootId)
{
    return Database::instance().getTaskHierarchy(rootId);
}

bool TaskController::addTask(Task &task)
{
    if (Database::instance().insertTask(task)) {
        emit taskAdded(task);
        return true;
    }
    return false;
}

bool TaskController::updateTask(const Task &task)
{
    if (Database::instance().updateTask(task)) {
        emit taskUpdated(task);
        return true;
    }
    return false;
}

bool TaskController::deleteTask(int id)
{
    Task task = getTaskById(id);
    if (Database::instance().deleteTask(id)) {
        emit taskDeleted(id);
        if (task.parentId() > 0) {
            updateParentProgress(task.parentId());
        }
        return true;
    }
    return false;
}

bool TaskController::restoreTask(int id)
{
    Task task = Database::instance().getTaskById(id, true);
    if (Database::instance().restoreTask(id)) {
        Task restored = getTaskById(id);
        if (restored.id() > 0) {
            emit taskUpdated(restored);
        }
        if (task.parentId() > 0) {
            updateParentProgress(task.parentId());
        }
        return true;
    }
    return false;
}

bool TaskController::permanentlyDeleteTask(int id)
{
    Task task = Database::instance().getTaskById(id, true);
    if (Database::instance().permanentlyDeleteTask(id)) {
        emit taskDeleted(id);
        if (task.parentId() > 0) {
            updateParentProgress(task.parentId());
        }
        return true;
    }
    return false;
}

bool TaskController::toggleTaskCompletion(int id)
{
    Task task = getTaskById(id);
    task.setCompleted(!task.isCompleted());

    if (updateTask(task)) {
        updateProgress(id);
        emit taskCompletionChanged(id, task.isCompleted());
        return true;
    }
    return false;
}

QList<Tag> TaskController::getAllTags()
{
    return Database::instance().getAllTags();
}

QList<Tag> TaskController::getTagsByTaskId(int taskId)
{
    return Database::instance().getTagsByTaskId(taskId);
}

bool TaskController::addTag(Tag &tag)
{
    if (Database::instance().insertTag(tag)) {
        emit tagsChanged();
        return true;
    }
    return false;
}

bool TaskController::updateTag(const Tag &tag)
{
    if (Database::instance().updateTag(tag)) {
        emit tagsChanged();
        return true;
    }
    return false;
}

bool TaskController::deleteTag(int id)
{
    if (Database::instance().deleteTag(id)) {
        emit tagsChanged();
        return true;
    }
    return false;
}

bool TaskController::assignTagToTask(int taskId, int tagId)
{
    if (Database::instance().assignTagToTask(taskId, tagId)) {
        emit tagsChanged();
        return true;
    }
    return false;
}

bool TaskController::removeTagFromTask(int taskId, int tagId)
{
    if (Database::instance().removeTagFromTask(taskId, tagId)) {
        emit tagsChanged();
        return true;
    }
    return false;
}

bool TaskController::addDependency(int taskId, int dependsOnId)
{
    if (Database::instance().addDependency(taskId, dependsOnId)) {
        emit dependenciesChanged();
        return true;
    }
    return false;
}

bool TaskController::removeDependency(int taskId, int dependsOnId)
{
    if (Database::instance().removeDependency(taskId, dependsOnId)) {
        emit dependenciesChanged();
        return true;
    }
    return false;
}

QList<int> TaskController::getDependencyIdsForTask(int taskId)
{
    return Database::instance().getDependencyIdsForTask(taskId);
}

QList<Task> TaskController::getDependenciesForTask(int taskId)
{
    return Database::instance().getDependenciesForTask(taskId);
}

bool TaskController::wouldCreateCircularDependency(int taskId, int dependsOnId)
{
    return Database::instance().wouldCreateCircularDependency(taskId, dependsOnId);
}

QList<Task> TaskController::getCircularDependencies(int taskId)
{
    return Database::instance().getCircularDependencies(taskId);
}

bool TaskController::addFileToTask(int taskId, const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    if (Database::instance().addFileToTask(taskId, filePath, fileInfo.fileName())) {
        emit filesChanged();
        return true;
    }
    return false;
}

bool TaskController::removeFileFromTask(int fileId)
{
    if (Database::instance().removeFileFromTask(fileId)) {
        emit filesChanged();
        return true;
    }
    return false;
}

double TaskController::updateProgress(int taskId)
{
    double progress = Database::instance().calculateProgress(taskId);
    Task updatedTask = getTaskById(taskId);
    emit taskUpdated(updatedTask);

    if (updatedTask.parentId() > 0) {
        updateParentProgress(updatedTask.parentId());
    }

    return progress;
}

double TaskController::updateParentProgress(int taskId)
{
    double progress = Database::instance().calculateProgress(taskId);
    Task task = getTaskById(taskId);
    emit taskUpdated(task);

    if (task.parentId() > 0) {
        updateParentProgress(task.parentId());
    }

    return progress;
}
