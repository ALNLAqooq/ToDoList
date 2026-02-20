#include "task_detail_widget.h"
#include "../controllers/task_controller.h"
#include "../models/tag.h"
#include "../utils/file_utils.h"
#include "../utils/logger.h"
#include "../utils/theme_utils.h"
#include <QDate>
#include <QHBoxLayout>
#include <QFileInfo>
#include <QIcon>
#include <QColor>
#include <QSize>
#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>
#include <QtGlobal>

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
    , m_fileTitleLabel(nullptr)
    , m_fileList(nullptr)
    , m_subtaskTitleLabel(nullptr)
    , m_subtaskList(nullptr)
    , m_dependencyTitleLabel(nullptr)
    , m_dependencyList(nullptr)
    , m_priorityLabel(nullptr)
    , m_deadlineLabel(nullptr)
    , m_progressLabel(nullptr)
    , m_statusLabel(nullptr)
    , m_sourceLabel(nullptr)
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
    m_mainLayout->setContentsMargins(12, 12, 12, 12);
    m_mainLayout->setSpacing(12);

    auto *headerLayout = new QHBoxLayout();
    m_headerLabel = new QLabel("任务详情", this);
    m_headerLabel->setObjectName("detailHeaderLabel");

    m_collapseButton = new QPushButton("收起", this);
    m_collapseButton->setObjectName("detailCollapseButton");
    m_collapseButton->setFixedHeight(24);
    connect(m_collapseButton, &QPushButton::clicked, this, &TaskDetailWidget::collapseRequested);

    headerLayout->addWidget(m_headerLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(m_collapseButton);

    m_placeholderLabel = new QLabel("请从任务树中选择一个任务查看详情", this);
    m_placeholderLabel->setAlignment(Qt::AlignCenter);
    m_placeholderLabel->setObjectName("detailPlaceholder");

    m_titleLabel = new QLabel(this);
    m_titleLabel->setWordWrap(true);
    m_titleLabel->setObjectName("detailTitle");
    m_titleLabel->hide();

    m_statusLabel = new QLabel(this);
    m_statusLabel->setWordWrap(true);
    m_statusLabel->setStyleSheet("font-size: 14px; font-weight: 600; padding: 6px 12px; border-radius: 6px;");
    m_statusLabel->hide();

    m_priorityLabel = new QLabel(this);
    m_priorityLabel->setProperty("detailMuted", true);
    m_priorityLabel->hide();

    m_deadlineLabel = new QLabel(this);
    m_deadlineLabel->setProperty("detailMuted", true);
    m_deadlineLabel->hide();

    m_progressLabel = new QLabel(this);
    m_progressLabel->setProperty("detailMuted", true);
    m_progressLabel->hide();

    m_sourceLabel = new QLabel(this);
    m_sourceLabel->setObjectName("detailSourceLabel");
    m_sourceLabel->setWordWrap(true);
    if (m_currentTask.parentId() > 0 && m_controller) {
        Task parentTask = m_controller->getTaskById(m_currentTask.parentId());
        if (parentTask.id() > 0) {
            QString timeStr;
            if (parentTask.createdAt().isValid()) {
                timeStr = parentTask.createdAt().toString("yyyy-MM-dd HH:mm");
            }
            QString info = timeStr.isEmpty()
                ? parentTask.title()
                : QString("%1 / %2").arg(timeStr, parentTask.title());
            m_sourceLabel->setText(QString("来源任务: %1").arg(info));
            if (timeStr.isEmpty()) {
                m_sourceLabel->setToolTip(QString("父任务: %1").arg(parentTask.title()));
            } else {
                m_sourceLabel->setToolTip(QString("父任务: %1\n创建时间: %2").arg(parentTask.title(), timeStr));
            }
            m_sourceLabel->show();
        } else {
            m_sourceLabel->hide();
        }
    } else {
        m_sourceLabel->hide();
    }

    QLabel *descTitle = new QLabel("描述:", this);
    descTitle->setProperty("sectionTitle", true);
    descTitle->hide();

    m_descriptionLabel = new QLabel(this);
    m_descriptionLabel->setWordWrap(true);
    m_descriptionLabel->setObjectName("detailDescription");
    m_descriptionLabel->hide();

    m_tagTitleLabel = new QLabel("标签:", this);
    m_tagTitleLabel->setProperty("sectionTitle", true);
    m_tagTitleLabel->hide();

    m_tagValueLabel = new QLabel(this);
    m_tagValueLabel->setWordWrap(true);
    m_tagValueLabel->setObjectName("detailTagValue");
    m_tagValueLabel->hide();

    m_subtaskTitleLabel = new QLabel("子任务:", this);
    m_subtaskTitleLabel->setProperty("sectionTitle", true);
    m_subtaskTitleLabel->hide();

    m_subtaskList = new QListWidget(this);
    m_subtaskList->setSelectionMode(QAbstractItemView::NoSelection);
    m_subtaskList->setMaximumHeight(140);
    m_subtaskList->setObjectName("detailSubtaskList");
    m_subtaskList->hide();

    m_fileTitleLabel = new QLabel("关联文件:", this);
    m_fileTitleLabel->setProperty("sectionTitle", true);
    m_fileTitleLabel->hide();

    m_fileList = new QListWidget(this);
    m_fileList->setSelectionMode(QAbstractItemView::NoSelection);
    m_fileList->setMaximumHeight(140);
    m_fileList->setIconSize(QSize(18, 18));
    m_fileList->setObjectName("detailFileList");
    m_fileList->hide();
    connect(m_fileList, &QListWidget::itemDoubleClicked, this, &TaskDetailWidget::onFileItemDoubleClicked);

    m_dependencyTitleLabel = new QLabel("任务依赖:", this);
    m_dependencyTitleLabel->setProperty("sectionTitle", true);
    m_dependencyTitleLabel->hide();

    m_dependencyList = new QListWidget(this);
    m_dependencyList->setSelectionMode(QAbstractItemView::NoSelection);
    m_dependencyList->setMaximumHeight(140);
    m_dependencyList->setObjectName("detailDependencyList");
    m_dependencyList->hide();

    m_mainLayout->addLayout(headerLayout);
    m_mainLayout->addWidget(m_placeholderLabel);
    m_mainLayout->addWidget(m_titleLabel);
    m_mainLayout->addWidget(m_statusLabel);
    m_mainLayout->addWidget(m_priorityLabel);
    m_mainLayout->addWidget(m_deadlineLabel);
    m_mainLayout->addWidget(m_progressLabel);
    m_mainLayout->addWidget(m_sourceLabel);
    m_mainLayout->addWidget(descTitle);
    m_mainLayout->addWidget(m_descriptionLabel);
    m_mainLayout->addWidget(m_tagTitleLabel);
    m_mainLayout->addWidget(m_tagValueLabel);
    m_mainLayout->addWidget(m_fileTitleLabel);
    m_mainLayout->addWidget(m_fileList);
    m_mainLayout->addWidget(m_subtaskTitleLabel);
    m_mainLayout->addWidget(m_subtaskList);
    m_mainLayout->addWidget(m_dependencyTitleLabel);
    m_mainLayout->addWidget(m_dependencyList);
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
    m_fileTitleLabel->hide();
    m_fileList->hide();
    m_subtaskTitleLabel->hide();
    m_subtaskList->hide();
    m_dependencyTitleLabel->hide();
    m_dependencyList->hide();
    m_priorityLabel->hide();
    m_deadlineLabel->hide();
    m_progressLabel->hide();
    m_statusLabel->hide();
    m_sourceLabel->hide();
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
        const QDateTime createdAt = m_currentTask.createdAt();
        const QDateTime dueAt = m_currentTask.dueDate();
        const QDateTime now = QDateTime::currentDateTime();
        double ratio = 0.0;
        if (createdAt.isValid() && dueAt.isValid()) {
            const qint64 total = createdAt.msecsTo(dueAt);
            if (total > 0) {
                const qint64 elapsed = createdAt.msecsTo(now);
                ratio = qBound(0.0, static_cast<double>(elapsed) / static_cast<double>(total), 1.0);
            } else {
                ratio = now > dueAt ? 1.0 : 0.0;
            }
        } else {
            ratio = now > dueAt ? 1.0 : 0.0;
        }

        const QColor startColor("#10B981");
        const QColor endColor("#EF4444");
        const QString blended = ThemeUtils::blendColors(startColor, endColor, ratio);
        const QColor blendColor(blended);
        const QString bgColor = QString("rgba(%1, %2, %3, 30)")
                                    .arg(blendColor.red())
                                    .arg(blendColor.green())
                                    .arg(blendColor.blue());

        m_deadlineLabel->setText("截止日期: " + dueAt.toString("yyyy-MM-dd HH:mm"));
        m_deadlineLabel->setStyleSheet(QString("color: %1; background-color: %2; padding: 4px 8px; border-radius: 6px; font-weight: 600;")
                                           .arg(blended, bgColor));
        m_deadlineLabel->show();
    } else {
        m_deadlineLabel->hide();
    }

    m_progressLabel->setText(QString("进度: %1%").arg(static_cast<int>(m_currentTask.progress() * 100)));
    m_progressLabel->show();

    m_sourceLabel->hide();


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

        Task taskWithFiles = m_controller->getTaskById(m_currentTask.id());
        QList<QString> filePaths = taskWithFiles.filePaths();
        m_fileList->clear();
        if (filePaths.isEmpty()) {
            m_fileTitleLabel->hide();
            m_fileList->hide();
        } else {
            for (const QString &filePath : filePaths) {
                QFileInfo fileInfo(filePath);
                QString fileName = fileInfo.fileName();
                if (fileName.isEmpty()) {
                    fileName = filePath;
                }

                auto *item = new QListWidgetItem(fileName);
                item->setIcon(QIcon(FileUtils::getFileIconPath(filePath)));
                item->setData(Qt::UserRole, filePath);

                if (!fileInfo.exists()) {
                    item->setForeground(QColor("#DC2626"));
                    item->setText(fileName + " (缺失)");
                    item->setToolTip(QString("缺失文件:\n%1").arg(filePath));
                } else {
                    QString typeLabel = FileUtils::getFileType(filePath);
                    QString sizeLabel = FileUtils::fileSizeFormatted(fileInfo.size());
                    item->setToolTip(QString("%1\n%2 · %3").arg(filePath, typeLabel, sizeLabel));
                }

                m_fileList->addItem(item);
            }
            m_fileTitleLabel->show();
            m_fileList->show();
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

        QList<Task> dependencies = m_controller->getDependenciesForTask(m_currentTask.id());
        m_dependencyList->clear();
        if (dependencies.isEmpty()) {
            m_dependencyTitleLabel->hide();
            m_dependencyList->hide();
        } else {
            for (const Task &dependency : dependencies) {
                QString title = dependency.title();
                if (dependency.isCompleted()) {
                    title += " (已完成)";
                }
                m_dependencyList->addItem(title);
            }
            m_dependencyTitleLabel->show();
            m_dependencyList->show();
        }
    } else {
        m_tagTitleLabel->hide();
        m_tagValueLabel->hide();
        m_fileTitleLabel->hide();
        m_fileList->hide();
        m_subtaskTitleLabel->hide();
        m_subtaskList->hide();
        m_dependencyTitleLabel->hide();
        m_dependencyList->hide();
    }


    LOG_INFO("TaskDetailWidget", QString("Task detail updated: %1").arg(m_currentTask.title()));
}

void TaskDetailWidget::onFileItemDoubleClicked(QListWidgetItem *item)
{
    if (!item) {
        return;
    }

    QString filePath = item->data(Qt::UserRole).toString();
    if (filePath.isEmpty()) {
        return;
    }

    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists()) {
        QMessageBox::warning(this, "提示", "文件不存在: " + filePath);
        return;
    }

    QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
}
