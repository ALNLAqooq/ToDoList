#include "sidebar.h"
#include "../utils/logger.h"
#include "../controllers/database.h"
#include "../models/folder.h"
#include "../models/tag.h"
#include <QListWidgetItem>
#include <QIcon>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QAction>
#include <QColor>
#include <QLineEdit>

Sidebar::Sidebar(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
    , m_stackWidget(nullptr)
    , m_expandedWidget(nullptr)
    , m_groupsList(nullptr)
    , m_newFolderButton(nullptr)
    , m_groupsTitle(nullptr)
    , m_foldersTitle(nullptr)
    , m_foldersList(nullptr)
    , m_tagsTitle(nullptr)
    , m_tagsList(nullptr)
    , m_collapsedWidget(nullptr)
    , m_expandButton(nullptr)
    , m_expanded(true)
{
    setMinimumWidth(200);
    setMaximumWidth(500);
    setMinimumHeight(400);
    setupUI();
    LOG_INFO("Sidebar", "Sidebar widget created");
}

Sidebar::~Sidebar()
{
}

void Sidebar::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    m_stackWidget = new QStackedWidget(this);

    setupExpandedView();
    setupCollapsedView();

    m_stackWidget->addWidget(m_expandedWidget);
    m_stackWidget->addWidget(m_collapsedWidget);
    m_stackWidget->setCurrentWidget(m_expandedWidget);

    m_mainLayout->addWidget(m_stackWidget);

    LOG_INFO("Sidebar", "Sidebar UI setup complete");
}

void Sidebar::setupExpandedView()
{
    m_expandedWidget = new QWidget(this);
    QVBoxLayout *expandedLayout = new QVBoxLayout(m_expandedWidget);
    expandedLayout->setContentsMargins(0, 0, 0, 0);
    expandedLayout->setSpacing(0);

    setupGroups();
    setupCustomFolders();
    setupTags();

    expandedLayout->addWidget(m_groupsTitle);
    expandedLayout->addWidget(m_groupsList);
    expandedLayout->addWidget(m_foldersTitle);
    expandedLayout->addWidget(m_foldersList);
    expandedLayout->addWidget(m_newFolderButton);
    expandedLayout->addWidget(m_tagsTitle);
    expandedLayout->addWidget(m_tagsList);
    expandedLayout->addStretch();
}

void Sidebar::setupGroups()
{
    m_groupsTitle = new QLabel("分组", this);
    m_groupsTitle->setStyleSheet("font-weight: bold; padding: 10px 15px 5px;");

    m_groupsList = new QListWidget(this);
    m_groupsList->setFrameStyle(QFrame::NoFrame);
    m_groupsList->setStyleSheet(
        "QListWidget { background: transparent; }"
        "QListWidget::item { padding: 8px 15px; border-radius: 4px; }"
        "QListWidget::item:hover { background: rgba(59, 130, 246, 0.1); }"
        "QListWidget::item:selected { background: rgba(59, 130, 246, 0.2); }"
    );
    m_groupsList->setMaximumHeight(200);

    QStringList groups = {"所有任务", "今天", "本周", "本月", "已过期", 
                          "高优先级", "中优先级", "低优先级",
                          "已完成", "未完成", "进行中"};

    for (const QString &group : groups) {
        QListWidgetItem *item = new QListWidgetItem(group, m_groupsList);
        m_groupsList->addItem(item);
    }

    QListWidgetItem *recycleItem = new QListWidgetItem("回收站", m_groupsList);
    m_groupsList->addItem(recycleItem);

    connect(m_groupsList, &QListWidget::itemClicked, this, &Sidebar::onItemClicked);

    LOG_INFO("Sidebar", "Groups setup complete");
}

