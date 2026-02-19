#ifndef SEARCH_WIDGET_H
#define SEARCH_WIDGET_H

#include <QWidget>
#include <QSet>
#include "../models/task_search_filters.h"

class QLineEdit;
class QPushButton;
class QComboBox;
class QToolButton;
class QMenu;
class TaskController;
class Tag;

class SearchWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SearchWidget(TaskController *controller, QWidget *parent = nullptr);

    TaskSearchFilters filters() const;
    void setSearchText(const QString &text);
    void setSelectedTags(const QList<int> &tagIds, bool emitSignal = true);
    void clearFilters();

signals:
    void filtersChanged(const TaskSearchFilters &filters);

private slots:
    void onSearchTextChanged(const QString &text);
    void onToggleFilters();
    void onClearFilters();
    void onFilterControlChanged();
    void onTagActionToggled(bool checked);
    void reloadTags();

private:
    void setupUI();
    void rebuildTagMenu();
    void updateTagButtonText();
    QList<int> selectedTagIds() const;
    void emitFiltersChanged();

    TaskController *m_controller;
    QLineEdit *m_searchEdit;
    QPushButton *m_filterToggleButton;
    QPushButton *m_clearButton;
    QWidget *m_filterPanel;
    QComboBox *m_priorityCombo;
    QComboBox *m_statusCombo;
    QComboBox *m_dateCombo;
    QComboBox *m_sortCombo;
    QToolButton *m_tagButton;
    QMenu *m_tagMenu;
    QSet<int> m_selectedTagIds;
    bool m_blockTagSignals;
};

#endif // SEARCH_WIDGET_H
