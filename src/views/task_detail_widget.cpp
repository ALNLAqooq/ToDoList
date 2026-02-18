#include "task_detail_widget.h"
#include "../controllers/task_controller.h"
#include "../models/tag.h"
#include "../utils/logger.h"
#include <QDate>
#include <QHBoxLayout>

TaskDetailWidget::TaskDetailWidget(TaskController *controller, QWidget *parent)
    : QWidget(parent)
    , m_controller(controller)
    , m_mainLayout(nullptr)
    , m_headerLabel(nullptr)
    , m_collapseButton(nullptr)
    , m_titleLabel(nullptr)
    , m_descriptionLabel(nullptr)
    , m_tagTitleLabel(nullptr)
    , m_tagValueLabel(nullptr)
    , m_subtaskTitleLabel(nullptr)
    , m_subtaskList(nullptr)
    , m_priorityLabel(nullptr)
    , m_deadlineLabel(nullptr)
    , m_progressLabel(nullptr)
    , m_statusLabel(nullptr)
    , m_placeholderLabel(nullptr)
{
    setupUI();
    LOG_INFO("TaskDetailWidget", "Task detail widget created");
}

TaskDetailWidget::~TaskDetailWidget()
{
}

void TaskDetailWidget::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(20, 20, 20, 20);
    m_mainLayout->setSpacing(16);

    auto *headerLayout = new QHBoxLayout();
    m_headerLabel = new QLabel("任务详情", this);
    m_headerLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #0F172A;");

    m_collapseButton = new QPushButton("收起", this);
    m_collapseButton->setFixedHeight(28);
    m_collapseButton->setStyleSheet(
        "QPushButton { background: #F1F5F9; color: #475569; padding: 4px 12px; "
        "border-radius: 6px; border: 1px solid #E2E8F0; font-size: 12px; }"
        "QPushButton:hover { background: #E2E8F0; }"
    );
    connect(m_collapseButton, &QPushButton::clicked, this, &TaskDetailWidget::collapseRequested);

    headerLayout->addWidget(m_headerLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(m_collapseButton);

    m_placeholderLabel = new QLabel("请从任务树中选择一个任务查看详情", this);
    m_placeholderLabel->setAlignment(Qt::AlignCenter);
    m_placeholderLabel->setStyleSheet("font-size: 16px; color: #94A3B8; padding: 50px;");

    m_titleLabel = new QLabel(this);
    m_titleLabel->setWordWrap(true);
    m_titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #0F172A;");
    m_titleLabel->hide();

    m_statusLabel = new QLabel(this);
    m_statusLabel->setWordWrap(true);
    m_statusLabel->setStyleSheet("font-size: 14px; font-weight: 600; padding: 6px 12px; border-radius: 6px;");
    m_statusLabel->hide();

    m_priorityLabel = new QLabel(this);
    m_priorityLabel->setStyleSheet("font-size: 14px; color: #64748B;");
    m_priorityLabel->hide();

    m_deadlineLabel = new QLabel(this);
    m_deadlineLabel->setStyleSheet("font-size: 14px; color: #64748B;");
    m_deadlineLabel->hide();

    m_progressLabel = new QLabel(this);
    m_progressLabel->setStyleSheet("font-size: 14px; color: #64748B;");
    m_progressLabel->hide();

    QLabel *descTitle = new QLabel("描述:", this);
    descTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #0F172A;");
    descTitle->hide();

    m_descriptionLabel = new QLabel(this);
    m_descriptionLabel->setWordWrap(true);
    m_descriptionLabel->setStyleSheet("font-size: 14px; color: #475569; padding: 12px; background: #F8FAFC; border-radius: 8px;");
    m_descriptionLabel->hide();

    m_tagTitleLabel = new QLabel("标签:", this);
    m_tagTitleLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #0F172A;");
    m_tagTitleLabel->hide();

    m_tagValueLabel = new QLabel(this);
    m_tagValueLabel->setWordWrap(true);
    m_tagValueLabel->setStyleSheet("font-size: 13px; color: #475569; padding: 6px 8px; background: #F1F5F9; border-radius: 6px;");
    m_tagValueLabel->hide();

    m_subtaskTitleLabel = new QLabel("子任务:", this);
    m_subtaskTitleLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #0F172A;");
    m_subtaskTitleLabel->hide();

    m_subtaskList = new QListWidget(this);
    m_subtaskList->setSelectionMode(QAbstractItemView::NoSelection);
    m_subtaskList->setMaximumHeight(140);
    m_subtaskList->setStyleSheet(R"(
        QListWidget {
            border: 1px solid #E2E8F0;
            border-radius: 8px;
            background: #FFFFFF;
            padding: 6px;
            font-size: 13px;
            color: #334155;
        }
    )");
    m_subtaskList->hide();

    m_mainLayout->addLayout(headerLayout);
    m_mainLayout->addWidget(m_placeholderLabel);
    m_mainLayout->addWidget(m_titleLabel);
    m_mainLayout->addWidget(m_statusLabel);
    m_mainLayout->addWidget(m_priorityLabel);
    m_mainLayout->addWidget(m_deadlineLabel);
    m_mainLayout->addWidget(m_progressLabel);
    m_mainLayout->addWidget(descTitle);
    m_mainLayout->addWidget(m_descriptionLabel);
    m_mainLayout->addWidget(m_tagTitleLabel);
    m_mainLayout->addWidget(m_tagValueLabel);
    m_mainLayout->addWidget(m_subtaskTitleLabel);
    m_mainLayout->addWidget(m_subtaskList);
    m_mainLayout->addStretch();
}

