#include "task_tree.h"
#include "../models/task.h"
#include "../controllers/database.h"
#include "../controllers/task_controller.h"
#include <QHeaderView>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QMessageBox>
#include <QDate>
#include <QSqlQuery>
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QPen>
#include <QColor>
#include <QStyle>
#include <QFontMetrics>
#include <functional>
#include <QRegularExpression>
#include <QSqlError>
#include "../utils/theme_manager.h"
#include "../utils/theme_utils.h"

namespace {
constexpr int RoleTaskId = Qt::UserRole;
constexpr int RoleCompleted = Qt::UserRole + 1;
constexpr int RoleHasChildren = Qt::UserRole + 2;
constexpr int RoleSourceInfo = Qt::UserRole + 3;

QString buildFtsQuery(const QString &text)
{
    QString cleaned = text;
    cleaned.replace(QRegularExpression(R"(["':*])"), " ");
    QStringList terms = cleaned.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
    for (QString &term : terms) {
        term = term.trimmed();
        if (!term.endsWith('*')) {
            term.append('*');
        }
    }
    return terms.join(" AND ");
}
}

TaskTreeItemDelegate::TaskTreeItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

QRect TaskTreeItemDelegate::checkboxRect(const QStyleOptionViewItem &option) const
{
    const int size = 20;
    const int left = option.rect.left() + 16;
    const int top = option.rect.top() + (option.rect.height() - size) / 2;
    return QRect(left, top, size, size);
}

void TaskTreeItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    opt.showDecorationSelected = false;
    const bool isSelected = opt.state & QStyle::State_Selected;
    const bool isHovered = opt.state & QStyle::State_MouseOver;
    opt.state &= ~QStyle::State_Selected;

    QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
    opt.text.clear();
    opt.icon = QIcon();
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

    const QRect cardRect = option.rect.adjusted(8, 6, -8, -6);
    const QRect cbRect = checkboxRect(option);

    ThemeUtils::Theme utilsTheme = ThemeUtils::Light;
    ThemeManager::Theme currentTheme = ThemeManager::instance().currentTheme();
    if (currentTheme == ThemeManager::Dark) {
        utilsTheme = ThemeUtils::Dark;
    } else if (currentTheme == ThemeManager::System) {
        const QColor textColor = opt.palette.color(QPalette::Text);
        const bool textIsLight = ThemeUtils::isColorLight(textColor);
        utilsTheme = textIsLight ? ThemeUtils::Dark : ThemeUtils::Light;
    }

    QColor cardBg = ThemeUtils::getSurfaceColor(utilsTheme);
    QColor borderColor = ThemeUtils::getBorderColor(utilsTheme);
    QColor accent = ThemeUtils::getPrimaryColor(utilsTheme);
    if (isHovered) {
        QColor hover = accent;
        hover.setAlpha(utilsTheme == ThemeUtils::Dark ? 30 : 20);
        const QString blended = ThemeUtils::blendColors(cardBg, hover, 0.08);
        cardBg = QColor(blended);
        borderColor = accent.lighter(130);
    }
    if (isSelected) {
        borderColor = accent;
    }

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setBrush(cardBg);
    painter->setPen(QPen(borderColor, 1));
    painter->drawRoundedRect(cardRect, 8, 8);

    if (isSelected) {
        QColor highlight = accent;
        highlight.setAlpha(utilsTheme == ThemeUtils::Dark ? 45 : 35);
        painter->setBrush(highlight);
        painter->setPen(Qt::NoPen);
        painter->drawRoundedRect(cardRect.adjusted(1, 1, -1, -1), 7, 7);
    }
    painter->restore();

    const bool completed = index.data(RoleCompleted).toBool();

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    QColor checkboxBorderColor = completed ? QColor("#3B82F6") : QColor("#CBD5E1");
    QColor fillColor = completed ? QColor("#3B82F6") : QColor("#FFFFFF");

    painter->setPen(QPen(checkboxBorderColor, 1.5));
    painter->setBrush(fillColor);
    painter->drawRoundedRect(cbRect, 3, 3);

    if (completed) {
        QPen checkPen(Qt::white, 2.0);
        painter->setPen(checkPen);
        QPoint p1(cbRect.left() + 4, cbRect.center().y());
        QPoint p2(cbRect.left() + 7, cbRect.bottom() - 4);
        QPoint p3(cbRect.right() - 3, cbRect.top() + 4);
        painter->drawLine(p1, p2);
        painter->drawLine(p2, p3);
    }

    painter->restore();

    const QString titleText = index.data(Qt::DisplayRole).toString();
    const QString sourceInfo = index.data(RoleSourceInfo).toString();

    QRect textRect = cardRect;
    textRect.setLeft(cbRect.right() + 16);
    textRect.setRight(cardRect.right() - 6);

    // 添加优先级指示器
    int taskId = index.data(RoleTaskId).toInt();
    if (taskId > 0) {
        TaskController controller;
        Task task = controller.getTaskById(taskId);
        if (task.id() > 0) {
            QString priorityText;
            QString priorityColor;
            int priority = task.priority();

            switch (priority) {
                case 3:
                    priorityText = "高";
                    priorityColor = "#EF4444";
                    break;
                case 2:
                    priorityText = "中";
                    priorityColor = "#F59E0B";
                    break;
                case 1:
                    priorityText = "低";
                    priorityColor = "#10B981";
                    break;
                default:
                    priorityText = "";
                    priorityColor = "";
                    break;
            }

            if (!priorityText.isEmpty()) {
                const int prioritySize = 30;
                const int priorityRight = cardRect.right() - 8;
                const int priorityTop = cardRect.top() + (cardRect.height() - prioritySize) / 2;
                QRect priorityRect = QRect(
                    priorityRight - prioritySize + 1,
                    priorityTop,
                    prioritySize,
                    prioritySize
                );
                painter->save();
                painter->setBrush(QColor(priorityColor));
                painter->setPen(Qt::NoPen);
                painter->drawRoundedRect(priorityRect, 4, 4);
                painter->setPen(Qt::white);
                QFont priorityFont = opt.font;
                priorityFont.setPointSize(12);
                priorityFont.setBold(true);
                painter->setFont(priorityFont);
                painter->drawText(priorityRect, Qt::AlignCenter, priorityText);
                painter->restore();

                // 调整文本绘制区域
                textRect.setWidth(textRect.width() - (prioritySize + 12));
            }
        }
    }

    painter->save();
    QColor textColor = opt.palette.color(QPalette::Text);
    QVariant fg = index.data(Qt::ForegroundRole);
    if (fg.canConvert<QBrush>()) {
        textColor = fg.value<QBrush>().color();
    }

    QFont titleFont = opt.font;
    const int basePoint = titleFont.pointSize() > 0 ? titleFont.pointSize() : 14;
    titleFont.setPointSize(basePoint );
    titleFont.setBold(false);
    QFont sourceFont = opt.font;
    sourceFont.setPointSize(qMax(10, basePoint - 1));

    if (sourceInfo.isEmpty()) {
        painter->setFont(titleFont);
        painter->setPen(textColor);
        painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, titleText);
    } else {
        QFontMetrics titleFm(titleFont);
        QFontMetrics sourceFm(sourceFont);
        const QString sourceText = QString("来源: %1").arg(sourceInfo);
        const int rightPadding = 8;
        int sourceWidth = sourceFm.horizontalAdvance(sourceText);
        int rightEdge = textRect.right() - rightPadding;
        int sourceLeft = qMax(textRect.left(), rightEdge - sourceWidth + 1);

        QRect sourceRect(sourceLeft, textRect.top(), rightEdge - sourceLeft + 1, textRect.height());
        QRect titleRect(textRect.left(), textRect.top(), sourceLeft - textRect.left() - 8, textRect.height());

        if (titleRect.width() < 0) {
            titleRect.setWidth(0);
        }

        painter->setFont(titleFont);
        painter->setPen(textColor);
        const QString elidedTitle = titleFm.elidedText(titleText, Qt::ElideRight, titleRect.width());
        painter->drawText(titleRect, Qt::AlignVCenter | Qt::AlignLeft, elidedTitle);

        QColor secondary = textColor;
        secondary.setAlpha(180);
        painter->setFont(sourceFont);
        painter->setPen(secondary);
        painter->drawText(sourceRect, Qt::AlignVCenter | Qt::AlignRight, sourceText);
    }
    painter->restore();
}

