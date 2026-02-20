#include "task_dialog.h"
#include "../controllers/database.h"
#include "../utils/file_utils.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QScrollBar>
#include <QHash>
#include <QSet>
#include <QColor>
#include <QSize>

TaskDialog::TaskDialog(TaskController *controller, int taskId, QWidget *parent)
    : QDialog(parent)
    , m_controller(controller)
    , m_taskId(taskId)
    , m_parentId(0)
    , m_folderId(0)
{
    setupUI();
    if (m_taskId != -1) {
        loadTaskData();
    }
    loadTags();
    loadTasksForDependency();
}

TaskDialog::~TaskDialog()
{
}

void TaskDialog::setupUI()
{
    setWindowTitle(m_taskId == -1 ? "新建任务" : "编辑任务");
    setMinimumSize(600, 700);
    setModal(true);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    m_tabWidget = new QTabWidget(this);
    m_basicInfoTab = new QWidget();
    m_detailsTab = new QWidget();
    m_dependenciesTab = new QWidget();

    setupBasicInfoTab();
    setupDetailsTab();
    setupDependenciesTab();

    m_tabWidget->addTab(m_basicInfoTab, "基本信息");
    m_tabWidget->addTab(m_detailsTab, "详细信息");
    m_tabWidget->addTab(m_dependenciesTab, "依赖关系");

    mainLayout->addWidget(m_tabWidget);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    QPushButton *saveButton = new QPushButton("保存", this);
    saveButton->setStyleSheet(R"(
        QPushButton {
            background-color: #3B82F6;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 8px 24px;
            font-size: 14px;
        }
        QPushButton:hover {
            background-color: #2563EB;
        }
    )");

    QPushButton *cancelButton = new QPushButton("取消", this);
    cancelButton->setStyleSheet(R"(
        QPushButton {
            background-color: #F3F4F6;
            color: #4B5563;
            border: none;
            border-radius: 4px;
            padding: 8px 24px;
            font-size: 14px;
        }
        QPushButton:hover {
            background-color: #E5E7EB;
        }
    )");

    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(buttonLayout);

    connect(saveButton, &QPushButton::clicked, this, &TaskDialog::onSaveClicked);
    connect(cancelButton, &QPushButton::clicked, this, &TaskDialog::onCancelClicked);
}

void TaskDialog::setupBasicInfoTab()
{
    QVBoxLayout *layout = new QVBoxLayout(m_basicInfoTab);
    layout->setSpacing(16);

    QLabel *titleLabel = new QLabel("标题:", m_basicInfoTab);
    titleLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    m_titleEdit = new QLineEdit(m_basicInfoTab);
    m_titleEdit->setPlaceholderText("请输入任务标题");
    m_titleEdit->setStyleSheet(R"(
        QLineEdit {
            border: 1px solid #D1D5DB;
            border-radius: 4px;
            padding: 8px;
            font-size: 14px;
        }
        QLineEdit:focus {
            border: 1px solid #3B82F6;
        }
    )");

    QLabel *descriptionLabel = new QLabel("描述:", m_basicInfoTab);
    descriptionLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    m_descriptionEdit = new QTextEdit(m_basicInfoTab);
    m_descriptionEdit->setPlaceholderText("请输入任务描述");
    m_descriptionEdit->setMaximumHeight(150);
    m_descriptionEdit->setStyleSheet(R"(
        QTextEdit {
            border: 1px solid #D1D5DB;
            border-radius: 4px;
            padding: 8px;
            font-size: 14px;
        }
        QTextEdit:focus {
            border: 1px solid #3B82F6;
        }
    )");

    QLabel *priorityLabel = new QLabel("优先级:", m_basicInfoTab);
    priorityLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    m_priorityCombo = new QComboBox(m_basicInfoTab);
    m_priorityCombo->addItem("低", Task::Low);
    m_priorityCombo->addItem("中", Task::Medium);
    m_priorityCombo->addItem("高", Task::High);
    m_priorityCombo->setStyleSheet(R"(
        QComboBox {
            border: 1px solid #D1D5DB;
            border-radius: 4px;
            padding: 8px;
            font-size: 14px;
        }
        QComboBox:focus {
            border: 1px solid #3B82F6;
        }
        QComboBox::drop-down {
            border: none;
            padding-right: 10px;
        }
    )");

    QLabel *deadlineLabel = new QLabel("截止日期:", m_basicInfoTab);
    deadlineLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    m_deadlineEdit = new QDateTimeEdit(m_basicInfoTab);
    m_deadlineEdit->setDateTime(QDateTime::currentDateTime().addDays(7));
    m_deadlineEdit->setCalendarPopup(true);
    m_deadlineEdit->setDisplayFormat("yyyy-MM-dd HH:mm");
    m_deadlineEdit->setStyleSheet(R"(
        QDateTimeEdit {
            border: 1px solid #D1D5DB;
            border-radius: 4px;
            padding: 8px;
            font-size: 14px;
        }
        QDateTimeEdit:focus {
            border: 1px solid #3B82F6;
        }
    )");

    layout->addWidget(titleLabel);
    layout->addWidget(m_titleEdit);
    layout->addWidget(descriptionLabel);
    layout->addWidget(m_descriptionEdit);
    layout->addWidget(priorityLabel);
    layout->addWidget(m_priorityCombo);
    layout->addWidget(deadlineLabel);
    layout->addWidget(m_deadlineEdit);
    layout->addStretch();
}

