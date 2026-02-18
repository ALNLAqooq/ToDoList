#ifndef TASK_DETAIL_WIDGET_H
#define TASK_DETAIL_WIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include "../models/task.h"

class TaskController;

class TaskDetailWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TaskDetailWidget(TaskController *controller, QWidget *parent = nullptr);
    ~TaskDetailWidget();

    void setTask(const Task &task);
    void clearTask();

signals:
    void collapseRequested();

private:
    void setupUI();
    void updateDisplay();

    TaskController *m_controller;
    QVBoxLayout *m_mainLayout;
    QLabel *m_headerLabel;
    QPushButton *m_collapseButton;
    QLabel *m_titleLabel;
    QLabel *m_descriptionLabel;
    QLabel *m_tagTitleLabel;
    QLabel *m_tagValueLabel;
    QLabel *m_subtaskTitleLabel;
    QListWidget *m_subtaskList;
    QLabel *m_priorityLabel;
    QLabel *m_deadlineLabel;
    QLabel *m_progressLabel;
    QLabel *m_statusLabel;
    QLabel *m_placeholderLabel;

    Task m_currentTask;
};

#endif // TASK_DETAIL_WIDGET_H
