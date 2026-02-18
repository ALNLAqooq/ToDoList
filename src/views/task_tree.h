#ifndef TASK_TREE_H
#define TASK_TREE_H

#include <QWidget>
#include <QTreeView>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QMenu>
#include <QAction>
#include <QSet>
#include <QStyledItemDelegate>
#include "../models/task.h"
#include "../controllers/task_controller.h"

class TaskTreeItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit TaskTreeItemDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

signals:
    void toggleRequested(int taskId);

private:
    QRect checkboxRect(const QStyleOptionViewItem &option) const;
};

class TaskTree : public QWidget
{
    Q_OBJECT

public:
    explicit TaskTree(TaskController *controller, QWidget *parent = nullptr);
    ~TaskTree();

    void loadTasks();
    void loadTasks(const QString &group);
    void loadTasks(const QString &group, int tagId);
    void refreshTasks();
    void expandAll();
    void collapseAll();
    void clearSelection();

signals:
    void taskSelected(int taskId);
    void taskDoubleClicked(int taskId);
    void contextMenuRequested(const QPoint &pos, int taskId);
    void taskCountChanged(int count);

private slots:
    void onItemDoubleClicked(const QModelIndex &index);
    void onItemClicked(const QModelIndex &index);
    void onContextMenu(const QPoint &pos);
    void onExpandItem(const QModelIndex &index);
    void onCollapseItem(const QModelIndex &index);
    void onDropData(const QMimeData *data, const QModelIndex &index);

private:
    void setupUI();
    void setupContextMenu();
    QStandardItem* createTaskItem(const Task &task, const QString &sourceInfo = QString(), const QString &sourceTooltip = QString());
    void loadChildTasks(int parentId, QStandardItem *parentItem);
    void loadAllTasks();
    void loadFilteredTasks(const QString &group, int tagId);
    Task getTaskFromIndex(const QModelIndex &index) const;
    QSet<int> collectExpandedTaskIds() const;
    void restoreExpandedTaskIds(const QSet<int> &ids);
    QModelIndex findIndexByTaskId(int taskId) const;

    TaskController *m_controller;
    QTreeView *m_treeView;
    QStandardItemModel *m_treeModel;
    QMenu *m_contextMenu;
    QAction *m_expandAction;
    QAction *m_collapseAction;
    QAction *m_addSubtaskAction;
    QAction *m_editAction;
    QAction *m_deleteAction;
    QAction *m_completeAction;
    
    QMap<int, Task> m_tasksCache;
    QString m_currentGroup;
    int m_currentTagId;
};

#endif // TASK_TREE_H