void TaskDialog::setupDetailsTab()
{
    QVBoxLayout *layout = new QVBoxLayout(m_detailsTab);
    layout->setSpacing(16);

    QLabel *progressLabel = new QLabel("进度:", m_detailsTab);
    progressLabel->setStyleSheet("font-weight: bold; font-size: 14px;");

    QHBoxLayout *progressLayout = new QHBoxLayout();
    m_progressSlider = new QSlider(Qt::Horizontal, m_detailsTab);
    m_progressSlider->setRange(0, 100);
    m_progressSlider->setValue(0);
    m_progressSlider->setStyleSheet(R"(
        QSlider::groove:horizontal {
            height: 8px;
            background: #E5E7EB;
            border-radius: 4px;
        }
        QSlider::handle:horizontal {
            background: #3B82F6;
            width: 18px;
            margin: -5px 0;
            border-radius: 9px;
        }
        QSlider::handle:horizontal:hover {
            background: #2563EB;
        }
    )");

    m_progressLabel = new QLabel("0%", m_detailsTab);
    m_progressLabel->setMinimumWidth(50);
    m_progressLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #3B82F6;");

    progressLayout->addWidget(m_progressSlider);
    progressLayout->addWidget(m_progressLabel);

    QLabel *tagsLabel = new QLabel("标签:", m_detailsTab);
    tagsLabel->setStyleSheet("font-weight: bold; font-size: 14px;");

    QHBoxLayout *tagLayout = new QHBoxLayout();
    m_tagCombo = new QComboBox(m_detailsTab);
    m_tagCombo->setPlaceholderText("选择标签");
    m_tagCombo->setEditable(true);
    m_tagCombo->setInsertPolicy(QComboBox::NoInsert);
    m_tagCombo->setStyleSheet(R"(
        QComboBox {
            border: 1px solid #D1D5DB;
            border-radius: 4px;
            padding: 8px;
            font-size: 14px;
        }
    )");

    m_addTagButton = new QPushButton("添加", m_detailsTab);
    m_addTagButton->setStyleSheet(R"(
        QPushButton {
            background-color: #3B82F6;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 6px 16px;
        }
        QPushButton:hover {
            background-color: #2563EB;
        }
    )");

    m_removeTagButton = new QPushButton("移除", m_detailsTab);
    m_removeTagButton->setStyleSheet(R"(
        QPushButton {
            background-color: #F3F4F6;
            color: #4B5563;
            border: none;
            border-radius: 4px;
            padding: 6px 16px;
        }
        QPushButton:hover {
            background-color: #E5E7EB;
        }
    )");

    tagLayout->addWidget(m_tagCombo);
    tagLayout->addWidget(m_addTagButton);
    tagLayout->addWidget(m_removeTagButton);

    m_tagList = new QListWidget(m_detailsTab);
    m_tagList->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tagList->setMaximumHeight(100);
    m_tagList->setStyleSheet(R"(
        QListWidget {
            border: 1px solid #D1D5DB;
            border-radius: 4px;
            background-color: white;
        }
        QListWidget::item:selected {
            background-color: #3B82F6;
        }
    )");

    QLabel *filesLabel = new QLabel("附件:", m_detailsTab);
    filesLabel->setStyleSheet("font-weight: bold; font-size: 14px;");

    QHBoxLayout *fileButtonLayout = new QHBoxLayout();
    m_browseFileButton = new QPushButton("浏览...", m_detailsTab);
    m_browseFileButton->setStyleSheet(R"(
        QPushButton {
            background-color: #3B82F6;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 6px 16px;
        }
        QPushButton:hover {
            background-color: #2563EB;
        }
    )");

    m_removeFileButton = new QPushButton("移除", m_detailsTab);
    m_removeFileButton->setStyleSheet(R"(
        QPushButton {
            background-color: #F3F4F6;
            color: #4B5563;
            border: none;
            border-radius: 4px;
            padding: 6px 16px;
        }
        QPushButton:hover {
            background-color: #E5E7EB;
        }
    )");

    m_openFileButton = new QPushButton("打开", m_detailsTab);
    m_openFileButton->setStyleSheet(R"(
        QPushButton {
            background-color: #10B981;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 6px 16px;
        }
        QPushButton:hover {
            background-color: #059669;
        }
    )");

    fileButtonLayout->addWidget(m_browseFileButton);
    fileButtonLayout->addWidget(m_removeFileButton);
    fileButtonLayout->addWidget(m_openFileButton);
    fileButtonLayout->addStretch();

    m_fileList = new QListWidget(m_detailsTab);
    m_fileList->setMaximumHeight(100);
    m_fileList->setIconSize(QSize(18, 18));
    m_fileList->setStyleSheet(R"(
        QListWidget {
            border: 1px solid #D1D5DB;
            border-radius: 4px;
            background-color: white;
        }
    )");

    QLabel *stepsLabel = new QLabel("子任务:", m_detailsTab);
    stepsLabel->setStyleSheet("font-weight: bold; font-size: 14px;");

    QHBoxLayout *stepButtonLayout = new QHBoxLayout();
    m_stepEdit = new QLineEdit(m_detailsTab);
    m_stepEdit->setPlaceholderText("输入子任务");

    m_addStepButton = new QPushButton("添加", m_detailsTab);
    m_addStepButton->setStyleSheet(R"(
        QPushButton {
            background-color: #3B82F6;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 6px 16px;
        }
        QPushButton:hover {
            background-color: #2563EB;
        }
    )");

    m_removeStepButton = new QPushButton("移除", m_detailsTab);
    m_removeStepButton->setStyleSheet(R"(
        QPushButton {
            background-color: #F3F4F6;
            color: #4B5563;
            border: none;
            border-radius: 4px;
            padding: 6px 16px;
        }
        QPushButton:hover {
            background-color: #E5E7EB;
        }
    )");

    stepButtonLayout->addWidget(m_stepEdit);
    stepButtonLayout->addWidget(m_addStepButton);
    stepButtonLayout->addWidget(m_removeStepButton);

    m_stepList = new QListWidget(m_detailsTab);
    m_stepList->setMaximumHeight(100);
    m_stepList->setStyleSheet(R"(
        QListWidget {
            border: 1px solid #D1D5DB;
            border-radius: 4px;
            background-color: white;
        }
    )");

    layout->addWidget(progressLabel);
    layout->addLayout(progressLayout);
    layout->addWidget(tagsLabel);
    layout->addLayout(tagLayout);
    layout->addWidget(m_tagList);
    layout->addWidget(filesLabel);
    layout->addLayout(fileButtonLayout);
    layout->addWidget(m_fileList);
    layout->addWidget(stepsLabel);
    layout->addLayout(stepButtonLayout);
    layout->addWidget(m_stepList);
    layout->addStretch();

    connect(m_progressSlider, &QSlider::valueChanged, this, &TaskDialog::onProgressChanged);
    connect(m_addTagButton, &QPushButton::clicked, this, &TaskDialog::onAddTagClicked);
    connect(m_removeTagButton, &QPushButton::clicked, this, &TaskDialog::onRemoveTagClicked);
    connect(m_browseFileButton, &QPushButton::clicked, this, &TaskDialog::onBrowseFileClicked);
    connect(m_removeFileButton, &QPushButton::clicked, this, &TaskDialog::onRemoveFileClicked);
    connect(m_openFileButton, &QPushButton::clicked, this, &TaskDialog::onOpenFileClicked);
    connect(m_addStepButton, &QPushButton::clicked, this, &TaskDialog::onAddStepClicked);
    connect(m_removeStepButton, &QPushButton::clicked, this, &TaskDialog::onRemoveStepClicked);
}