QSize TaskTreeItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    int height = qMax(size.height(), option.fontMetrics.height() + 44);
    return QSize(size.width(), height);
}

bool TaskTreeItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    Q_UNUSED(model);
    if (event->type() == QEvent::MouseButtonRelease) {
        auto *mouseEvent = static_cast<QMouseEvent *>(event);
        if (checkboxRect(option).contains(mouseEvent->pos())) {
            int taskId = index.data(RoleTaskId).toInt();
            if (taskId > 0) {
                emit toggleRequested(taskId);
                return true;
            }
        }
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

TaskTree::TaskTree(TaskController *controller, QWidget *parent)
    : QWidget(parent)
    , m_controller(controller)
    , m_treeView(nullptr)
    , m_treeModel(nullptr)
    , m_contextMenu(nullptr)
    , m_currentGroup("所有任务")
    , m_currentTagId(0)
    , m_currentFolderId(0)
    , m_searchFilters()
{
    setupUI();
    setupContextMenu();
    connect(m_controller, &TaskController::taskAdded, this, &TaskTree::refreshTasks);
    connect(m_controller, &TaskController::taskUpdated, this, &TaskTree::refreshTasks);
    connect(m_controller, &TaskController::taskDeleted, this, [this](int taskId) {
        Q_UNUSED(taskId);
        refreshTasks();
    });
    connect(m_controller, &TaskController::taskCompletionChanged, this, &TaskTree::refreshTasks);
}

TaskTree::~TaskTree()
{
}

void TaskTree::setupUI()
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_treeView = new QTreeView(this);
    m_treeView->setObjectName("taskTreeView");
    m_treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    
    m_treeModel = new QStandardItemModel(this);
    m_treeModel->setHorizontalHeaderLabels(QStringList() << "任务");
    
    m_treeView->setModel(m_treeModel);
    m_treeView->setHeaderHidden(true);
    m_treeView->setIndentation(20);
    m_treeView->setAnimated(true);
    m_treeView->setExpandsOnDoubleClick(false);
    m_treeView->setRootIsDecorated(false);
    m_treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_treeView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_treeView->setDragEnabled(true);
    m_treeView->setAcceptDrops(true);
    m_treeView->setDropIndicatorShown(true);
    m_treeView->setDragDropMode(QAbstractItemView::InternalMove);
    m_treeView->setUniformRowHeights(false);
    
    m_treeView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_treeView->header()->setStretchLastSection(true);

    auto *delegate = new TaskTreeItemDelegate(m_treeView);
    connect(delegate, &TaskTreeItemDelegate::toggleRequested, this, [this](int taskId) {
        m_controller->toggleTaskCompletion(taskId);
    });
    m_treeView->setItemDelegate(delegate);

    connect(m_treeView, &QTreeView::doubleClicked, this, &TaskTree::onItemDoubleClicked);
    connect(m_treeView, &QTreeView::clicked, this, &TaskTree::onItemClicked);
    connect(m_treeView, &QTreeView::customContextMenuRequested, this, &TaskTree::onContextMenu);
    connect(m_treeView, &QTreeView::expanded, this, &TaskTree::onExpandItem);
    connect(m_treeView, &QTreeView::collapsed, this, &TaskTree::onCollapseItem);
    
    layout->addWidget(m_treeView);
}

