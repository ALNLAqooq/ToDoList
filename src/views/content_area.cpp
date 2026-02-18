#include "content_area.h"
#include "task_tree.h"
#include "task_detail_widget.h"
#include "task_dialog.h"
#include "../controllers/task_controller.h"
#include "../utils/logger.h"
#include <QMessageBox>

ContentArea::ContentArea(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
    , m_headerLayout(nullptr)
    , m_groupLabel(nullptr)
    , m_tagFilterWidget(nullptr)
    , m_tagFilterLabel(nullptr)
    , m_tagClearButton(nullptr)
    , m_placeholderLabel(nullptr)
    , m_splitter(nullptr)
    , m_taskTree(nullptr)
    , m_taskDetailWidget(nullptr)
    , m_controller(nullptr)
    , m_currentGroup("所有任务")
    , m_currentTaskId(0)
    , m_currentTagId(0)
    , m_currentTagName("")
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
    m_mainLayout->setContentsMargins(20, 20, 20, 20);
    m_mainLayout->setSpacing(15);

    m_groupLabel = new QLabel("所有任务", this);
    m_groupLabel->setFixedHeight(32);
    m_groupLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #0F172A; padding: 0;");
    m_groupLabel->setAlignment(Qt::AlignVCenter);

    m_tagFilterWidget = new QWidget(this);
    auto *tagLayout = new QHBoxLayout(m_tagFilterWidget);
    tagLayout->setContentsMargins(10, 4, 10, 4);
    tagLayout->setSpacing(8);

    m_tagFilterLabel = new QLabel(this);
    m_tagFilterLabel->setStyleSheet("font-size: 12px; color: #1F2937;");

    m_tagClearButton = new QPushButton("清除", this);
    m_tagClearButton->setFixedHeight(22);
    m_tagClearButton->setStyleSheet(
        "QPushButton { background: #E2E8F0; color: #334155; padding: 2px 8px; "
        "border-radius: 10px; border: none; font-size: 12px; }"
        "QPushButton:hover { background: #CBD5E1; }"
    );
    connect(m_tagClearButton, &QPushButton::clicked, this, &ContentArea::onClearTagFilter);

    tagLayout->addWidget(m_tagFilterLabel);
    tagLayout->addWidget(m_tagClearButton);
    m_tagFilterWidget->setStyleSheet("background: #F8FAFC; border: 1px solid #E2E8F0; border-radius: 14px;");
    m_tagFilterWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
    m_tagFilterWidget->hide();

    m_headerLayout = new QHBoxLayout();
    m_headerLayout->addWidget(m_groupLabel);
    m_headerLayout->addSpacing(12);
    m_headerLayout->addWidget(m_tagFilterWidget);
    m_headerLayout->addStretch();

    m_placeholderLabel = new QLabel("暂无任务。点击 + 按钮添加新任务。", this);
    m_placeholderLabel->setAlignment(Qt::AlignCenter);
    m_placeholderLabel->setStyleSheet("font-size: 16px; color: #94A3B8; padding: 50px;");
    m_placeholderLabel->hide();

    m_splitter = new QSplitter(Qt::Horizontal, this);
    m_splitter->setHandleWidth(1);
    m_splitter->setStyleSheet("QSplitter::handle { background: #E2E8F0; }");

    m_controller = new TaskController(this);
    m_taskTree = new TaskTree(m_controller, this);
    m_taskTree->setContextMenuPolicy(Qt::CustomContextMenu);

    m_taskDetailWidget = new TaskDetailWidget(m_controller, this);

    connect(m_taskTree, &TaskTree::taskSelected, this, &ContentArea::onTaskSelected);
    connect(m_taskTree, &TaskTree::taskDoubleClicked, this, &ContentArea::onTaskDoubleClicked);
    connect(m_taskTree, &TaskTree::contextMenuRequested, this, &ContentArea::onContextMenuRequested);
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
    m_mainLayout->addWidget(m_splitter);

    collapseTaskDetailPanel();

    LOG_INFO("ContentArea", "Content area UI setup complete");
}

void ContentArea::setCurrentGroup(const QString &group)
{
    m_currentGroup = group;
    m_groupLabel->setText(group);
    loadTasks();
    LOG_INFO("ContentArea", QString("Current group set to: %1").arg(group));
}

QString ContentArea::getCurrentGroup() const
{
    return m_currentGroup;
}

void ContentArea::loadTasks()
{
    if (m_taskTree) {
        m_taskTree->loadTasks(m_currentGroup, m_currentTagId);
        LOG_INFO("ContentArea", "Tasks loaded in task tree for group: " + m_currentGroup);
    }
}

void ContentArea::onTaskSelected(int taskId)
{
    if (taskId > 0) {
        showTaskDetailPanel();
        m_currentTaskId = taskId;
        Task task = m_controller->getTaskById(taskId);
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
    loadTasks();
}

void ContentArea::onClearTagFilter()
{
    m_currentTagId = 0;
    m_currentTagName.clear();
    updateTagFilterDisplay();
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
    if (m_taskDetailWidget->isVisible()) {
        m_taskDetailWidget->setVisible(false);
    }
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