void TaskDialog::setupDependenciesTab()
{
    QVBoxLayout *layout = new QVBoxLayout(m_dependenciesTab);
    layout->setSpacing(16);

    QLabel *dependencyLabel = new QLabel("任务依赖:", m_dependenciesTab);
    dependencyLabel->setStyleSheet("font-weight: bold; font-size: 14px;");

    QHBoxLayout *dependencyButtonLayout = new QHBoxLayout();
    m_taskCombo = new QComboBox(m_dependenciesTab);
    m_taskCombo->setPlaceholderText("选择依赖的任务");
    m_taskCombo->setStyleSheet(R"(
        QComboBox {
            border: 1px solid #D1D5DB;
            border-radius: 4px;
            padding: 8px;
            font-size: 14px;
        }
    )");

    m_addDependencyButton = new QPushButton("添加依赖", m_dependenciesTab);
    m_addDependencyButton->setStyleSheet(R"(
        QPushButton {
            background-color: #3B82F6;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 6px 16px;
        }
        QPushButton:hover {
            background-color: #2563EB;
        }
    )");

    m_removeDependencyButton = new QPushButton("移除依赖", m_dependenciesTab);
    m_removeDependencyButton->setStyleSheet(R"(
        QPushButton {
            background-color: #F3F4F6;
            color: #4B5563;
            border: none;
            border-radius: 4px;
            padding: 6px 16px;
        }
        QPushButton:hover {
            background-color: #E5E7EB;
        }
    )");

    dependencyButtonLayout->addWidget(m_taskCombo);
    dependencyButtonLayout->addWidget(m_addDependencyButton);
    dependencyButtonLayout->addWidget(m_removeDependencyButton);

    m_dependencyList = new QListWidget(m_dependenciesTab);
    m_dependencyList->setSelectionMode(QAbstractItemView::SingleSelection);
    m_dependencyList->setStyleSheet(R"(
        QListWidget {
            border: 1px solid #D1D5DB;
            border-radius: 4px;
            background-color: white;
        }
        QListWidget::item:selected {
            background-color: #3B82F6;
        }
    )");

    layout->addWidget(dependencyLabel);
    layout->addLayout(dependencyButtonLayout);
    layout->addWidget(m_dependencyList);
    layout->addStretch();

    connect(m_addDependencyButton, &QPushButton::clicked, this, &TaskDialog::onAddDependencyClicked);
    connect(m_removeDependencyButton, &QPushButton::clicked, this, &TaskDialog::onRemoveDependencyClicked);
}