void TaskTree::setupContextMenu()
{
    m_contextMenu = new QMenu(this);
    
    m_expandAction = new QAction("展开", this);
    connect(m_expandAction, &QAction::triggered, this, [this]() {
        QModelIndex index = m_treeView->currentIndex();
        if (index.isValid()) {
            m_treeView->expand(index);
        }
    });
    
    m_collapseAction = new QAction("折叠", this);
    connect(m_collapseAction, &QAction::triggered, this, [this]() {
        QModelIndex index = m_treeView->currentIndex();
        if (index.isValid()) {
            m_treeView->collapse(index);
        }
    });
    
    m_addSubtaskAction = new QAction("添加子任务", this);
    connect(m_addSubtaskAction, &QAction::triggered, this, [this]() {
        QModelIndex index = m_treeView->currentIndex();
        if (index.isValid()) {
            Task parentTask = getTaskFromIndex(index);
            if (parentTask.id() > 0) {
                emit contextMenuRequested(QPoint(0, 0), parentTask.id());
            }
        }
    });
    
    m_editAction = new QAction("编辑", this);
    connect(m_editAction, &QAction::triggered, this, [this]() {
        QModelIndex index = m_treeView->currentIndex();
        if (index.isValid()) {
            Task task = getTaskFromIndex(index);
            if (task.id() > 0) {
                emit contextMenuRequested(QPoint(0, 0), task.id());
            }
        }
    });
    
    m_deleteAction = new QAction("删除", this);
    connect(m_deleteAction, &QAction::triggered, this, [this]() {
        QModelIndex index = m_treeView->currentIndex();
        if (index.isValid()) {
            Task task = getTaskFromIndex(index);
            if (task.id() > 0) {
                QMessageBox::StandardButton reply = QMessageBox::question(
                    this, "确认删除",
                    "确定要删除这个任务吗？",
                    QMessageBox::Yes | QMessageBox::No);
                if (reply == QMessageBox::Yes) {
                    m_controller->deleteTask(task.id());
                }
            }
        }
    });
    
    m_completeAction = new QAction("标记完成", this);
    connect(m_completeAction, &QAction::triggered, this, [this]() {
        QModelIndex index = m_treeView->currentIndex();
        if (index.isValid()) {
            Task task = getTaskFromIndex(index);
            if (task.id() > 0) {
                m_controller->toggleTaskCompletion(task.id());
            }
        }
    });
    
    m_contextMenu->addAction(m_expandAction);
    m_contextMenu->addAction(m_collapseAction);
    m_contextMenu->addSeparator();
    m_contextMenu->addAction(m_addSubtaskAction);
    m_contextMenu->addAction(m_editAction);
    m_contextMenu->addAction(m_completeAction);
    m_contextMenu->addSeparator();
    m_contextMenu->addAction(m_deleteAction);
}

