#include "content_area.h"
#include "task_tree.h"
#include "task_detail_widget.h"
#include "task_dialog.h"
#include "search_widget.h"
#include "../controllers/task_controller.h"
#include "../utils/logger.h"
#include <QMessageBox>
#include <QRandomGenerator>

ContentArea::ContentArea(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
    , m_headerLayout(nullptr)
    , m_groupLabel(nullptr)
    , m_tagFilterWidget(nullptr)
    , m_tagFilterLabel(nullptr)
    , m_tagClearButton(nullptr)
    , m_searchWidget(nullptr)
    , m_placeholderLabel(nullptr)
    , m_splitter(nullptr)
    , m_taskTree(nullptr)
    , m_taskDetailWidget(nullptr)
    , m_controller(nullptr)
    , m_currentGroup("所有任务")
    , m_currentTaskId(0)
    , m_currentTagId(0)
    , m_currentTagName("")
    , m_searchFilters()
{
    setupUI();
    LOG_INFO("ContentArea", "Content area widget created");
}

ContentArea::~ContentArea()
{
}

void ContentArea::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(12, 12, 12, 12);
    m_mainLayout->setSpacing(10);

    m_groupLabel = new QLabel("所有任务", this);
    m_groupLabel->setObjectName("groupLabel");
    m_groupLabel->setFixedHeight(28);
    m_groupLabel->setAlignment(Qt::AlignVCenter);

    m_tagFilterWidget = new QWidget(this);
    auto *tagLayout = new QHBoxLayout(m_tagFilterWidget);
    tagLayout->setContentsMargins(8, 4, 8, 4);
    tagLayout->setSpacing(6);

    m_tagFilterLabel = new QLabel(this);
    m_tagFilterLabel->setObjectName("tagFilterLabel");
    m_tagFilterLabel->setProperty("secondary", true);

    m_tagClearButton = new QPushButton("清除", this);
    m_tagClearButton->setFixedHeight(22);
    m_tagClearButton->setObjectName("tagClearButton");
    connect(m_tagClearButton, &QPushButton::clicked, this, &ContentArea::onClearTagFilter);

    tagLayout->addWidget(m_tagFilterLabel);
    tagLayout->addWidget(m_tagClearButton);
    m_tagFilterWidget->setObjectName("tagFilterWidget");
    m_tagFilterWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
    m_tagFilterWidget->hide();

    m_headerLayout = new QHBoxLayout();
    m_headerLayout->addWidget(m_groupLabel);
    m_headerLayout->addSpacing(12);
    m_headerLayout->addWidget(m_tagFilterWidget);
    m_headerLayout->addStretch();

    m_controller = new TaskController(this);
    m_searchWidget = new SearchWidget(m_controller, this);
    connect(m_searchWidget, &SearchWidget::filtersChanged, this, &ContentArea::onSearchFiltersChanged);

    m_placeholderLabel = new QLabel("暂无任务。点击 + 按钮添加新任务。", this);
    m_placeholderLabel->setAlignment(Qt::AlignCenter);
    m_placeholderLabel->setWordWrap(true);
    m_placeholderLabel->setObjectName("contentPlaceholder");
    m_placeholderLabel->hide();

    m_splitter = new QSplitter(Qt::Horizontal, this);
    m_splitter->setHandleWidth(1);

    m_taskTree = new TaskTree(m_controller, this);
    m_taskTree->setContextMenuPolicy(Qt::CustomContextMenu);

    m_taskDetailWidget = new TaskDetailWidget(m_controller, this);

    connect(m_taskTree, &TaskTree::taskSelected, this, &ContentArea::onTaskSelected);
    connect(m_taskTree, &TaskTree::taskDoubleClicked, this, &ContentArea::onTaskDoubleClicked);
    connect(m_taskTree, &TaskTree::contextMenuRequested, this, &ContentArea::onContextMenuRequested);
    connect(m_taskTree, &TaskTree::taskCountChanged, this, &ContentArea::onTaskCountChanged);
    connect(m_taskDetailWidget, &TaskDetailWidget::collapseRequested, this, &ContentArea::onDetailCollapseRequested);
    connect(m_controller, &TaskController::taskUpdated, this, &ContentArea::onTaskUpdated);
    connect(m_controller, &TaskController::taskCompletionChanged, this, &ContentArea::onTaskCompletionChanged);
    connect(m_controller, &TaskController::taskDeleted, this, &ContentArea::onTaskDeleted);

    m_splitter->addWidget(m_taskTree);
    m_splitter->addWidget(m_taskDetailWidget);
    m_splitter->setStretchFactor(0, 1);
    m_splitter->setStretchFactor(1, 0);
    m_splitter->setSizes({600, 400});

    m_mainLayout->addLayout(m_headerLayout);
    m_mainLayout->addWidget(m_searchWidget);
    m_mainLayout->addWidget(m_placeholderLabel, 1);
    m_mainLayout->addWidget(m_splitter, 1);

    collapseTaskDetailPanel();

    LOG_INFO("ContentArea", "Content area UI setup complete");
}