void TaskDialog::loadTaskData()
{
    m_currentTask = m_controller->getTaskById(m_taskId);
    m_parentId = m_currentTask.parentId();

    m_titleEdit->setText(m_currentTask.title());
    m_descriptionEdit->setText(m_currentTask.description());

    int priorityIndex = m_priorityCombo->findData(m_currentTask.priority());
    if (priorityIndex != -1) {
        m_priorityCombo->setCurrentIndex(priorityIndex);
    }

    if (m_currentTask.dueDate().isValid()) {
        m_deadlineEdit->setDateTime(m_currentTask.dueDate());
    }

    m_progressSlider->setValue(static_cast<int>(m_currentTask.progress() * 100));

    m_selectedTagIds = m_currentTask.tagIds();
    m_filePaths = m_currentTask.filePaths();
    m_selectedDependencyIds = m_controller->getDependencyIdsForTask(m_taskId);

    loadTags();
    loadFileList();
    loadTasksForDependency();
    loadSubtasks();
}

void TaskDialog::loadTags()
{
    m_tagCombo->clear();
    QList<Tag> tags = m_controller->getAllTags();
    for (const Tag &tag : tags) {
        m_tagCombo->addItem(tag.name(), tag.id());
    }

    m_tagList->clear();
    for (int tagId : m_selectedTagIds) {
        for (int i = 0; i < m_tagCombo->count(); ++i) {
            if (m_tagCombo->itemData(i).toInt() == tagId) {
                m_tagList->addItem(m_tagCombo->itemText(i));
                break;
            }
        }
    }
}