QStandardItem* TaskTree::createTaskItem(const Task &task, const QString &sourceInfo, const QString &sourceTooltip)
{
    QList<QStandardItem*> items;
    QStandardItem *item = new QStandardItem(task.title());
    item->setData(task.id(), RoleTaskId);
    item->setData(task.isCompleted(), RoleCompleted);
    item->setData(task.hasChildren(), RoleHasChildren);
    if (!sourceInfo.isEmpty()) {
        item->setData(sourceInfo, RoleSourceInfo);
        if (!sourceTooltip.isEmpty()) {
            item->setToolTip(sourceTooltip);
        }
    }
    
    if (task.isCompleted()) {
        item->setForeground(QBrush(QColor("#888888")));
    }
    
    items << item;
    return item;
}

void TaskTree::loadChildTasks(int parentId, QStandardItem *parentItem)
{
    QList<Task> subtasks = m_controller->getSubTasks(parentId);
    
    for (int i = parentItem->rowCount() - 1; i >= 0; --i) {
        QStandardItem *child = parentItem->child(i);
            if (child->text() == "加载中...") {
            parentItem->removeRow(i);
        }
    }
    
    for (const Task &task : subtasks) {
        QStandardItem *childItem = createTaskItem(task);
        parentItem->appendRow(childItem);
        m_tasksCache[task.id()] = task;
        
    }
}

void TaskTree::loadAllTasks()
{
    m_treeModel->clear();
        m_treeModel->setHorizontalHeaderLabels(QStringList() << "任务");
    m_tasksCache.clear();
    
    QList<Task> allTasks = m_controller->getTaskHierarchy(0);
    
    QMap<int, QStandardItem*> itemMap;
    
    for (const Task &task : allTasks) {
        QStandardItem *item = createTaskItem(task);
        m_tasksCache[task.id()] = task;
        itemMap[task.id()] = item;
        
        if (task.parentId() == 0 || task.parentId() == -1) {
            m_treeModel->appendRow(item);
        }
    }
    
    for (const Task &task : allTasks) {
        if (task.parentId() > 0 && itemMap.contains(task.parentId())) {
            QStandardItem *parentItem = itemMap[task.parentId()];
            QStandardItem *childItem = itemMap[task.id()];
            parentItem->appendRow(childItem);
        }
    }

    emit taskCountChanged(m_treeModel->rowCount());
}