void Sidebar::setupCustomFolders()
{
    m_foldersTitle = new QLabel("文件夹", this);
    m_foldersTitle->setStyleSheet("font-weight: bold; padding: 10px 15px 5px;");

    m_foldersList = new QListWidget(this);
    m_foldersList->setFrameStyle(QFrame::NoFrame);
    m_foldersList->setStyleSheet(
        "QListWidget { background: transparent; }"
        "QListWidget::item { padding: 8px 15px; border-radius: 4px; }"
        "QListWidget::item:hover { background: rgba(59, 130, 246, 0.1); }"
        "QListWidget::item:selected { background: rgba(59, 130, 246, 0.2); }"
    );
    m_foldersList->setMaximumHeight(150);
    m_foldersList->setContextMenuPolicy(Qt::CustomContextMenu);

    m_newFolderButton = new QPushButton("+ 新建文件夹", this);
    m_newFolderButton->setStyleSheet(
        "QPushButton { background: transparent; border: 1px dashed #94A3B8; "
        "padding: 8px; border-radius: 4px; color: #94A3B8; }"
        "QPushButton:hover { border-color: #3B82F6; color: #3B82F6; }"
    );

    connect(m_newFolderButton, &QPushButton::clicked, this, &Sidebar::onNewFolderClicked);
    connect(m_foldersList, &QListWidget::itemClicked, this, &Sidebar::onItemClicked);
    connect(m_foldersList, &QListWidget::customContextMenuRequested, this, [this](const QPoint &pos) {
        QListWidgetItem *item = m_foldersList->itemAt(pos);
        if (item) {
            QMenu menu(this);
            QAction *renameAction = menu.addAction("重命名");
            QAction *deleteAction = menu.addAction("删除");
            QAction *selected = menu.exec(m_foldersList->mapToGlobal(pos));

            if (selected == renameAction) {
                renameFolder(item);
            } else if (selected == deleteAction) {
                deleteFolder(item);
            }
        }
    });

    loadFolders();
    LOG_INFO("Sidebar", "Custom folders setup complete");
}

void Sidebar::setupCollapsedView()
{
    m_collapsedWidget = new QWidget(this);
    QVBoxLayout *collapsedLayout = new QVBoxLayout(m_collapsedWidget);
    collapsedLayout->setContentsMargins(5, 10, 5, 10);
    collapsedLayout->setSpacing(10);

    m_expandButton = new QPushButton(">", this);
    m_expandButton->setFixedSize(30, 30);
    m_expandButton->setStyleSheet(
        "QPushButton { background: #3B82F6; border-radius: 15px; color: white; font-weight: bold; }"
        "QPushButton:hover { background: #2563EB; }"
    );

    connect(m_expandButton, &QPushButton::clicked, this, [this]() {
        emit collapseRequested();
    });

    collapsedLayout->addWidget(m_expandButton);
    collapsedLayout->addStretch();

    LOG_INFO("Sidebar", "Collapsed view setup complete");
}

int Sidebar::sidebarWidth() const
{
    return m_expanded ? width() : 40;
}

void Sidebar::setSidebarWidth(int width)
{
    if (m_expanded) {
        setMinimumWidth(qMax(200, width));
        setMaximumWidth(qMin(500, width));
        resize(width, height());
    }
}

void Sidebar::setExpanded(bool expanded)
{
    if (m_expanded != expanded) {
        m_expanded = expanded;
        
        if (expanded) {
            setMinimumWidth(200);
            setMaximumWidth(500);
            m_stackWidget->setCurrentWidget(m_expandedWidget);
        } else {
            setMinimumWidth(40);
            setMaximumWidth(40);
            m_stackWidget->setCurrentWidget(m_collapsedWidget);
        }
        
        LOG_INFO("Sidebar", QString("Sidebar %1").arg(expanded ? "expanded" : "collapsed"));
        
        emit sizeChanged();
    }
}

bool Sidebar::isExpanded() const
{
    return m_expanded;
}

void Sidebar::refreshTags()
{
    loadTags();
}

void Sidebar::onItemClicked(QListWidgetItem *item)
{
    if (!item) {
        return;
    }

    QListWidget *owner = item->listWidget();
    if (owner == m_tagsList) {
        int tagId = item->data(Qt::UserRole).toInt();
        emit tagSelected(tagId, item->text());
        LOG_INFO("Sidebar", QString("Tag selected: %1 (%2)").arg(item->text()).arg(tagId));
        return;
    }

    QString group = item->text();
    emit groupChanged(group);
    LOG_INFO("Sidebar", QString("Group changed to: %1").arg(group));
}

