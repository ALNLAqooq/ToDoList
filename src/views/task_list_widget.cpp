#include "task_list_widget.h"
#include "task_card_widget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QLayoutItem>

TaskListWidget::TaskListWidget(TaskController *controller, QWidget *parent)
    : QWidget(parent)
    , m_controller(controller)
{
    setupUI();
    refreshTasks();

    connect(m_controller, &TaskController::taskAdded, this, &TaskListWidget::onTaskAdded);
    connect(m_controller, &TaskController::taskUpdated, this, &TaskListWidget::onTaskUpdated);
    connect(m_controller, &TaskController::taskDeleted, this, &TaskListWidget::onTaskDeleted);
    connect(m_controller, &TaskController::taskCompletionChanged, this, &TaskListWidget::onTaskCompletionChanged);
}

TaskListWidget::~TaskListWidget()
{
}

void TaskListWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    QLayout *filterLayout = setupFilterBar();
    mainLayout->addLayout(filterLayout);

    setupTaskList();
    mainLayout->addWidget(m_scrollArea);
}

QLayout* TaskListWidget::setupFilterBar()
{
    QHBoxLayout *filterLayout = new QHBoxLayout();

    QLabel *searchLabel = new QLabel("搜索:", this);
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("输入关键词搜索任务...");
    m_searchEdit->setMinimumWidth(300);

    m_filterButton = new QPushButton("筛选", this);

    filterLayout->addWidget(searchLabel);
    filterLayout->addWidget(m_searchEdit);
    filterLayout->addWidget(m_filterButton);
    filterLayout->addStretch();

    connect(m_searchEdit, &QLineEdit::textChanged, this, &TaskListWidget::filterTasks);

    return filterLayout;
}

void TaskListWidget::setupTaskList()
{
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameShape(QFrame::NoFrame);

    m_taskListContainer = new QWidget();
    m_taskListLayout = new QVBoxLayout(m_taskListContainer);
    m_taskListLayout->setSpacing(15);
    m_taskListLayout->setContentsMargins(0, 0, 0, 0);
    m_taskListLayout->addStretch();

    m_scrollArea->setWidget(m_taskListContainer);
}

void TaskListWidget::refreshTasks()
{
    qDeleteAll(m_taskCards);
    m_taskCards.clear();
    m_taskListLayout->removeItem(m_taskListLayout->itemAt(m_taskListLayout->count() - 1));

    m_allTasks = m_controller->getAllTasks();
    m_filteredTasks = m_allTasks;

    for (const Task &task : m_filteredTasks) {
        if (task.parentId() == 0) {
            TaskCardWidget *card = new TaskCardWidget(task, m_controller, this);
            m_taskListLayout->insertWidget(m_taskListLayout->count() - 1, card);
            m_taskCards.append(card);
        }
    }

    m_taskListLayout->addStretch();
}

void TaskListWidget::onTaskAdded(const Task &task)
{
    if (task.parentId() == 0) {
        TaskCardWidget *card = new TaskCardWidget(task, m_controller, this);
        m_taskListLayout->insertWidget(m_taskListLayout->count() - 1, card);
        m_taskCards.append(card);
    }
    m_allTasks.append(task);
}

void TaskListWidget::onTaskUpdated(const Task &task)
{
    TaskCardWidget *card = findTaskCard(task.id());
    if (card) {
        card->updateTask(task);
    }
}

void TaskListWidget::onTaskDeleted(int taskId)
{
    TaskCardWidget *card = findTaskCard(taskId);
    if (card) {
        m_taskListLayout->removeWidget(card);
        m_taskCards.removeOne(card);
        delete card;
    }
}

void TaskListWidget::onTaskCompletionChanged(int taskId, bool completed)
{
    TaskCardWidget *card = findTaskCard(taskId);
    if (card) {
        card->setCompleted(completed);
    }
}

TaskCardWidget* TaskListWidget::findTaskCard(int taskId)
{
    for (TaskCardWidget *card : m_taskCards) {
        if (card->taskId() == taskId) {
            return card;
        }
    }
    return nullptr;
}

void TaskListWidget::filterTasks()
{
    QString searchText = m_searchEdit->text().trimmed().toLower();

    if (searchText.isEmpty()) {
        m_filteredTasks = m_allTasks;
    } else {
        m_filteredTasks.clear();
        for (const Task &task : m_allTasks) {
            if (task.title().toLower().contains(searchText) ||
                task.description().toLower().contains(searchText)) {
                m_filteredTasks.append(task);
            }
        }
    }

    qDeleteAll(m_taskCards);
    m_taskCards.clear();
    m_taskListLayout->removeItem(m_taskListLayout->itemAt(m_taskListLayout->count() - 1));

    for (const Task &task : m_filteredTasks) {
        if (task.parentId() == 0) {
            TaskCardWidget *card = new TaskCardWidget(task, m_controller, this);
            m_taskListLayout->insertWidget(m_taskListLayout->count() - 1, card);
            m_taskCards.append(card);
        }
    }

    m_taskListLayout->addStretch();
}
