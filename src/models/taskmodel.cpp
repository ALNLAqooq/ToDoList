#include "taskmodel.h"
#include "../controllers/database.h"
#include "../models/task.h"
#include <QDebug>

TaskModel::TaskModel(QObject *parent)
    : QStandardItemModel(parent)
    , m_database(&Database::instance())
    , m_priorityFilter(0)
    , m_completedFilter(-1)
{
    setColumnCount(1);
}

TaskModel::~TaskModel()
{
}

QHash<int, QByteArray> TaskModel::roleNames() const
{
    QHash<int, QByteArray> roles = QStandardItemModel::roleNames();
    roles[IdRole] = "id";
    roles[TitleRole] = "title";
    roles[DescriptionRole] = "description";
    roles[PriorityRole] = "priority";
    roles[DueDateRole] = "dueDate";
    roles[CompletedRole] = "completed";
    roles[ProgressRole] = "progress";
    roles[ParentIdRole] = "parentId";
    roles[CreatedAtRole] = "createdAt";
    roles[UpdatedAtRole] = "updatedAt";
    roles[TagIdsRole] = "tagIds";
    roles[DependencyIdsRole] = "dependencyIds";
    roles[FilePathsRole] = "filePaths";
    return roles;
}

void TaskModel::loadTasks()
{
    clear();
    m_taskItems.clear();

    QList<Task> tasks = m_database->getAllTasks();

    for (const Task &task : tasks) {
        if (task.parentId() == 0) {
            QStandardItem *item = createTaskItem(task);
            appendRow(item);
            addChildTasks(task.id(), item);
        }
    }

    emit modelLoaded();
}

void TaskModel::loadTasksByParentId(int parentId)
{
    clear();
    m_taskItems.clear();

    QList<Task> tasks = m_database->getTasksByParentId(parentId);

    for (const Task &task : tasks) {
        QStandardItem *item = createTaskItem(task);
        appendRow(item);
    }

    emit modelLoaded();
}

void TaskModel::loadTaskHierarchy()
{
    clear();
    m_taskItems.clear();

    QList<Task> tasks = m_database->getAllTasks();

    QHash<int, QList<Task>> tasksByParent;
    for (const Task &task : tasks) {
        tasksByParent[task.parentId()].append(task);
    }

    QList<Task> rootTasks = tasksByParent[0];
    for (const Task &task : rootTasks) {
        QStandardItem *item = createTaskItem(task);
        appendRow(item);
        buildTaskHierarchy(task.id(), item, tasksByParent);
    }

    emit modelLoaded();
}

void TaskModel::buildTaskHierarchy(int parentId, QStandardItem *parentItem, const QHash<int, QList<Task>> &tasksByParent)
{
    if (!tasksByParent.contains(parentId)) {
        return;
    }

    QList<Task> childTasks = tasksByParent[parentId];
    for (const Task &task : childTasks) {
        QStandardItem *item = createTaskItem(task);
        parentItem->appendRow(item);
        buildTaskHierarchy(task.id(), item, tasksByParent);
    }
}

void TaskModel::reload()
{
    loadTaskHierarchy();
}

bool TaskModel::addTask(const Task &task)
{
    Task newTask = task;
    if (!m_database->insertTask(newTask)) {
        return false;
    }

    QStandardItem *item = createTaskItem(newTask);
    m_taskItems[newTask.id()] = item;

    if (newTask.parentId() == 0) {
        appendRow(item);
    } else {
        QStandardItem *parentItem = m_taskItems.value(newTask.parentId(), nullptr);
        if (parentItem) {
            parentItem->appendRow(item);
        } else {
            appendRow(item);
        }
    }

    emit taskAdded(newTask.id());
    return true;
}

bool TaskModel::updateTask(const Task &task)
{
    if (!m_database->updateTask(task)) {
        return false;
    }

    QStandardItem *item = m_taskItems.value(task.id(), nullptr);
    if (item) {
        item->setData(task.title(), Qt::DisplayRole);
        item->setData(task.title(), TitleRole);
        item->setData(task.description(), DescriptionRole);
        item->setData(static_cast<int>(task.priority()), PriorityRole);
        item->setData(task.dueDate(), DueDateRole);
        item->setData(task.isCompleted(), CompletedRole);
        item->setData(task.progress(), ProgressRole);
        item->setData(task.parentId(), ParentIdRole);
        item->setData(task.updatedAt(), UpdatedAtRole);

        QList<QVariant> tagIds;
        for (int tagId : task.tagIds()) {
            tagIds.append(tagId);
        }
        item->setData(tagIds, TagIdsRole);

        QList<QVariant> dependencyIds;
        for (int depId : task.dependencyIds()) {
            dependencyIds.append(depId);
        }
        item->setData(dependencyIds, DependencyIdsRole);

        QList<QVariant> filePaths;
        for (const QString &path : task.filePaths()) {
            filePaths.append(path);
        }
        item->setData(filePaths, FilePathsRole);
    }

    emit taskUpdated(task.id());
    return true;
}