void ContentArea::setCurrentGroup(const QString &group)
{
    m_currentGroup = group;
    m_groupLabel->setText(group);
    if (m_taskTree) {
        m_taskTree->clearSelection();
    }
    collapseTaskDetailPanel();
    loadTasks();
    LOG_INFO("ContentArea", QString("Current group set to: %1").arg(group));
}

QString ContentArea::getCurrentGroup() const
{
    return m_currentGroup;
}

void ContentArea::setSearchText(const QString &text)
{
    if (m_searchWidget) {
        m_searchWidget->setSearchText(text);
    }
}

void ContentArea::loadTasks()
{
    if (m_taskTree) {
        m_taskTree->loadTasks(m_currentGroup, m_currentTagId, m_searchFilters);
        LOG_INFO("ContentArea", "Tasks loaded in task tree for group: " + m_currentGroup);
    }
}

void ContentArea::onTaskSelected(int taskId)
{
    if (taskId > 0) {
        showTaskDetailPanel();
        m_currentTaskId = taskId;
        Task task = (m_currentGroup == "回收站")
            ? m_controller->getTaskByIdIncludingDeleted(taskId)
            : m_controller->getTaskById(taskId);
        m_taskDetailWidget->setTask(task);
    } else {
        m_currentTaskId = 0;
        m_taskDetailWidget->clearTask();
    }
    LOG_INFO("ContentArea", QString("Task selected: %1").arg(taskId));
}

void ContentArea::onTaskDoubleClicked(int taskId)
{
    LOG_INFO("ContentArea", QString("Task double clicked: %1").arg(taskId));
}

void ContentArea::onContextMenuRequested(const QPoint &pos, int taskId)
{
    Q_UNUSED(pos);
    LOG_INFO("ContentArea", QString("Context menu requested for task: %1").arg(taskId));
    
    QMenu menu(this);

    if (m_currentGroup == "回收站") {
        QAction *restoreAction = new QAction("恢复任务", this);
        connect(restoreAction, &QAction::triggered, this, [this, taskId]() {
            if (m_controller->restoreTask(taskId)) {
                m_taskTree->refreshTasks();
                emit tagsChanged();
            }
        });

        QAction *permanentDeleteAction = new QAction("永久删除", this);
        connect(permanentDeleteAction, &QAction::triggered, this, [this, taskId]() {
            QMessageBox::StandardButton reply = QMessageBox::question(
                this, "确认永久删除",
                "该任务将被永久删除且无法恢复，确定继续吗？",
                QMessageBox::Yes | QMessageBox::No);
            if (reply == QMessageBox::Yes) {
                m_controller->permanentlyDeleteTask(taskId);
            }
        });

        menu.addAction(restoreAction);
        menu.addAction(permanentDeleteAction);
        menu.exec(QCursor::pos());
        return;
    }
    
    QAction *editAction = new QAction("编辑任务", this);
    connect(editAction, &QAction::triggered, this, [this, taskId]() {
        TaskDialog *dialog = new TaskDialog(m_controller, taskId, this);
        dialog->exec();
        m_taskTree->refreshTasks();
        emit tagsChanged();
    });
    
    QAction *addSubtaskAction = new QAction("添加子任务", this);
    connect(addSubtaskAction, &QAction::triggered, this, [this, taskId]() {
        TaskDialog *dialog = new TaskDialog(m_controller, -1, this);
        dialog->setParentTaskId(taskId);
        dialog->exec();
        m_taskTree->refreshTasks();
        emit tagsChanged();
    });
    
    QAction *deleteAction = new QAction("删除任务", this);
    connect(deleteAction, &QAction::triggered, this, [this, taskId]() {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, "确认删除",
            "确定要删除这个任务吗？",
            QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            m_controller->deleteTask(taskId);
        }
    });
    
    menu.addAction(editAction);
    menu.addAction(addSubtaskAction);
    menu.addSeparator();
    menu.addAction(deleteAction);
    
    menu.exec(QCursor::pos());
}

