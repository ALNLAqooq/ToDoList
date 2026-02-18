#include "task_card_widget.h"
#include "task_dialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QDate>

TaskCardWidget::TaskCardWidget(const Task &task, TaskController *controller, QWidget *parent)
    : QWidget(parent)
    , m_controller(controller)
    , m_taskId(task.id())
    , m_title(task.title())
    , m_description(task.description())
    , m_priority(task.priority())
    , m_dueDate(task.dueDate())
    , m_completed(task.isCompleted())
    , m_progress(task.progress())
{
    m_tags = m_controller->getTagsByTaskId(m_taskId);
    setupUI();
}

TaskCardWidget::~TaskCardWidget()
{
}

void TaskCardWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(12);
    mainLayout->setContentsMargins(16, 16, 16, 16);

    QHBoxLayout *headerLayout = new QHBoxLayout();
    headerLayout->setSpacing(12);

    m_checkBox = new QCheckBox(this);
    m_checkBox->setChecked(m_completed);

    m_titleLabel = new QLabel(m_title, this);
    m_titleLabel->setWordWrap(true);

    m_priorityLabel = new QLabel(this);
    updatePriorityIndicator();

    m_dueDateLabel = new QLabel(this);
    updateDueDateDisplay();

    m_editButton = new QPushButton(this);
    m_editButton->setToolTip("编辑");
    m_editButton->setFixedSize(32, 32);
    m_editButton->setStyleSheet(R"(
        QPushButton {
            background-color: transparent;
            border: none;
            border-radius: 4px;
            padding: 6px;
        }
        QPushButton:hover {
            background-color: rgba(59, 130, 246, 0.1);
        }
        QPushButton:pressed {
            background-color: rgba(59, 130, 246, 0.2);
        }
    )");

    m_deleteButton = new QPushButton(this);
    m_deleteButton->setToolTip("删除");
    m_deleteButton->setFixedSize(32, 32);
    m_deleteButton->setStyleSheet(R"(
        QPushButton {
            background-color: transparent;
            border: none;
            border-radius: 4px;
            padding: 6px;
        }
        QPushButton:hover {
            background-color: rgba(239, 68, 68, 0.1);
        }
        QPushButton:pressed {
            background-color: rgba(239, 68, 68, 0.2);
        }
    )");

    headerLayout->addWidget(m_checkBox);
    headerLayout->addWidget(m_titleLabel, 1);
    headerLayout->addWidget(m_priorityLabel);
    headerLayout->addWidget(m_dueDateLabel);
    headerLayout->addWidget(m_editButton);
    headerLayout->addWidget(m_deleteButton);

    if (!m_description.isEmpty()) {
        m_descriptionLabel = new QLabel(m_description, this);
        m_descriptionLabel->setWordWrap(true);
    } else {
        m_descriptionLabel = nullptr;
    }

    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setTextVisible(true);
    m_progressBar->setMaximumHeight(8);
    m_progressBar->setFixedHeight(8);
    updateProgressDisplay();

    m_tagsWidget = new QWidget(this);
    m_tagsLayout = new QHBoxLayout(m_tagsWidget);
    m_tagsLayout->setSpacing(6);
    m_tagsLayout->setContentsMargins(0, 0, 0, 0);
    m_tagsLayout->addStretch();
    updateTagsDisplay();

    mainLayout->addLayout(headerLayout);

    if (m_descriptionLabel) {
        mainLayout->addWidget(m_descriptionLabel);
    }

    mainLayout->addWidget(m_progressBar);
    mainLayout->addWidget(m_tagsWidget);

    connect(m_checkBox, &QCheckBox::stateChanged, this, &TaskCardWidget::onCheckBoxChanged);
    connect(m_editButton, &QPushButton::clicked, this, &TaskCardWidget::onEditClicked);
    connect(m_deleteButton, &QPushButton::clicked, this, &TaskCardWidget::onDeleteClicked);

    setCursor(Qt::PointingHandCursor);
}

void TaskCardWidget::updateTask(const Task &task)
{
    m_title = task.title();
    m_description = task.description();
    m_priority = task.priority();
    m_dueDate = task.dueDate();
    m_completed = task.isCompleted();
    m_progress = task.progress();
    m_tags = m_controller->getTagsByTaskId(m_taskId);

    m_checkBox->setChecked(m_completed);
    m_titleLabel->setText(m_title);

    if (m_descriptionLabel) {
        m_descriptionLabel->setText(m_description);
    }

    updatePriorityIndicator();
    updateDueDateDisplay();
    updateProgressDisplay();
    updateTagsDisplay();

    if (m_completed) {
        m_titleLabel->setStyleSheet("color: #94A3B8; text-decoration: line-through;");
    } else {
        m_titleLabel->setStyleSheet("color: #F8FAFC;");
    }
}