void TaskDialog::loadTasksForDependency()
{
    m_taskCombo->clear();
    m_dependencyList->clear();

    QList<Task> tasks = m_controller->getAllTasks();
    QHash<int, QString> taskTitles;
    QSet<int> circularIds;

    if (m_taskId > 0) {
        QList<Task> circularTasks = m_controller->getCircularDependencies(m_taskId);
        for (const Task &task : circularTasks) {
            circularIds.insert(task.id());
        }
    }

    for (const Task &task : tasks) {
        taskTitles.insert(task.id(), task.title());
        if (task.id() == m_taskId) {
            continue;
        }

        if (m_taskId > 0 && m_controller->wouldCreateCircularDependency(m_taskId, task.id())) {
            continue;
        }

        m_taskCombo->addItem(task.title(), task.id());
    }

    for (int depId : m_selectedDependencyIds) {
    QString title = taskTitles.value(depId, QString("任务 %1").arg(depId));
        if (circularIds.contains(depId)) {
        title += "（循环）";
        }
        m_dependencyList->addItem(title);
    }
}

void TaskDialog::loadFileList()
{
    m_fileList->clear();
    for (const QString &filePath : m_filePaths) {
        QFileInfo fileInfo(filePath);
        QString fileName = fileInfo.fileName();
        if (fileName.isEmpty()) {
            fileName = filePath;
        }

        auto *item = new QListWidgetItem(fileName);
        item->setData(Qt::UserRole, filePath);
        item->setIcon(QIcon(FileUtils::getFileIconPath(filePath)));

        if (!fileInfo.exists()) {
            item->setForeground(QColor("#DC2626"));
        item->setText(fileName + "（缺失）");
        item->setToolTip(QString("缺失文件：\n%1").arg(filePath));
        } else {
            QString typeLabel = FileUtils::getFileType(filePath);
            QString sizeLabel = FileUtils::fileSizeFormatted(fileInfo.size());
            item->setToolTip(QString("%1\n%2 · %3").arg(filePath, typeLabel, sizeLabel));
        }

        m_fileList->addItem(item);
    }
}

void TaskDialog::loadSubtasks()
{
    m_stepList->clear();
    m_deletedSubtaskIds.clear();

    if (m_taskId <= 0) {
        return;
    }

    QList<Task> subtasks = m_controller->getSubTasks(m_taskId);
    for (const Task &task : subtasks) {
        auto *item = new QListWidgetItem(task.title());
        item->setData(Qt::UserRole, task.id());
        m_stepList->addItem(item);
    }
}

void TaskDialog::deleteSubtaskTree(int taskId)
{
    QList<Task> children = m_controller->getSubTasks(taskId);
    for (const Task &child : children) {
        deleteSubtaskTree(child.id());
    }
    m_controller->deleteTask(taskId);
}

void TaskDialog::updateProgressLabel(int value)
{
    m_progressLabel->setText(QString("%1%").arg(value));
}

Task TaskDialog::getTask() const
{
    Task task;
    task.setId(m_taskId);
    task.setParentId(m_parentId);
    task.setTitle(m_titleEdit->text());
    task.setDescription(m_descriptionEdit->toPlainText());
    task.setPriority(m_priorityCombo->currentData().toInt());
    task.setDueDate(m_deadlineEdit->dateTime());
    task.setProgress(m_progressSlider->value() / 100.0);
    task.setTagIds(m_selectedTagIds);
    task.setFilePaths(m_filePaths);
    task.setDependencyIds(m_selectedDependencyIds);
    return task;
}