bool TaskModel::deleteTask(int taskId)
{
    if (!m_database->deleteTask(taskId)) {
        return false;
    }

    QStandardItem *item = m_taskItems.value(taskId, nullptr);
    if (item) {
        QModelIndex index = indexFromItem(item);
        removeRow(index.row(), index.parent());
        m_taskItems.remove(taskId);
    }

    emit taskDeleted(taskId);
    return true;
}

bool TaskModel::setTaskCompleted(int taskId, bool completed)
{
    Task task = m_database->getTaskById(taskId);
    if (task.id() == 0) {
        return false;
    }

    task.setCompleted(completed);
    return updateTask(task);
}

bool TaskModel::setTaskProgress(int taskId, double progress)
{
    Task task = m_database->getTaskById(taskId);
    if (task.id() == 0) {
        return false;
    }

    task.setProgress(progress);
    return updateTask(task);
}

Task TaskModel::getTaskById(int taskId) const
{
    return m_database->getTaskById(taskId);
}

QStandardItem* TaskModel::getTaskItem(int taskId) const
{
    return m_taskItems.value(taskId, nullptr);
}

QModelIndex TaskModel::getTaskIndex(int taskId) const
{
    QStandardItem *item = getTaskItem(taskId);
    if (item) {
        return indexFromItem(item);
    }
    return QModelIndex();
}

void TaskModel::setFilter(const QString &filter)
{
    m_textFilter = filter;
    applyFilters();
}

void TaskModel::setFilterByPriority(int priority)
{
    m_priorityFilter = priority;
    applyFilters();
}

void TaskModel::setFilterByCompleted(bool completed)
{
    m_completedFilter = completed ? 1 : 0;
    applyFilters();
}

void TaskModel::clearFilters()
{
    m_textFilter.clear();
    m_priorityFilter = 0;
    m_completedFilter = -1;
    applyFilters();
}

void TaskModel::sortByPriority(Qt::SortOrder order)
{
    sort(0, order);
}

void TaskModel::sortByDueDate(Qt::SortOrder order)
{
    sort(0, order);
}

void TaskModel::sortByCreatedDate(Qt::SortOrder order)
{
    sort(0, order);
}

QStandardItem* TaskModel::createTaskItem(const Task &task)
{
    QStandardItem *item = new QStandardItem();
    item->setText(task.title());
    item->setData(task.id(), IdRole);
    item->setData(task.title(), TitleRole);
    item->setData(task.description(), DescriptionRole);
    item->setData(static_cast<int>(task.priority()), PriorityRole);
    item->setData(task.dueDate(), DueDateRole);
    item->setData(task.isCompleted(), CompletedRole);
    item->setData(task.progress(), ProgressRole);
    item->setData(task.parentId(), ParentIdRole);
    item->setData(task.createdAt(), CreatedAtRole);
    item->setData(task.updatedAt(), UpdatedAtRole);

    QList<QVariant> tagIds;
    for (int tagId : task.tagIds()) {
        tagIds.append(tagId);
    }
    item->setData(tagIds, TagIdsRole);

    QList<QVariant> dependencyIds;
    for (int depId : task.dependencyIds()) {
        dependencyIds.append(depId);
    }
    item->setData(dependencyIds, DependencyIdsRole);

    QList<QVariant> filePaths;
    for (const QString &path : task.filePaths()) {
        filePaths.append(path);
    }
    item->setData(filePaths, FilePathsRole);

    m_taskItems[task.id()] = item;

    return item;
}

void TaskModel::addChildTasks(int parentId, QStandardItem *parentItem)
{
    QList<Task> childTasks = m_database->getTasksByParentId(parentId);

    for (const Task &task : childTasks) {
        QStandardItem *childItem = createTaskItem(task);
        parentItem->appendRow(childItem);
        addChildTasks(task.id(), childItem);
    }
}

void TaskModel::applyFilters()
{
    Q_UNUSED(this);
}

bool TaskModel::matchesFilters(const Task &task) const
{
    if (!m_textFilter.isEmpty()) {
        QString searchText = m_textFilter.toLower();
        bool matchesTitle = task.title().toLower().contains(searchText);
        bool matchesDesc = task.description().toLower().contains(searchText);
        if (!matchesTitle && !matchesDesc) {
            return false;
        }
    }

    if (m_priorityFilter > 0) {
        if (static_cast<int>(task.priority()) != m_priorityFilter) {
            return false;
        }
    }

    if (m_completedFilter >= 0) {
        if (task.isCompleted() != (m_completedFilter == 1)) {
            return false;
        }
    }

    return true;
}
