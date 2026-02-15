#include "task_card_widget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QProgressBar>
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
    setupUI();
}

TaskCardWidget::~TaskCardWidget()
{
}

void TaskCardWidget::setupUI()
{
    setStyleSheet(R"(
        TaskCardWidget {
            background-color: #FFFFFF;
            border-radius: 8px;
            border: 1px solid #E5E7EB;
            padding: 16px;
        }
        TaskCardWidget:hover {
            border: 1px solid #3B82F6;
        }
    )");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(12);

    QHBoxLayout *headerLayout = new QHBoxLayout();

    m_checkBox = new QCheckBox(this);
    m_checkBox->setChecked(m_completed);
    m_checkBox->setStyleSheet(R"(
        QCheckBox::indicator {
            width: 20px;
            height: 20px;
            border-radius: 4px;
            border: 2px solid #9CA3AF;
            background-color: white;
        }
        QCheckBox::indicator:checked {
            background-color: #3B82F6;
            border-color: #3B82F6;
        }
    )");

    m_titleLabel = new QLabel(m_title, this);
    m_titleLabel->setWordWrap(true);
    m_titleLabel->setStyleSheet(R"(
        QLabel {
            font-size: 16px;
            font-weight: bold;
            color: #1F2937;
        }
    )");
    if (m_completed) {
        m_titleLabel->setStyleSheet(R"(
            QLabel {
                font-size: 16px;
                font-weight: bold;
                color: #9CA3AF;
                text-decoration: line-through;
            }
        )");
    }

    m_priorityLabel = new QLabel(this);
    updatePriorityIndicator();

    m_dueDateLabel = new QLabel(this);
    updateDueDateDisplay();

    m_editButton = new QPushButton("编辑", this);
    m_editButton->setStyleSheet(R"(
        QPushButton {
            background-color: #F3F4F6;
            border: none;
            border-radius: 4px;
            padding: 4px 12px;
            color: #4B5563;
        }
        QPushButton:hover {
            background-color: #E5E7EB;
        }
    )");

    m_deleteButton = new QPushButton("删除", this);
    m_deleteButton->setStyleSheet(R"(
        QPushButton {
            background-color: #FEF2F2;
            border: none;
            border-radius: 4px;
            padding: 4px 12px;
            color: #DC2626;
        }
        QPushButton:hover {
            background-color: #FEE2E2;
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
        m_descriptionLabel->setStyleSheet(R"(
            QLabel {
                font-size: 14px;
                color: #6B7280;
            }
        )");
    } else {
        m_descriptionLabel = nullptr;
    }

    m_progressLabel = new QLabel(this);
    updateProgressDisplay();

    mainLayout->addLayout(headerLayout);

    if (m_descriptionLabel) {
        mainLayout->addWidget(m_descriptionLabel);
    }

    mainLayout->addWidget(m_progressLabel);

    connect(m_checkBox, &QCheckBox::stateChanged, this, &TaskCardWidget::onCheckBoxChanged);
    connect(m_editButton, &QPushButton::clicked, this, &TaskCardWidget::onEditClicked);
    connect(m_deleteButton, &QPushButton::clicked, this, &TaskCardWidget::onDeleteClicked);
}

void TaskCardWidget::updateTask(const Task &task)
{
    m_title = task.title();
    m_description = task.description();
    m_priority = task.priority();
    m_dueDate = task.dueDate();
    m_completed = task.isCompleted();
    m_progress = task.progress();

    m_checkBox->setChecked(m_completed);
    m_titleLabel->setText(m_title);

    if (m_descriptionLabel) {
        m_descriptionLabel->setText(m_description);
    }

    updatePriorityIndicator();
    updateDueDateDisplay();
    updateProgressDisplay();

    if (m_completed) {
        m_titleLabel->setStyleSheet(R"(
            QLabel {
                font-size: 16px;
                font-weight: bold;
                color: #9CA3AF;
                text-decoration: line-through;
            }
        )");
    } else {
        m_titleLabel->setStyleSheet(R"(
            QLabel {
                font-size: 16px;
                font-weight: bold;
                color: #1F2937;
            }
        )");
    }
}

void TaskCardWidget::setCompleted(bool completed)
{
    m_completed = completed;
    m_checkBox->setChecked(completed);

    if (completed) {
        m_titleLabel->setStyleSheet(R"(
            QLabel {
                font-size: 16px;
                font-weight: bold;
                color: #9CA3AF;
                text-decoration: line-through;
            }
        )");
    } else {
        m_titleLabel->setStyleSheet(R"(
            QLabel {
                font-size: 16px;
                font-weight: bold;
                color: #1F2937;
            }
        )");
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
            dateColor = "#6B7280";
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
    if (m_progress > 0) {
        int percentage = static_cast<int>(m_progress * 100);
        m_progressLabel->setText(QString("进度: %1%").arg(percentage));
        m_progressLabel->setStyleSheet(R"(
            QLabel {
                color: #3B82F6;
                font-size: 12px;
            }
        )");
    } else {
        m_progressLabel->setText("");
    }
}

void TaskCardWidget::onCheckBoxChanged(int state)
{
    bool completed = (state == Qt::Checked);
    m_controller->toggleTaskCompletion(m_taskId);
}

void TaskCardWidget::onEditClicked()
{
    QMessageBox::information(this, "提示", "编辑功能待实现");
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