void TaskCardWidget::setCompleted(bool completed)
{
    m_completed = completed;
    m_checkBox->setChecked(completed);

    if (completed) {
        m_titleLabel->setStyleSheet("color: #94A3B8; text-decoration: line-through;");
    } else {
        m_titleLabel->setStyleSheet("color: #F8FAFC;");
    }
}

void TaskCardWidget::updatePriorityIndicator()
{
    QString priorityText;
    QString priorityColor;

    switch (m_priority) {
        case Task::High:
            priorityText = "高";
            priorityColor = "#EF4444";
            break;
        case Task::Medium:
            priorityText = "中";
            priorityColor = "#F59E0B";
            break;
        case Task::Low:
            priorityText = "低";
            priorityColor = "#10B981";
            break;
    }

    m_priorityLabel->setText(priorityText);
    m_priorityLabel->setStyleSheet(QString(R"(
        QLabel {
            background-color: %1;
            color: white;
            padding: 2px 8px;
            border-radius: 4px;
            font-size: 12px;
            font-weight: bold;
        }
    )").arg(priorityColor));
}

void TaskCardWidget::updateDueDateDisplay()
{
    if (m_dueDate.isValid()) {
        QDate currentDate = QDate::currentDate();
        QDate dueDate = m_dueDate.date();

        int daysLeft = currentDate.daysTo(dueDate);

        QString dateText;
        QString dateColor;

        if (daysLeft < 0) {
            dateText = "已过期 " + QString::number(qAbs(daysLeft)) + " 天";
            dateColor = "#DC2626";
        } else if (daysLeft == 0) {
            dateText = "今天到期";
            dateColor = "#EF4444";
        } else if (daysLeft == 1) {
            dateText = "明天到期";
            dateColor = "#F59E0B";
        } else if (daysLeft <= 7) {
            dateText = QString::number(daysLeft) + " 天后到期";
            dateColor = "#F59E0B";
        } else {
            dateText = m_dueDate.toString("yyyy-MM-dd");
            dateColor = "#94A3B8";
        }

        m_dueDateLabel->setText(dateText);
        m_dueDateLabel->setStyleSheet(QString(R"(
            QLabel {
                color: %1;
                font-size: 12px;
                padding: 2px 8px;
            }
        )").arg(dateColor));
    } else {
        m_dueDateLabel->setText("");
    }
}

void TaskCardWidget::updateProgressDisplay()
{
    int percentage = static_cast<int>(m_progress * 100);
    m_progressBar->setValue(percentage);

    QString progressColor;

    if (percentage >= 100) {
        progressColor = "#10B981";
    } else if (percentage >= 50) {
        progressColor = "#3B82F6";
    } else if (percentage >= 25) {
        progressColor = "#F59E0B";
    } else {
        progressColor = "#EF4444";
    }

    m_progressBar->setStyleSheet(QString(R"(
        QProgressBar {
            background-color: #334155;
            border: none;
            border-radius: 4px;
            text-align: center;
            color: #F8FAFC;
            height: 8px;
        }
        QProgressBar::chunk {
            background-color: %1;
            border-radius: 4px;
        }
    )").arg(progressColor));
}

void TaskCardWidget::updateTagsDisplay()
{
    for (int i = m_tagsLayout->count() - 1; i >= 0; i--) {
        QLayoutItem *item = m_tagsLayout->takeAt(i);
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }

    m_tagsLayout->addStretch();

    for (const Tag &tag : m_tags) {
        QLabel *tagLabel = new QLabel(tag.name(), this);
        tagLabel->setStyleSheet(QString(R"(
            QLabel {
                background-color: %1;
                color: white;
                padding: 2px 8px;
                border-radius: 4px;
                font-size: 11px;
            }
        )").arg(tag.color()));
        m_tagsLayout->addWidget(tagLabel);
    }

    m_tagsWidget->setVisible(!m_tags.isEmpty());
}

void TaskCardWidget::onCheckBoxChanged(int state)
{
    bool completed = (state == Qt::Checked);
    m_controller->toggleTaskCompletion(m_taskId);
}

void TaskCardWidget::onEditClicked()
{
    emit editRequested(m_taskId);
}

void TaskCardWidget::onDeleteClicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "确认删除",
        "确定要删除这个任务吗？",
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        m_controller->deleteTask(m_taskId);
    }
}
