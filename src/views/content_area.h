#ifndef CONTENT_AREA_H
#define CONTENT_AREA_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSplitter>
#include <QMenu>
#include <QAction>
#include <QPushButton>
#include <QSizePolicy>
#include "../models/task.h"
#include "../models/task_search_filters.h"

class TaskTree;
class TaskController;
class TaskDetailWidget;
class SearchWidget;
class EmptyStateWidget;

class ContentArea : public QWidget
{
    Q_OBJECT

public:
    explicit ContentArea(QWidget *parent = nullptr);
    ~ContentArea();

    void setCurrentGroup(const QString &group);
    QString getCurrentGroup() const;
    void loadTasks();
    void setSearchText(const QString &text);

signals:
    void tagsChanged();

public slots:
    void onTagSelected(int tagId, const QString &tagName);

private slots:
    void onTaskSelected(int taskId);
    void onTaskDoubleClicked(int taskId);
    void onContextMenuRequested(const QPoint &pos, int taskId);
    void onDetailCollapseRequested();
    void onTaskUpdated(const Task &task);
    void onTaskCompletionChanged(int taskId, bool completed);
    void onTaskDeleted(int taskId);
    void onClearTagFilter();
    void onTaskCountChanged(int count);
    void onSearchFiltersChanged(const TaskSearchFilters &filters);

private:
    void setupUI();
    void showTaskDetailPanel();
    void collapseTaskDetailPanel();
    void updateDetailForTask(int taskId);
    void updateTagFilterDisplay();
    void updateEmptyState(int count);

    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_headerLayout;
    QLabel *m_groupLabel;
    QWidget *m_tagFilterWidget;
    QLabel *m_tagFilterLabel;
    QPushButton *m_tagClearButton;
    SearchWidget *m_searchWidget;
    EmptyStateWidget *m_emptyStateWidget;
    QSplitter *m_splitter;
    TaskTree *m_taskTree;
    TaskDetailWidget *m_taskDetailWidget;
    TaskController *m_controller;
    QString m_currentGroup;
    int m_currentTaskId;
    int m_currentTagId;
    QString m_currentTagName;
    TaskSearchFilters m_searchFilters;
};

#endif 