void Sidebar::onNewFolderClicked()
{
    createFolder();
}

void Sidebar::setupTags()
{
    m_tagsTitle = new QLabel("标签", this);
    m_tagsTitle->setStyleSheet("font-weight: bold; padding: 10px 15px 5px;");

    m_tagsList = new QListWidget(this);
    m_tagsList->setFrameStyle(QFrame::NoFrame);
    m_tagsList->setStyleSheet(
        "QListWidget { background: transparent; }"
        "QListWidget::item { padding: 8px 15px; border-radius: 4px; }"
        "QListWidget::item:hover { background: rgba(59, 130, 246, 0.1); }"
        "QListWidget::item:selected { background: rgba(59, 130, 246, 0.2); }"
    );
    m_tagsList->setMaximumHeight(150);

    connect(m_tagsList, &QListWidget::itemClicked, this, &Sidebar::onItemClicked);

    loadTags();
    LOG_INFO("Sidebar", "Tags setup complete");
}

void Sidebar::loadFolders()
{
    m_foldersList->clear();

    QList<Folder> folders = Database::instance().getAllFolders();
    for (const Folder &folder : folders) {
        QListWidgetItem *item = new QListWidgetItem(folder.name(), m_foldersList);
        item->setData(Qt::UserRole, folder.id());
        m_foldersList->addItem(item);
    }
}

void Sidebar::loadTags()
{
    m_tagsList->clear();

    QList<Tag> tags = Database::instance().getAllTags();
    for (const Tag &tag : tags) {
        QListWidgetItem *item = new QListWidgetItem(tag.name(), m_tagsList);
        item->setData(Qt::UserRole, tag.id());
        item->setForeground(QColor(tag.color()));
        m_tagsList->addItem(item);
    }
}

void Sidebar::createFolder()
{
    bool ok;
    QString folderName = QInputDialog::getText(this, "新建文件夹", "文件夹名称:", QLineEdit::Normal, "", &ok);

    if (ok && !folderName.isEmpty()) {
        Folder folder(0, folderName);
        folder.setPosition(m_foldersList->count());

        if (Database::instance().insertFolder(folder)) {
            loadFolders();
            LOG_INFO("Sidebar", QString("Folder created: %1").arg(folderName));
        } else {
            QMessageBox::warning(this, "错误", "创建文件夹失败");
        }
    }
}

void Sidebar::renameFolder(QListWidgetItem *item)
{
    if (!item) {
        return;
    }

    int folderId = item->data(Qt::UserRole).toInt();
    QString oldName = item->text();

    bool ok;
    QString newName = QInputDialog::getText(this, "重命名文件夹", "新名称:", QLineEdit::Normal, oldName, &ok);

    if (ok && !newName.isEmpty() && newName != oldName) {
        Folder folder = Database::instance().getFolderById(folderId);
        folder.setName(newName);

        if (Database::instance().updateFolder(folder)) {
            loadFolders();
            LOG_INFO("Sidebar", QString("Folder renamed from %1 to %2").arg(oldName).arg(newName));
        } else {
            QMessageBox::warning(this, "错误", "重命名文件夹失败");
        }
    }
}

void Sidebar::deleteFolder(QListWidgetItem *item)
{
    if (!item) {
        return;
    }

    int folderId = item->data(Qt::UserRole).toInt();
    QString folderName = item->text();

    auto reply = QMessageBox::question(this, "确认删除",
        QString("确定要删除文件夹 \"%1\" 吗?\n\n文件夹内的任务不会被删除。").arg(folderName),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (Database::instance().deleteFolder(folderId)) {
            loadFolders();
            LOG_INFO("Sidebar", QString("Folder deleted: %1").arg(folderName));
        } else {
            QMessageBox::warning(this, "错误", "删除文件夹失败");
        }
    }
}
