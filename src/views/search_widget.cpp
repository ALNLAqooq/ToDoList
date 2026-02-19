#include "search_widget.h"
#include "../controllers/task_controller.h"
#include "../models/tag.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QToolButton>
#include <QMenu>
#include <QAction>
#include <QLabel>

SearchWidget::SearchWidget(TaskController *controller, QWidget *parent)
    : QWidget(parent)
    , m_controller(controller)
    , m_searchEdit(nullptr)
    , m_filterToggleButton(nullptr)
    , m_clearButton(nullptr)
    , m_filterPanel(nullptr)
    , m_priorityCombo(nullptr)
    , m_statusCombo(nullptr)
    , m_dateCombo(nullptr)
    , m_sortCombo(nullptr)
    , m_tagButton(nullptr)
    , m_tagMenu(nullptr)
    , m_blockTagSignals(false)
{
    setupUI();

    if (m_controller) {
        reloadTags();
        connect(m_controller, &TaskController::tagsChanged, this, &SearchWidget::reloadTags);
    }
}

void SearchWidget::setupUI()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(6);

    auto *topRow = new QHBoxLayout();
    topRow->setSpacing(8);

    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("搜索任务...");
    m_searchEdit->setClearButtonEnabled(true);
    connect(m_searchEdit, &QLineEdit::textChanged, this, &SearchWidget::onSearchTextChanged);
    topRow->addWidget(m_searchEdit, 1);

    m_filterToggleButton = new QPushButton("筛选", this);
    m_filterToggleButton->setProperty("flat", true);
    connect(m_filterToggleButton, &QPushButton::clicked, this, &SearchWidget::onToggleFilters);
    topRow->addWidget(m_filterToggleButton);

    m_clearButton = new QPushButton("清空", this);
    m_clearButton->setProperty("flat", true);
    connect(m_clearButton, &QPushButton::clicked, this, &SearchWidget::onClearFilters);
    topRow->addWidget(m_clearButton);

    mainLayout->addLayout(topRow);

    m_filterPanel = new QWidget(this);
    auto *panelLayout = new QGridLayout(m_filterPanel);
    panelLayout->setContentsMargins(0, 0, 0, 0);
    panelLayout->setHorizontalSpacing(10);
    panelLayout->setVerticalSpacing(8);

    auto *priorityLabel = new QLabel("优先级", this);
    m_priorityCombo = new QComboBox(this);
    m_priorityCombo->addItem("全部", 0);
    m_priorityCombo->addItem("高", 3);
    m_priorityCombo->addItem("中", 2);
    m_priorityCombo->addItem("低", 1);
    connect(m_priorityCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SearchWidget::onFilterControlChanged);

    auto *statusLabel = new QLabel("状态", this);
    m_statusCombo = new QComboBox(this);
    m_statusCombo->addItem("全部", static_cast<int>(TaskSearchStatusFilter::Any));
    m_statusCombo->addItem("已完成", static_cast<int>(TaskSearchStatusFilter::Completed));
    m_statusCombo->addItem("未完成", static_cast<int>(TaskSearchStatusFilter::Incomplete));
    m_statusCombo->addItem("进行中", static_cast<int>(TaskSearchStatusFilter::InProgress));
    connect(m_statusCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SearchWidget::onFilterControlChanged);

    auto *dateLabel = new QLabel("日期", this);
    m_dateCombo = new QComboBox(this);
    m_dateCombo->addItem("全部", static_cast<int>(TaskSearchDateFilter::Any));
    m_dateCombo->addItem("今天", static_cast<int>(TaskSearchDateFilter::Today));
    m_dateCombo->addItem("本周", static_cast<int>(TaskSearchDateFilter::ThisWeek));
    m_dateCombo->addItem("本月", static_cast<int>(TaskSearchDateFilter::ThisMonth));
    m_dateCombo->addItem("已逾期", static_cast<int>(TaskSearchDateFilter::Overdue));
    connect(m_dateCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SearchWidget::onFilterControlChanged);

    auto *tagLabel = new QLabel("标签", this);
    m_tagButton = new QToolButton(this);
    m_tagButton->setPopupMode(QToolButton::InstantPopup);
    m_tagMenu = new QMenu(this);
    m_tagButton->setMenu(m_tagMenu);
    updateTagButtonText();

    auto *sortLabel = new QLabel("排序", this);
    m_sortCombo = new QComboBox(this);
    m_sortCombo->addItem("手动", static_cast<int>(TaskSearchSort::Manual));
    m_sortCombo->addItem("创建时间（新到旧）", static_cast<int>(TaskSearchSort::CreatedDesc));
    m_sortCombo->addItem("截止时间（近到远）", static_cast<int>(TaskSearchSort::DueDateAsc));
    m_sortCombo->addItem("截止时间（远到近）", static_cast<int>(TaskSearchSort::DueDateDesc));
    m_sortCombo->addItem("优先级（高到低）", static_cast<int>(TaskSearchSort::PriorityDesc));
    m_sortCombo->addItem("优先级（低到高）", static_cast<int>(TaskSearchSort::PriorityAsc));
    connect(m_sortCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SearchWidget::onFilterControlChanged);

    panelLayout->addWidget(priorityLabel, 0, 0);
    panelLayout->addWidget(m_priorityCombo, 0, 1);
    panelLayout->addWidget(statusLabel, 0, 2);
    panelLayout->addWidget(m_statusCombo, 0, 3);
    panelLayout->addWidget(dateLabel, 1, 0);
    panelLayout->addWidget(m_dateCombo, 1, 1);
    panelLayout->addWidget(tagLabel, 1, 2);
    panelLayout->addWidget(m_tagButton, 1, 3);
    panelLayout->addWidget(sortLabel, 2, 0);
    panelLayout->addWidget(m_sortCombo, 2, 1);

    m_filterPanel->setVisible(false);
    mainLayout->addWidget(m_filterPanel);
}

