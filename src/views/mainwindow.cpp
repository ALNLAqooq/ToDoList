#include "mainwindow.h"
#include "sidebar.h"
#include "content_area.h"
#include "task_dialog.h"
#include "notificationpanel.h"
#include "../utils/logger.h"
#include "../utils/theme_manager.h"
#include "../controllers/task_controller.h"
#include "../controllers/notificationmanager.h"
#include <QSettings>
#include <QApplication>
#include <QScreen>
#include <QIcon>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_mainLayout(nullptr)
    , m_splitter(nullptr)
    , m_bottomBar(nullptr)
    , m_bottomBarLayout(nullptr)
    , m_sidebar(nullptr)
    , m_contentArea(nullptr)
    , m_toolbar(nullptr)
    , m_searchBox(nullptr)
    , m_newTaskButton(nullptr)
    , m_collapseButton(nullptr)
    , m_notificationButton(nullptr)
    , m_notificationBadge(nullptr)
    , m_notificationPanel(nullptr)
    , m_quickTaskInput(nullptr)
    , m_quickAddButton(nullptr)
    , m_themeButton(nullptr)
{
    loadSettings();
    setupUI();
    setupToolbar();
    setupLayout();
    setupBottomBar();
    setupNotificationButton();
}

MainWindow::~MainWindow()
{
    saveSettings();
}

void MainWindow::setupUI()
{
    setWindowTitle("任务清单");
    setMinimumSize(1024, 768);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    m_mainLayout = new QVBoxLayout(centralWidget);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    m_splitter = new QSplitter(Qt::Horizontal, this);
    m_splitter->setHandleWidth(1);
    m_splitter->setChildrenCollapsible(false);

    m_sidebar = new Sidebar(this);
    connect(m_sidebar, &Sidebar::groupChanged, this, &MainWindow::onGroupChanged);
    connect(m_sidebar, &Sidebar::collapseRequested, this, &MainWindow::onCollapseRequested);
    connect(m_sidebar, &Sidebar::sizeChanged, this, [this]() {
        QTimer::singleShot(0, this, [this]() {
            QList<int> sizes = m_splitter->sizes();
            if (m_sidebar->isExpanded()) {
                sizes[0] = m_sidebar->sidebarWidth();
            } else {
                sizes[0] = 40;
            }
            m_splitter->setSizes(sizes);
        });
    });

    m_contentArea = new ContentArea(this);
    connect(m_sidebar, &Sidebar::tagSelected, m_contentArea, &ContentArea::onTagSelected);
    connect(m_contentArea, &ContentArea::tagsChanged, m_sidebar, &Sidebar::refreshTags);
    m_contentArea->loadTasks();

    LOG_INFO("MainWindow", "Main window UI setup complete");
}

void MainWindow::setupLayout()
{
    m_splitter->addWidget(m_sidebar);
    m_splitter->addWidget(m_contentArea);
    m_splitter->setStretchFactor(0, 0);
    m_splitter->setStretchFactor(1, 1);

    QList<int> sizes;
    sizes.append(280);
    sizes.append(1000);
    m_splitter->setSizes(sizes);

    m_mainLayout->addWidget(m_splitter);

    LOG_INFO("MainWindow", "Main window layout setup complete");
}

void MainWindow::setupBottomBar()
{
    m_bottomBar = new QWidget(this);
    m_bottomBar->setObjectName("bottomBar");
    m_bottomBar->setFixedHeight(60);

    m_bottomBarLayout = new QHBoxLayout(m_bottomBar);
    m_bottomBarLayout->setContentsMargins(12, 10, 12, 10);
    m_bottomBarLayout->setSpacing(10);

    m_quickTaskInput = new QLineEdit(this);
    m_quickTaskInput->setObjectName("quickTaskInput");
    m_quickTaskInput->setFixedHeight(40);
    m_quickTaskInput->setPlaceholderText("快速添加任务...");
    m_bottomBarLayout->addWidget(m_quickTaskInput);

    m_quickAddButton = new QPushButton("添加", this);
    m_quickAddButton->setObjectName("quickAddButton");
    m_quickAddButton->setFixedHeight(40);
    connect(m_quickAddButton, &QPushButton::clicked, this, [this]() {
        QString title = m_quickTaskInput->text().trimmed();
        if (!title.isEmpty()) {
            TaskController controller;
            Task newTask;
            newTask.setTitle(title);
            controller.addTask(newTask);
            m_quickTaskInput->clear();
            refreshTaskList();
            LOG_INFO("MainWindow", QString("Quick task added: %1").arg(title));
        }
    });
    m_bottomBarLayout->addWidget(m_quickAddButton);

    m_mainLayout->addWidget(m_bottomBar);

    LOG_INFO("MainWindow", "Bottom bar setup complete");
}