void TaskTree::loadFilteredTasks(const QString &group, int tagId, const TaskSearchFilters &filters, int folderId)
{
    m_treeModel->clear();
    m_treeModel->setHorizontalHeaderLabels(QStringList() << "任务");
    m_tasksCache.clear();
    
    QList<Task> tasks;
    QString whereClause;
    bool showDeleted = false;
    bool isRecycleBin = (group == "回收站");
    
    if (group == "今天") {
        whereClause = "date(t.created_at) = date('now', 'localtime')";
    } else if (group == "本周") {
        whereClause = "strftime('%Y-%W', t.created_at) = strftime('%Y-%W', 'now', 'localtime')";
    } else if (group == "本月") {
        whereClause = "strftime('%Y-%m', t.created_at) = strftime('%Y-%m', 'now', 'localtime')";
    } else if (group == "已过期") {
        whereClause = "t.due_date < datetime('now', 'localtime') AND t.completed = 0";
    } else if (group == "高优先级") {
        whereClause = "t.priority = 3";
    } else if (group == "中优先级") {
        whereClause = "t.priority = 2";
    } else if (group == "低优先级") {
        whereClause = "t.priority = 1";
    } else if (group == "已完成") {
        whereClause = "t.completed = 1";
    } else if (group == "未完成") {
        whereClause = "t.completed = 0";
    } else if (group == "进行中") {
        whereClause = "t.completed = 0 AND t.progress > 0";
    } else if (group == "所有任务") {
        whereClause = "1=1";
    } else {
        if (tagId <= 0 && folderId <= 0 && !isRecycleBin) {
            loadAllTasks();
            return;
        }
        whereClause = "1=1";
    }

    if (isRecycleBin) {
        whereClause = "1=1";
        showDeleted = true;
    }

    QStringList joins;
    QStringList conditions;
    QVariantList bindValues;
    
    if (!whereClause.isEmpty()) {
        conditions << whereClause;
    }
    
    QList<int> tagIds = filters.tagIds;
    if (tagId > 0) {
        tagIds.clear();
        tagIds.append(tagId);
    }
    if (!tagIds.isEmpty()) {
        joins << "INNER JOIN task_tags tt ON tt.task_id = t.id";
        QStringList placeholders;
        for (int i = 0; i < tagIds.size(); ++i) {
            placeholders << "?";
        }
        conditions << QString("tt.tag_id IN (%1)").arg(placeholders.join(","));
        for (int id : tagIds) {
            bindValues << id;
        }
    }

    if (folderId > 0) {
        joins << "INNER JOIN task_folders tf ON tf.task_id = t.id";
        conditions << "tf.folder_id = ?";
        bindValues << folderId;
    }
    
    if (filters.priority > 0) {
        conditions << "t.priority = ?";
        bindValues << filters.priority;
    }
    
    switch (filters.status) {
    case TaskSearchStatusFilter::Completed:
        conditions << "t.completed = 1";
        break;
    case TaskSearchStatusFilter::Incomplete:
        conditions << "t.completed = 0";
        break;
    case TaskSearchStatusFilter::InProgress:
        conditions << "t.completed = 0 AND t.progress > 0";
        break;
    default:
        break;
    }
    
    switch (filters.date) {
    case TaskSearchDateFilter::Today:
        conditions << "date(t.due_date) = date('now', 'localtime')";
        break;
    case TaskSearchDateFilter::ThisWeek:
        conditions << "strftime('%Y-%W', t.due_date) = strftime('%Y-%W', 'now', 'localtime')";
        break;
    case TaskSearchDateFilter::ThisMonth:
        conditions << "strftime('%Y-%m', t.due_date) = strftime('%Y-%m', 'now', 'localtime')";
        break;
    case TaskSearchDateFilter::Overdue:
        conditions << "t.due_date < datetime('now', 'localtime') AND t.completed = 0";
        break;
    default:
        break;
    }
    
    const QString rawText = filters.text.trimmed();
    const QString ftsQuery = buildFtsQuery(rawText);
    const bool hasText = !rawText.isEmpty();
    bool useFts = hasText && !ftsQuery.isEmpty();
    bool useLike = hasText && !useFts;
    
    auto buildOrderClause = [&filters]() -> QString {
        switch (filters.sort) {
        case TaskSearchSort::CreatedDesc:
            return "ORDER BY t.created_at DESC";
        case TaskSearchSort::DueDateAsc:
            return "ORDER BY CASE WHEN t.due_date IS NULL OR t.due_date = '' THEN 1 ELSE 0 END, t.due_date ASC";
        case TaskSearchSort::DueDateDesc:
            return "ORDER BY CASE WHEN t.due_date IS NULL OR t.due_date = '' THEN 1 ELSE 0 END, t.due_date DESC";
        case TaskSearchSort::PriorityDesc:
            return "ORDER BY t.priority DESC, t.created_at DESC";
        case TaskSearchSort::PriorityAsc:
            return "ORDER BY t.priority ASC, t.created_at DESC";
        case TaskSearchSort::Manual:
        default:
            return "ORDER BY t.created_at DESC";
        }
    };
    
    auto runQuery = [&](bool ftsMode, bool likeMode) -> bool {
        QStringList queryJoins = joins;
        QStringList queryConditions = conditions;
        QVariantList queryBinds = bindValues;
        
        if (ftsMode) {
            queryJoins << "INNER JOIN tasks_fts f ON f.rowid = t.id";
            queryConditions << "f MATCH ?";
            queryBinds << ftsQuery;
        } else if (likeMode) {
            queryConditions << "(lower(t.title) LIKE ? OR lower(t.description) LIKE ?)";
            const QString pattern = "%" + rawText.toLower() + "%";
            queryBinds << pattern << pattern;
        }
        
        const QString orderClause = buildOrderClause();
        const QString deletedPredicate = showDeleted ? "t.is_deleted = 1" : "t.is_deleted = 0";
        const bool useDistinct = !tagIds.isEmpty();
        const QString distinctClause = useDistinct ? "DISTINCT " : "";
        const QString joinClause = queryJoins.join(" ");
        const QString whereClauseCombined = queryConditions.isEmpty() ? QString("1=1") : queryConditions.join(" AND ");
        const QString queryStr = QString(R"(
            SELECT %1t.id, t.title, t.description, t.priority, t.due_date, t.completed, t.progress, t.parent_id, t.created_at, t.updated_at,
                   CASE WHEN EXISTS(SELECT 1 FROM tasks child WHERE child.parent_id = t.id AND child.is_deleted = 0) THEN 1 ELSE 0 END as has_children
            FROM tasks t
            %2
            WHERE %3 AND %4
            %5
        )").arg(distinctClause, joinClause, deletedPredicate, whereClauseCombined, orderClause);
        
        QSqlQuery query(Database::instance().database());
        query.prepare(queryStr);
        for (const QVariant &value : queryBinds) {
            query.addBindValue(value);
        }
        if (!query.exec()) {
            return false;
        }
        
        tasks.clear();
        while (query.next()) {
            Task task;
            task.setId(query.value(0).toInt());
            task.setTitle(query.value(1).toString());
            task.setDescription(query.value(2).toString());
            task.setPriority(query.value(3).toInt());
            task.setDueDate(QDateTime::fromString(query.value(4).toString(), Qt::ISODate));
            task.setCompleted(query.value(5).toBool());
            task.setProgress(query.value(6).toDouble());
            task.setParentId(query.value(7).toInt());
            task.setCreatedAt(QDateTime::fromString(query.value(8).toString(), Qt::ISODate));
            task.setUpdatedAt(QDateTime::fromString(query.value(9).toString(), Qt::ISODate));
            task.setHasChildren(query.value(10).toBool());
            tasks.append(task);
        }
        return true;
    };
    
    if (!runQuery(useFts, useLike) && useFts) {
        runQuery(false, true);
    }
    
    QMap<int, QStandardItem*> itemMap;
    QSet<int> taskIds;
    for (const Task &task : tasks) {
        taskIds.insert(task.id());
    }
    
    QMap<int, Task> parentCache;
    auto buildSourceInfo = [&](int parentId, QString *tooltipOut) -> QString {
        if (parentId <= 0) {
            return QString();
        }
        Task parentTask;
        if (parentCache.contains(parentId)) {
            parentTask = parentCache.value(parentId);
        } else {
            parentTask = m_controller->getTaskById(parentId);
            parentCache.insert(parentId, parentTask);
        }
        if (parentTask.id() <= 0) {
            return QString();
        }
        QString timeStr;
        if (parentTask.createdAt().isValid()) {
            timeStr = parentTask.createdAt().toString("yyyy-MM-dd HH:mm");
        }
        if (tooltipOut) {
            if (timeStr.isEmpty()) {
                *tooltipOut = QString("父任务：%1").arg(parentTask.title());
            } else {
                *tooltipOut = QString("父任务：%1\n创建时间：%2").arg(parentTask.title(), timeStr);
            }
        }
        if (timeStr.isEmpty()) {
            return parentTask.title();
        }
        return QString("%1 / %2").arg(timeStr, parentTask.title());
    };
    
    for (const Task &task : tasks) {
        QString sourceInfo;
        QString sourceTooltip;
        if (task.parentId() > 0 && !taskIds.contains(task.parentId())) {
            sourceInfo = buildSourceInfo(task.parentId(), &sourceTooltip);
        }
        QStandardItem *item = createTaskItem(task, sourceInfo, sourceTooltip);
        m_tasksCache[task.id()] = task;
        itemMap[task.id()] = item;
    }
    
    for (const Task &task : tasks) {
        if (task.parentId() <= 0 || !itemMap.contains(task.parentId())) {
            m_treeModel->appendRow(itemMap[task.id()]);
        }
    }
    
    for (const Task &task : tasks) {
        if (task.parentId() > 0 && itemMap.contains(task.parentId())) {
            QStandardItem *parentItem = itemMap[task.parentId()];
            QStandardItem *childItem = itemMap[task.id()];
            parentItem->appendRow(childItem);
        }
    }
    
    emit taskCountChanged(m_treeModel->rowCount());
}