void TaskDialog::onSaveClicked()
{
    if (m_titleEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "警告", "任务标题不能为空");
        return;
    }

    
    QStringList missingFiles;
    for (const QString &filePath : m_filePaths) {
        if (!FileUtils::exists(filePath)) {
            missingFiles.append(filePath);
        }
    }

    if (!missingFiles.isEmpty()) {
        QMessageBox dialog(this);
        dialog.setIcon(QMessageBox::Warning);
        dialog.setWindowTitle("文件缺失");
        dialog.setText("部分附件已丢失。");
        dialog.setInformativeText(missingFiles.join("\n"));
        QPushButton *keepButton = dialog.addButton("保留", QMessageBox::AcceptRole);
        QPushButton *removeButton = dialog.addButton("移除缺失项", QMessageBox::DestructiveRole);
        QPushButton *cancelButton = dialog.addButton("取消", QMessageBox::RejectRole);
        dialog.setDefaultButton(keepButton);
        dialog.exec();

        if (dialog.clickedButton() == cancelButton) {
            return;
        }

        if (dialog.clickedButton() == removeButton) {
            QSet<QString> missingSet(missingFiles.begin(), missingFiles.end());
            QList<QString> filtered;
            for (const QString &filePath : m_filePaths) {
                if (!missingSet.contains(filePath)) {
                    filtered.append(filePath);
                }
            }
            m_filePaths = filtered;
            loadFileList();
        }
    }

    Task task = getTask();
    bool isNewTask = (m_taskId == -1);

    if (isNewTask) {
        if (!m_controller->addTask(task)) {
            QMessageBox::critical(this, "错误", "任务创建失败");
            return;
        }
        m_taskId = task.id();
    } else {
        if (!m_controller->updateTask(task)) {
            QMessageBox::critical(this, "错误", "任务更新失败");
            return;
        }
    }

    if (m_folderId > 0 && m_taskId > 0) {
        Database::instance().assignTaskToFolder(m_taskId, m_folderId);
    }

    Database &db = Database::instance();

    QSet<int> keptSubtaskIds;
    for (int i = 0; i < m_stepList->count(); ++i) {
        QListWidgetItem *item = m_stepList->item(i);
        QString title = item->text().trimmed();
        if (title.isEmpty()) {
            continue;
        }

        int subtaskId = item->data(Qt::UserRole).toInt();
        if (subtaskId > 0) {
            Task subtask = m_controller->getTaskById(subtaskId);
            if (subtask.id() > 0) {
                subtask.setTitle(title);
                subtask.setParentId(m_taskId);
                m_controller->updateTask(subtask);
                if (m_folderId > 0) {
                    Database::instance().assignTaskToFolder(subtask.id(), m_folderId);
                }
                keptSubtaskIds.insert(subtaskId);
                continue;
            }
        }

        Task newSubtask;
        newSubtask.setTitle(title);
        newSubtask.setParentId(m_taskId);
        m_controller->addTask(newSubtask);
        if (m_folderId > 0 && newSubtask.id() > 0) {
            Database::instance().assignTaskToFolder(newSubtask.id(), m_folderId);
        }
    }

    for (int deletedId : m_deletedSubtaskIds) {
        if (!keptSubtaskIds.contains(deletedId)) {
            deleteSubtaskTree(deletedId);
        }
    }

    db.removeAllTagsFromTask(m_taskId);
    for (int tagId : m_selectedTagIds) {
        db.assignTagToTask(m_taskId, tagId);
    }

    db.removeAllFilesFromTask(m_taskId);
    for (const QString &filePath : m_filePaths) {
        QFileInfo fileInfo(filePath);
        db.addFileToTask(m_taskId, filePath, fileInfo.fileName());
    }

    db.removeAllDependenciesFromTask(m_taskId);
    for (int depId : m_selectedDependencyIds) {
        db.addDependency(m_taskId, depId);
    }

    m_controller->updateProgress(m_taskId);

    QMessageBox::information(this, "成功", isNewTask ? "任务创建成功" : "任务更新成功");
    accept();
}

void TaskDialog::onCancelClicked()
{
    reject();
}

void TaskDialog::onBrowseFileClicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "选择文件", "", "所有文件 (*)");
    if (!filePath.isEmpty()) {
        QString normalizedPath = FileUtils::normalizePath(filePath);
        if (!FileUtils::exists(normalizedPath) || !FileUtils::isFile(normalizedPath)) {
            QMessageBox::warning(this, "警告", "所选文件不存在。");
            return;
        }

        if (!m_filePaths.contains(normalizedPath)) {
            m_filePaths.append(normalizedPath);
            loadFileList();
        } else {
            QMessageBox::warning(this, "警告", "文件已添加。");
        }
    }
}


