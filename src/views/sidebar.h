#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QWidget>
#include <QVBoxLayout>
#include <QListWidget>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QLineEdit>
#include <QGroupBox>

class Sidebar : public QWidget
{
    Q_OBJECT

public:
    explicit Sidebar(QWidget *parent = nullptr);
    ~Sidebar();

    void setExpanded(bool expanded);
    bool isExpanded() const;
    
    int sidebarWidth() const;
    void setSidebarWidth(int width);
    void refreshTags();

signals:
    void groupChanged(const QString &group);
    void folderSelected(int folderId, const QString &folderName);
    void tagSelected(int tagId, const QString &tagName);
    void tagUpdated();
    void collapseRequested();
    void sizeChanged();

private slots:
    void onItemClicked(QListWidgetItem *item);
    void onNewFolderClicked();

private:
    void setupUI();
    void setupExpandedView();
    void setupGroups();
    void setupCustomFolders();
    void setupTags();
    void setupCollapsedView();
    void updateCollapsedView();
    void loadFolders();
    void loadTags();
    void createFolder();
    void renameFolder(QListWidgetItem *item);
    void deleteFolder(QListWidgetItem *item);
    void editTag(QListWidgetItem *item);
    void deleteTag(QListWidgetItem *item);

    QVBoxLayout *m_mainLayout;
    QStackedWidget *m_stackWidget;
    
    QWidget *m_expandedWidget;
    QListWidget *m_groupsList;
    QPushButton *m_newFolderButton;
    QLabel *m_groupsTitle;
    QLabel *m_foldersTitle;
    QListWidget *m_foldersList;
    QLabel *m_tagsTitle;
    QListWidget *m_tagsList;
    
    QWidget *m_collapsedWidget;
    QPushButton *m_expandButton;
    
    bool m_expanded;
    static const int DEFAULT_WIDTH = 280;
};

#endif 