void TaskTree::loadTasks()
{
    loadTasks("所有任务", 0, TaskSearchFilters(), 0);
}

void TaskTree::loadTasks(const QString &group)
{
    loadTasks(group, 0, TaskSearchFilters(), 0);
}

void TaskTree::loadTasks(const QString &group, int tagId)
{
    loadTasks(group, tagId, m_searchFilters, 0);
}

void TaskTree::loadTasks(const QString &group, int tagId, const TaskSearchFilters &filters)
{
    loadTasks(group, tagId, filters, 0);
}

void TaskTree::loadTasks(const QString &group, int tagId, const TaskSearchFilters &filters, int folderId)
{
    m_currentGroup = group;
    m_currentTagId = tagId;
    m_currentFolderId = folderId;
    m_searchFilters = filters;

    if (group == "所有任务" && tagId <= 0 && folderId <= 0 && !filters.hasActiveFilters()) {
        loadAllTasks();
        return;
    }

    loadFilteredTasks(group, tagId, filters, folderId);
}

void TaskTree::refreshTasks()
{
    QSet<int> expandedIds = collectExpandedTaskIds();
    int selectedId = -1;
    QModelIndex currentIndex = m_treeView->currentIndex();
    if (currentIndex.isValid()) {
        selectedId = currentIndex.data(RoleTaskId).toInt();
    }

    loadTasks(m_currentGroup, m_currentTagId, m_searchFilters, m_currentFolderId);
    restoreExpandedTaskIds(expandedIds);

    if (selectedId > 0) {
        QModelIndex idx = findIndexByTaskId(selectedId);
        if (idx.isValid()) {
            m_treeView->setCurrentIndex(idx);
        }
    }
}

