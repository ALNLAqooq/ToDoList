#ifndef TASK_DIALOG_H
#define TASK_DIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QSlider>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QCheckBox>
#include "../models/task.h"
#include "../models/tag.h"
#include "../controllers/task_controller.h"

class TaskDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TaskDialog(TaskController *controller, int taskId = -1, QWidget *parent = nullptr);
    ~TaskDialog();

    int getTaskId() const { return m_taskId; }
    void setParentTaskId(int parentId) { m_parentId = parentId; }
    Task getTask() const;

private slots:
    void onSaveClicked();
    void onCancelClicked();
    void onBrowseFileClicked();
    void onRemoveFileClicked();
    void onOpenFileClicked();
    void onAddTagClicked();
    void onRemoveTagClicked();
    void onAddStepClicked();
    void onRemoveStepClicked();
    void onProgressChanged(int value);
    void onTagSelectionChanged();
    void onAddDependencyClicked();
    void onRemoveDependencyClicked();

private:
    void setupUI();
    void setupBasicInfoTab();
    void setupDetailsTab();
    void setupDependenciesTab();
    void loadTaskData();
    void loadTags();
    void loadTasksForDependency();
    void loadFileList();
    void loadSubtasks();
    void updateProgressLabel(int value);
    void deleteSubtaskTree(int taskId);

    TaskController *m_controller;
    int m_taskId;
    int m_parentId;
    Task m_currentTask;

    QTabWidget *m_tabWidget;
    QWidget *m_basicInfoTab;
    QWidget *m_detailsTab;
    QWidget *m_dependenciesTab;

    QLineEdit *m_titleEdit;
    QTextEdit *m_descriptionEdit;
    QComboBox *m_priorityCombo;
    QDateTimeEdit *m_deadlineEdit;
    QSlider *m_progressSlider;
    QLabel *m_progressLabel;

    QListWidget *m_tagList;
    QComboBox *m_tagCombo;
    QPushButton *m_addTagButton;
    QPushButton *m_removeTagButton;

    QListWidget *m_fileList;
    QPushButton *m_browseFileButton;
    QPushButton *m_removeFileButton;
    QPushButton *m_openFileButton;

    QListWidget *m_stepList;
    QLineEdit *m_stepEdit;
    QPushButton *m_addStepButton;
    QPushButton *m_removeStepButton;

    QListWidget *m_dependencyList;
    QComboBox *m_taskCombo;
    QPushButton *m_addDependencyButton;
    QPushButton *m_removeDependencyButton;

    QList<int> m_selectedTagIds;
    QList<int> m_selectedDependencyIds;
    QList<QString> m_filePaths;
    QList<int> m_deletedSubtaskIds;
};

#endif // TASK_DIALOG_H
