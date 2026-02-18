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

class TaskTree;
class TaskController;
class TaskDetailWidget;

class ContentArea : public QWidget
{
    Q_OBJECT

public:
    explicit ContentArea(QWidget *parent = nullptr);
    ~ContentArea();

    void setCurrentGroup(const QString &group);
    QString getCurrentGroup() const;
    void loadTasks();

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

private:
    void setupUI();
    void showTaskDetailPanel();
    void collapseTaskDetailPanel();
    void updateDetailForTask(int taskId);
    void updateTagFilterDisplay();

    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_headerLayout;
    QLabel *m_groupLabel;
    QWidget *m_tagFilterWidget;
    QLabel *m_tagFilterLabel;
    QPushButton *m_tagClearButton;
    QLabel *m_placeholderLabel;
    QSplitter *m_splitter;
    TaskTree *m_taskTree;
    TaskDetailWidget *m_taskDetailWidget;
    TaskController *m_controller;
    QString m_currentGroup;
    int m_currentTaskId;
    int m_currentTagId;
    QString m_currentTagName;
};

#endif 
