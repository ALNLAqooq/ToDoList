#ifndef TASK_CARD_WIDGET_H
#define TASK_CARD_WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include "../models/task.h"
#include "../controllers/task_controller.h"

class TaskCardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TaskCardWidget(const Task &task, TaskController *controller, QWidget *parent = nullptr);
    ~TaskCardWidget();

    int taskId() const { return m_taskId; }
    void updateTask(const Task &task);
    void setCompleted(bool completed);

private slots:
    void onCheckBoxChanged(int state);
    void onEditClicked();
    void onDeleteClicked();

private:
    void setupUI();
    void updatePriorityIndicator();
    void updateDueDateDisplay();
    void updateProgressDisplay();

    TaskController *m_controller;
    int m_taskId;
    QString m_title;
    QString m_description;
    Task::Priority m_priority;
    QDateTime m_dueDate;
    bool m_completed;
    double m_progress;

    QCheckBox *m_checkBox;
    QLabel *m_titleLabel;
    QLabel *m_descriptionLabel;
    QLabel *m_priorityLabel;
    QLabel *m_dueDateLabel;
    QLabel *m_progressLabel;
    QPushButton *m_editButton;
    QPushButton *m_deleteButton;
};

#endif // TASK_CARD_WIDGET_H