void TaskTree::expandAll()
{
    m_treeView->expandAll();
}

void TaskTree::collapseAll()
{
    m_treeView->collapseAll();
}

void TaskTree::clearSelection()
{
    if (!m_treeView) {
        return;
    }
    m_treeView->clearSelection();
    m_treeView->setCurrentIndex(QModelIndex());
}

Task TaskTree::getTaskFromIndex(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Task();
    }
    
    QStandardItem *item = m_treeModel->itemFromIndex(index);
    if (!item) {
        return Task();
    }
    
    int taskId = item->data(RoleTaskId).toInt();
    if (m_tasksCache.contains(taskId)) {
        return m_tasksCache[taskId];
    }
    
    return m_controller->getTaskById(taskId);
}

QSet<int> TaskTree::collectExpandedTaskIds() const
{
    QSet<int> ids;
    if (!m_treeModel || !m_treeView) {
        return ids;
    }

    std::function<void(QStandardItem*)> visitItem = [&](QStandardItem *item) {
        if (!item) {
            return;
        }
        QModelIndex idx = item->index();
        if (m_treeView->isExpanded(idx)) {
            int id = item->data(RoleTaskId).toInt();
            if (id > 0) {
                ids.insert(id);
            }
        }
        for (int i = 0; i < item->rowCount(); ++i) {
            visitItem(item->child(i));
        }
    };

    for (int row = 0; row < m_treeModel->rowCount(); ++row) {
        visitItem(m_treeModel->item(row));
    }

    return ids;
}