void MainWindow::setupToolbar()
{
    m_toolbar = addToolBar("Main Toolbar");
    m_toolbar->setObjectName("mainToolbar");
    m_toolbar->setMovable(false);
    m_toolbar->setFixedHeight(56);

    m_searchBox = new QLineEdit(this);
    m_searchBox->setObjectName("toolbarSearch");
    m_searchBox->setPlaceholderText("搜索任务...");
    m_searchBox->setMinimumWidth(240);
    m_searchBox->setMaximumWidth(360);
    m_searchBox->setFixedHeight(36);
    connect(m_searchBox, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);
    m_toolbar->addWidget(m_searchBox);

    QWidget *spacer = new QWidget(this);
    spacer->setObjectName("toolbarSpacer");
    spacer->setAttribute(Qt::WA_StyledBackground, true);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_toolbar->addWidget(spacer);

    m_newTaskButton = new QPushButton("新建任务", this);
    m_newTaskButton->setObjectName("primaryActionButton");
    m_newTaskButton->setFixedHeight(36);
    connect(m_newTaskButton, &QPushButton::clicked, this, &MainWindow::onNewTaskClicked);
    m_toolbar->addWidget(m_newTaskButton);

    m_collapseButton = new QPushButton(this);
    m_collapseButton->setFixedSize(36, 36);
    m_collapseButton->setText("◀");
    m_collapseButton->setProperty("iconButton", true);
    connect(m_collapseButton, &QPushButton::clicked, this, &MainWindow::onCollapseRequested);
    m_toolbar->addWidget(m_collapseButton);

    m_notificationButton = new QToolButton(this);
    m_notificationButton->setFixedSize(36, 36);
    m_notificationButton->setText("🔔");
    m_notificationButton->setProperty("iconButton", true);
    m_notificationButton->setPopupMode(QToolButton::InstantPopup);
    connect(m_notificationButton, &QToolButton::clicked, this, &MainWindow::onNotificationClicked);
    m_toolbar->addWidget(m_notificationButton);

    m_themeButton = new QPushButton(this);
    m_themeButton->setFixedSize(36, 36);
    m_themeButton->setText("🌙");
    m_themeButton->setToolTip("切换主题");
    m_themeButton->setProperty("iconButton", true);
    connect(m_themeButton, &QPushButton::clicked, this, &MainWindow::onThemeToggleClicked);
    m_toolbar->addWidget(m_themeButton);

    LOG_INFO("MainWindow", "Main window toolbar setup complete");
}

void MainWindow::loadSettings()
{
    QSettings settings;

    int width = settings.value("window_width", 1280).toInt();
    int height = settings.value("window_height", 720).toInt();
    int x = settings.value("window_x", -1).toInt();
    int y = settings.value("window_y", -1).toInt();
    int sidebarWidth = settings.value("sidebar_width", 280).toInt();

    resize(width, height);

    if (x >= 0 && y >= 0) {
        move(x, y);
    } else {
        QScreen *screen = QApplication::primaryScreen();
        QRect screenGeometry = screen->geometry();
        int centerX = (screenGeometry.width() - width) / 2;
        int centerY = (screenGeometry.height() - height) / 2;
        move(centerX, centerY);
    }

    if (m_sidebar) {
        m_sidebar->setSidebarWidth(sidebarWidth);
    }

    LOG_INFO("MainWindow", QString("Window loaded with size: %1x%2").arg(width).arg(height));
}