TaskSearchFilters SearchWidget::filters() const
{
    TaskSearchFilters filters;
    filters.text = m_searchEdit ? m_searchEdit->text() : QString();
    filters.priority = m_priorityCombo ? m_priorityCombo->currentData().toInt() : 0;
    filters.status = static_cast<TaskSearchStatusFilter>(
        m_statusCombo ? m_statusCombo->currentData().toInt() : static_cast<int>(TaskSearchStatusFilter::Any));
    filters.date = static_cast<TaskSearchDateFilter>(
        m_dateCombo ? m_dateCombo->currentData().toInt() : static_cast<int>(TaskSearchDateFilter::Any));
    filters.sort = static_cast<TaskSearchSort>(
        m_sortCombo ? m_sortCombo->currentData().toInt() : static_cast<int>(TaskSearchSort::Manual));
    filters.tagIds = selectedTagIds();
    return filters;
}

void SearchWidget::setSearchText(const QString &text)
{
    if (!m_searchEdit) {
        return;
    }
    if (m_searchEdit->text() == text) {
        return;
    }
    m_searchEdit->setText(text);
}

void SearchWidget::setSelectedTags(const QList<int> &tagIds, bool emitSignal)
{
    QSet<int> newSet;
    for (int tagId : tagIds) {
        newSet.insert(tagId);
    }
    m_selectedTagIds = newSet;

    if (m_tagMenu) {
        m_blockTagSignals = true;
        const QList<QAction *> actions = m_tagMenu->actions();
        for (QAction *action : actions) {
            if (!action || !action->isCheckable()) {
                continue;
            }
            int id = action->data().toInt();
            action->setChecked(m_selectedTagIds.contains(id));
        }
        m_blockTagSignals = false;
    }

    updateTagButtonText();
    if (emitSignal) {
        emitFiltersChanged();
    }
}

void SearchWidget::clearFilters()
{
    if (m_searchEdit) {
        m_searchEdit->clear();
    }
    if (m_priorityCombo) {
        m_priorityCombo->setCurrentIndex(0);
    }
    if (m_statusCombo) {
        m_statusCombo->setCurrentIndex(0);
    }
    if (m_dateCombo) {
        m_dateCombo->setCurrentIndex(0);
    }
    if (m_sortCombo) {
        m_sortCombo->setCurrentIndex(0);
    }
    setSelectedTags(QList<int>(), false);
    emitFiltersChanged();
}

void SearchWidget::onSearchTextChanged(const QString &text)
{
    Q_UNUSED(text);
    emitFiltersChanged();
}

void SearchWidget::onToggleFilters()
{
    if (!m_filterPanel) {
        return;
    }
    bool visible = m_filterPanel->isVisible();
    m_filterPanel->setVisible(!visible);
    m_filterToggleButton->setText(visible ? "筛选" : "收起筛选");
}

void SearchWidget::onClearFilters()
{
    clearFilters();
}

void SearchWidget::onFilterControlChanged()
{
    emitFiltersChanged();
}

void SearchWidget::onTagActionToggled(bool checked)
{
    if (m_blockTagSignals) {
        return;
    }
    QAction *action = qobject_cast<QAction *>(sender());
    if (!action) {
        return;
    }
    int id = action->data().toInt();
    if (checked) {
        m_selectedTagIds.insert(id);
    } else {
        m_selectedTagIds.remove(id);
    }
    updateTagButtonText();
    emitFiltersChanged();
}

void SearchWidget::reloadTags()
{
    if (!m_controller) {
        return;
    }

    QList<Tag> tags = m_controller->getAllTags();
    QSet<int> preserved = m_selectedTagIds;
    m_selectedTagIds.clear();

    if (m_tagMenu) {
        m_tagMenu->clear();
    }

    for (const Tag &tag : tags) {
        QAction *action = new QAction(tag.name(), m_tagMenu);
        action->setCheckable(true);
        action->setData(tag.id());
        connect(action, &QAction::toggled, this, &SearchWidget::onTagActionToggled);
        if (m_tagMenu) {
            m_tagMenu->addAction(action);
        }
        if (preserved.contains(tag.id())) {
            action->setChecked(true);
            m_selectedTagIds.insert(tag.id());
        }
    }

    if (tags.isEmpty() && m_tagMenu) {
        QAction *emptyAction = new QAction("暂无标签", m_tagMenu);
        emptyAction->setEnabled(false);
        m_tagMenu->addAction(emptyAction);
    }

    updateTagButtonText();
    emitFiltersChanged();
}

void SearchWidget::updateTagButtonText()
{
    if (!m_tagButton) {
        return;
    }
    int count = m_selectedTagIds.size();
    if (count == 0) {
        m_tagButton->setText("全部标签");
    } else if (count == 1) {
        int id = *m_selectedTagIds.constBegin();
        const QList<QAction *> actions = m_tagMenu ? m_tagMenu->actions() : QList<QAction *>();
        QString name;
        for (QAction *action : actions) {
            if (action && action->data().toInt() == id) {
                name = action->text();
                break;
            }
        }
        m_tagButton->setText(name.isEmpty() ? "1 个标签" : name);
    } else {
        m_tagButton->setText(QString("标签（%1）").arg(count));
    }
}

QList<int> SearchWidget::selectedTagIds() const
{
    QList<int> ids;
    ids.reserve(m_selectedTagIds.size());
    for (int id : m_selectedTagIds) {
        ids.append(id);
    }
    return ids;
}

void SearchWidget::emitFiltersChanged()
{
    emit filtersChanged(filters());
}