void ContentArea::onDetailCollapseRequested()
{
    collapseTaskDetailPanel();
}

void ContentArea::onTaskUpdated(const Task &task)
{
    updateDetailForTask(task.id());
}

void ContentArea::onTaskCompletionChanged(int taskId, bool completed)
{
    Q_UNUSED(completed);
    updateDetailForTask(taskId);
}

void ContentArea::onTaskDeleted(int taskId)
{
    if (taskId == m_currentTaskId) {
        m_currentTaskId = 0;
        m_taskDetailWidget->clearTask();
        collapseTaskDetailPanel();
    }
}

void ContentArea::onTagSelected(int tagId, const QString &tagName)
{
    m_currentTagId = tagId;
    m_currentTagName = tagName;
    updateTagFilterDisplay();
    if (m_searchWidget) {
        m_searchWidget->setSelectedTags(QList<int>() << tagId);
    } else {
        loadTasks();
    }
}

void ContentArea::onClearTagFilter()
{
    m_currentTagId = 0;
    m_currentTagName.clear();
    updateTagFilterDisplay();
    if (m_searchWidget) {
        m_searchWidget->setSelectedTags(QList<int>());
    } else {
        loadTasks();
    }
}

void ContentArea::onTaskCountChanged(int count)
{
    updateEmptyState(count);
}

void ContentArea::onSearchFiltersChanged(const TaskSearchFilters &filters)
{
    m_searchFilters = filters;
    if (m_currentTagId > 0 && !filters.tagIds.contains(m_currentTagId)) {
        m_currentTagId = 0;
        m_currentTagName.clear();
        updateTagFilterDisplay();
    }
    loadTasks();
}

void ContentArea::showTaskDetailPanel()
{
    if (!m_taskDetailWidget->isVisible()) {
        m_taskDetailWidget->setVisible(true);
    }
    m_splitter->setSizes({600, 400});
}

void ContentArea::collapseTaskDetailPanel()
{
    m_taskDetailWidget->clearTask();
    m_taskDetailWidget->setVisible(false);
    m_currentTaskId = 0;
    m_splitter->setSizes({1, 0});
}

void ContentArea::updateDetailForTask(int taskId)
{
    if (!m_taskDetailWidget->isVisible()) {
        return;
    }
    if (taskId <= 0 || taskId != m_currentTaskId) {
        return;
    }
    Task task = m_controller->getTaskById(taskId);
    if (task.id() > 0) {
        m_taskDetailWidget->setTask(task);
    }
}

void ContentArea::updateTagFilterDisplay()
{
    if (m_currentTagId > 0 && !m_currentTagName.isEmpty()) {
        m_tagFilterLabel->setText(QString("当前标签：%1").arg(m_currentTagName));
        m_tagFilterWidget->show();
    } else {
        m_tagFilterWidget->hide();
    }
}

void ContentArea::updateEmptyState(int count)
{
    if (count > 0) {
        m_placeholderLabel->hide();
        m_splitter->show();
        return;
    }

    bool isAllTasks = (m_currentGroup == "所有任务" && m_currentTagId <= 0);
    if (isAllTasks) {
        m_placeholderLabel->setText("添加你的第一个任务");
    } else {
        m_placeholderLabel->setText(randomEmptyMessage());
    }

    m_placeholderLabel->show();
    m_splitter->hide();
    collapseTaskDetailPanel();
}

QString ContentArea::randomEmptyMessage() const
{
    static const QStringList messages = {
        "没有任务，休息一下吧！",
        "今日清空，喝杯水放松下。",
        "暂时没有事项，给自己一点空白。",
        "列表很干净，继续保持。",
        "没有待办，正好规划下一步。",
        "清单为空，享受片刻宁静。",
        "没任务了，出去走走吧。",
        "一切已处理，辛苦啦！",
        "当前无任务，放松一下眼睛。",
        "没有新的安排，保持好节奏。"
    };
    int index = QRandomGenerator::global()->bounded(messages.size());
    return messages.at(index);
}