void TaskTree::restoreExpandedTaskIds(const QSet<int> &ids)
{
    if (!m_treeModel || !m_treeView || ids.isEmpty()) {
        return;
    }

    std::function<void(QStandardItem*)> visitItem = [&](QStandardItem *item) {
        if (!item) {
            return;
        }
        int id = item->data(RoleTaskId).toInt();
        if (ids.contains(id)) {
            m_treeView->expand(item->index());
        }
        for (int i = 0; i < item->rowCount(); ++i) {
            visitItem(item->child(i));
        }
    };

    for (int row = 0; row < m_treeModel->rowCount(); ++row) {
        visitItem(m_treeModel->item(row));
    }
}

QModelIndex TaskTree::findIndexByTaskId(int taskId) const
{
    if (!m_treeModel || taskId <= 0) {
        return QModelIndex();
    }

    std::function<QModelIndex(QStandardItem*)> findInItem = [&](QStandardItem *item) -> QModelIndex {
        if (!item) {
            return QModelIndex();
        }
        int id = item->data(RoleTaskId).toInt();
        if (id == taskId) {
            return item->index();
        }
        for (int i = 0; i < item->rowCount(); ++i) {
            QModelIndex found = findInItem(item->child(i));
            if (found.isValid()) {
                return found;
            }
        }
        return QModelIndex();
    };

    for (int row = 0; row < m_treeModel->rowCount(); ++row) {
        QModelIndex found = findInItem(m_treeModel->item(row));
        if (found.isValid()) {
            return found;
        }
    }

    return QModelIndex();
}

void TaskTree::onItemDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }
    
    Task task = getTaskFromIndex(index);
    if (task.id() > 0) {
        emit taskDoubleClicked(task.id());
    }
    
    if (m_treeView->isExpanded(index)) {
        m_treeView->collapse(index);
    } else {
        m_treeView->expand(index);
    }
}

void TaskTree::onItemClicked(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }
    
    Task task = getTaskFromIndex(index);
    if (task.id() > 0) {
        emit taskSelected(task.id());
    }
}

void TaskTree::onContextMenu(const QPoint &pos)
{
    QModelIndex index = m_treeView->indexAt(pos);
    if (index.isValid()) {
        m_treeView->setCurrentIndex(index);
        Task task = getTaskFromIndex(index);
        if (task.id() > 0) {
            emit contextMenuRequested(pos, task.id());
        }
    }
}

void TaskTree::onExpandItem(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }
    
    QStandardItem *item = m_treeModel->itemFromIndex(index);
    if (!item) {
        return;
    }
    
    if (item->rowCount() == 1 && item->child(0)->text() == "加载中...") {
    int taskId = item->data(RoleTaskId).toInt();
        loadChildTasks(taskId, item);
    }
}

void TaskTree::onCollapseItem(const QModelIndex &index)
{
    Q_UNUSED(index);
}

void TaskTree::onDropData(const QMimeData *data, const QModelIndex &index)
{
    Q_UNUSED(data);
    Q_UNUSED(index);
}