void TaskDetailWidget::setTask(const Task &task)
{
    m_currentTask = task;
    updateDisplay();
}

void TaskDetailWidget::clearTask()
{
    m_currentTask = Task();
    m_placeholderLabel->show();
    m_titleLabel->hide();
    m_descriptionLabel->hide();
    m_tagTitleLabel->hide();
    m_tagValueLabel->hide();
    m_subtaskTitleLabel->hide();
    m_subtaskList->hide();
    m_priorityLabel->hide();
    m_deadlineLabel->hide();
    m_progressLabel->hide();
    m_statusLabel->hide();
}

void TaskDetailWidget::updateDisplay()
{
    if (m_currentTask.id() <= 0) {
        clearTask();
        return;
    }

    m_placeholderLabel->hide();

    m_titleLabel->setText(m_currentTask.title());
    m_titleLabel->show();

    QString statusText = m_currentTask.isCompleted() ? "已完成" : "进行中";
    QString statusColor = m_currentTask.isCompleted() ? "#10B981" : "#3B82F6";
    m_statusLabel->setText(statusText);
    m_statusLabel->setStyleSheet(QString("font-size: 14px; font-weight: 600; padding: 6px 12px; border-radius: 6px; background: %1; color: white;").arg(statusColor));
    m_statusLabel->show();

    QString priorityText;
    switch (m_currentTask.priority()) {
        case Task::High:
            priorityText = "优先级: 高";
            break;
        case Task::Medium:
            priorityText = "优先级: 中";
            break;
        case Task::Low:
            priorityText = "优先级: 低";
            break;
    }
    m_priorityLabel->setText(priorityText);
    m_priorityLabel->show();

    if (m_currentTask.dueDate().isValid()) {
        m_deadlineLabel->setText("截止日期: " + m_currentTask.dueDate().toString("yyyy-MM-dd HH:mm"));
        m_deadlineLabel->show();
    } else {
        m_deadlineLabel->hide();
    }

    m_progressLabel->setText(QString("进度: %1%").arg(static_cast<int>(m_currentTask.progress() * 100)));
    m_progressLabel->show();

    if (!m_currentTask.description().isEmpty()) {
        m_descriptionLabel->setText(m_currentTask.description());
        m_descriptionLabel->show();
    } else {
        m_descriptionLabel->hide();
    }

    if (m_controller) {
        QList<Tag> tags = m_controller->getTagsByTaskId(m_currentTask.id());
        if (tags.isEmpty()) {
            m_tagTitleLabel->hide();
            m_tagValueLabel->hide();
        } else {
            QStringList names;
            for (const Tag &tag : tags) {
                names.append(tag.name());
            }
            m_tagValueLabel->setText(names.join(", "));
            m_tagTitleLabel->show();
            m_tagValueLabel->show();
        }

        QList<Task> subtasks = m_controller->getSubTasks(m_currentTask.id());
        m_subtaskList->clear();
        for (const Task &subtask : subtasks) {
            QString title = subtask.title();
            if (subtask.isCompleted()) {
                title = "✔ " + title;
            }
            m_subtaskList->addItem(title);
        }
        if (subtasks.isEmpty()) {
            m_subtaskTitleLabel->hide();
            m_subtaskList->hide();
        } else {
            m_subtaskTitleLabel->show();
            m_subtaskList->show();
        }
    } else {
        m_tagTitleLabel->hide();
        m_tagValueLabel->hide();
        m_subtaskTitleLabel->hide();
        m_subtaskList->hide();
    }

    LOG_INFO("TaskDetailWidget", QString("Task detail updated: %1").arg(m_currentTask.title()));
}
