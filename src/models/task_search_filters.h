#ifndef TASK_SEARCH_FILTERS_H
#define TASK_SEARCH_FILTERS_H

#include <QString>
#include <QList>

enum class TaskSearchStatusFilter {
    Any = 0,
    Completed = 1,
    Incomplete = 2,
    InProgress = 3
};

enum class TaskSearchDateFilter {
    Any = 0,
    Today = 1,
    ThisWeek = 2,
    ThisMonth = 3,
    Overdue = 4
};

enum class TaskSearchSort {
    Manual = 0,
    CreatedDesc = 1,
    DueDateAsc = 2,
    DueDateDesc = 3,
    PriorityDesc = 4,
    PriorityAsc = 5
};

struct TaskSearchFilters {
    QString text;
    int priority = 0;
    TaskSearchStatusFilter status = TaskSearchStatusFilter::Any;
    TaskSearchDateFilter date = TaskSearchDateFilter::Any;
    TaskSearchSort sort = TaskSearchSort::Manual;
    QList<int> tagIds;

    bool hasActiveFilters() const
    {
        return !text.trimmed().isEmpty()
            || priority > 0
            || status != TaskSearchStatusFilter::Any
            || date != TaskSearchDateFilter::Any
            || !tagIds.isEmpty()
            || sort != TaskSearchSort::Manual;
    }
};

#endif // TASK_SEARCH_FILTERS_H