void TaskDialog::onRemoveFileClicked()
{
    QListWidgetItem *item = m_fileList->currentItem();
    if (item) {
        QString filePath = item->data(Qt::UserRole).toString();
        int index = m_filePaths.indexOf(filePath);
        if (index >= 0) {
            m_filePaths.removeAt(index);
        }
        loadFileList();
    }
}


void TaskDialog::onOpenFileClicked()
{
    QListWidgetItem *item = m_fileList->currentItem();
    if (item) {
        QString filePath = item->data(Qt::UserRole).toString();
        QFileInfo fileInfo(filePath);
        if (fileInfo.exists()) {
            if (!QDesktopServices::openUrl(QUrl::fromLocalFile(filePath))) {
                QMessageBox::warning(this, "错误", "无法打开文件: " + filePath);
            }
        } else {
            QMessageBox::warning(this, "警告", "文件不存在: " + filePath);
        }
    }
}


void TaskDialog::onAddTagClicked()
{
    QString tagName = m_tagCombo->currentText().trimmed();
    if (tagName.isEmpty()) {
        return;
    }

    int tagIndex = m_tagCombo->findText(tagName);
    int tagId = -1;
    if (tagIndex >= 0) {
        tagId = m_tagCombo->itemData(tagIndex).toInt();
    } else {
        Tag newTag;
        newTag.setName(tagName);
        if (!m_controller->addTag(newTag)) {
            QMessageBox::warning(this, "警告", "标签创建失败");
            return;
        }
        loadTags();
        tagIndex = m_tagCombo->findText(tagName);
        if (tagIndex >= 0) {
            tagId = m_tagCombo->itemData(tagIndex).toInt();
        }
    }

    if (tagId > 0 && !m_selectedTagIds.contains(tagId)) {
        m_selectedTagIds.append(tagId);
        m_tagList->addItem(tagName);
    }
}

void TaskDialog::onRemoveTagClicked()
{
    int currentRow = m_tagList->currentRow();
    if (currentRow >= 0 && currentRow < m_selectedTagIds.size()) {
        m_selectedTagIds.removeAt(currentRow);
        delete m_tagList->takeItem(currentRow);
    }
}

void TaskDialog::onAddStepClicked()
{
    QString step = m_stepEdit->text().trimmed();
    if (!step.isEmpty()) {
        auto *item = new QListWidgetItem(step);
        item->setData(Qt::UserRole, 0);
        m_stepList->addItem(item);
        m_stepEdit->clear();
    }
}

void TaskDialog::onRemoveStepClicked()
{
    int currentRow = m_stepList->currentRow();
    if (currentRow >= 0) {
        QListWidgetItem *item = m_stepList->takeItem(currentRow);
        if (item) {
            int subtaskId = item->data(Qt::UserRole).toInt();
            if (subtaskId > 0 && !m_deletedSubtaskIds.contains(subtaskId)) {
                m_deletedSubtaskIds.append(subtaskId);
            }
            delete item;
        }
    }
}

void TaskDialog::onProgressChanged(int value)
{
    updateProgressLabel(value);
}

void TaskDialog::onTagSelectionChanged()
{
}

void TaskDialog::onAddDependencyClicked()
{
    int depId = m_taskCombo->currentData().toInt();
    QString depName = m_taskCombo->currentText();

    if (depId <= 0) {
        return;
    }

    if (m_taskId > 0 && m_controller->wouldCreateCircularDependency(m_taskId, depId)) {
        QMessageBox::warning(this, "警告", "依赖会造成循环引用。");
        return;
    }

    if (!m_selectedDependencyIds.contains(depId)) {
        m_selectedDependencyIds.append(depId);
        m_dependencyList->addItem(depName);
    }
}

void TaskDialog::onRemoveDependencyClicked()
{
    int currentRow = m_dependencyList->currentRow();
    if (currentRow >= 0 && currentRow < m_selectedDependencyIds.size()) {
        m_selectedDependencyIds.removeAt(currentRow);
        delete m_dependencyList->takeItem(currentRow);
    }
}
