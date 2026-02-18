#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QLabel>
#include <QToolBar>
#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>
#include <QMenu>
#include <QWidgetAction>

class Sidebar;
class ContentArea;
class NotificationPanel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onGroupChanged(const QString &group);
    void onCollapseRequested();
    void onSearchTextChanged(const QString &text);
    void onNewTaskClicked();
    void refreshTaskList();
    void onNotificationClicked();
    void onNotificationCountChanged(int count);
    void onNotificationPanelClose();

private:
    void setupUI();
    void setupLayout();
    void setupToolbar();
    void setupBottomBar();
    void setupNotificationButton();
    void loadSettings();
    void saveSettings();

    QVBoxLayout *m_mainLayout;
    QSplitter *m_splitter;
    QWidget *m_bottomBar;
    QHBoxLayout *m_bottomBarLayout;
    
    Sidebar *m_sidebar;
    ContentArea *m_contentArea;
    
    QToolBar *m_toolbar;
    QLineEdit *m_searchBox;
    QPushButton *m_newTaskButton;
    QPushButton *m_collapseButton;
    QToolButton *m_notificationButton;
    QLabel *m_notificationBadge;
    NotificationPanel *m_notificationPanel;
    
    QLineEdit *m_quickTaskInput;
    QPushButton *m_quickAddButton;
};

#endif // MAINWINDOW_H