void MainWindow::saveSettings()
{
    QSettings settings;

    settings.setValue("window_width", width());
    settings.setValue("window_height", height());
    settings.setValue("window_x", x());
    settings.setValue("window_y", y());
    settings.setValue("sidebar_width", m_sidebar->sidebarWidth());

    LOG_INFO("MainWindow", QString("Window settings saved: %1x%2").arg(width()).arg(height()));
}

void MainWindow::onGroupChanged(const QString &group)
{
    m_contentArea->setCurrentGroup(group);
    LOG_INFO("MainWindow", QString("Group changed to: %1").arg(group));
}

void MainWindow::onCollapseRequested()
{
    bool isExpanded = m_sidebar->isExpanded();
    m_sidebar->setExpanded(!isExpanded);
    m_collapseButton->setText(isExpanded ? "▶" : "◀");
    LOG_INFO("MainWindow", QString("Sidebar %1").arg(isExpanded ? "collapsed" : "expanded"));
}

void MainWindow::onSearchTextChanged(const QString &text)
{
    LOG_INFO("MainWindow", QString("Search text changed: %1").arg(text));
}

void MainWindow::onNewTaskClicked()
{
    LOG_INFO("MainWindow", "New task button clicked");
    
    TaskController *controller = new TaskController(this);
    TaskDialog *dialog = new TaskDialog(controller, -1, this);
    
    if (dialog->exec() == QDialog::Accepted) {
        LOG_INFO("MainWindow", "New task created via dialog");
        refreshTaskList();
    }
    if (m_sidebar) {
        m_sidebar->refreshTags();
    }
    
    dialog->deleteLater();
}

void MainWindow::refreshTaskList()
{
    if (m_contentArea) {
        m_contentArea->loadTasks();
        LOG_INFO("MainWindow", "Task list refreshed");
    }
}

void MainWindow::setupNotificationButton()
{
    NotificationManager &manager = NotificationManager::instance();
    connect(&manager, &NotificationManager::unreadCountChanged, this, &MainWindow::onNotificationCountChanged);

    m_notificationBadge = new QLabel(m_notificationButton);
    m_notificationBadge->setFixedSize(18, 18);
    m_notificationBadge->move(26, 0);
    m_notificationBadge->setAlignment(Qt::AlignCenter);
    m_notificationBadge->setStyleSheet(
        "QLabel { background: #EF4444; color: white; border-radius: 9px; "
        "font-size: 10px; font-weight: bold; }"
    );
    m_notificationBadge->hide();

    onNotificationCountChanged(manager.unreadCount());

    LOG_INFO("MainWindow", "Notification button setup complete");
}

void MainWindow::onNotificationClicked()
{
    if (!m_notificationPanel) {
        m_notificationPanel = new NotificationPanel(this);
        m_notificationPanel->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
        connect(m_notificationPanel, &NotificationPanel::closeRequested, this, &MainWindow::onNotificationPanelClose);
    }

    QPoint globalPos = m_notificationButton->mapToGlobal(QPoint(0, m_notificationButton->height()));
    int panelX = globalPos.x() + m_notificationButton->width() - m_notificationPanel->width();
    m_notificationPanel->move(panelX, globalPos.y());
    m_notificationPanel->show();
    m_notificationPanel->raise();
    m_notificationPanel->activateWindow();

    LOG_INFO("MainWindow", "Notification panel opened");
}

void MainWindow::onNotificationCountChanged(int count)
{
    if (count > 0) {
        m_notificationBadge->setText(count > 99 ? "99+" : QString::number(count));
        m_notificationBadge->show();
    } else {
        m_notificationBadge->hide();
    }
    LOG_INFO("MainWindow", QString("Notification count changed: %1").arg(count));
}

void MainWindow::onNotificationPanelClose()
{
    if (m_notificationPanel) {
        m_notificationPanel->hide();
        LOG_INFO("MainWindow", "Notification panel closed");
    }
}

void MainWindow::onThemeToggleClicked()
{
    ThemeManager &manager = ThemeManager::instance();
    manager.toggleTheme();

    ThemeManager::Theme theme = manager.currentTheme();
    m_themeButton->setText(theme == ThemeManager::Light ? "🌙" : "☀️");

    LOG_INFO("MainWindow", QString("Theme toggled to: %1").arg(manager.themeName(theme)));
}
