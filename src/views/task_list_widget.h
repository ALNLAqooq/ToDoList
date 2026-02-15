#ifndef TASK_LIST_WIDGET_H
#define TASK_LIST_WIDGET_H

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QLayout>
#include "../controllers/task_controller.h"

class TaskCardWidget;
class QLabel;
class QLineEdit;
class QPushButton;

class TaskListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TaskListWidget(TaskController *controller, QWidget *parent = nullptr);
    ~TaskListWidget();

public slots:
    void refreshTasks();
    void onTaskAdded(const Task &task);
    void onTaskUpdated(const Task &task);
    void onTaskDeleted(int taskId);
    void onTaskCompletionChanged(int taskId, bool completed);

private:
    void setupUI();
    QLayout* setupFilterBar();
    void setupTaskList();
    void filterTasks();
    TaskCardWidget* findTaskCard(int taskId);

    TaskController *m_controller;
    QScrollArea *m_scrollArea;
    QWidget *m_taskListContainer;
    QVBoxLayout *m_taskListLayout;
    QLineEdit *m_searchEdit;
    QPushButton *m_filterButton;
    QList<Task> m_allTasks;
    QList<Task> m_filteredTasks;
    QList<TaskCardWidget*> m_taskCards;
};

#endif // TASK_LIST_WIDGET_H
