#include "mainwindow.h"
#include "sidebar.h"
#include "content_area.h"
#include "task_dialog.h"
#include "notificationpanel.h"
#include "../utils/logger.h"
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
    m_splitter->setStyleSheet("QSplitter::handle { background: #E2E8F0; }");
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
    m_bottomBar->setFixedHeight(56);
    m_bottomBar->setStyleSheet(
        "QWidget { background: #F8FAFC; border-top: 1px solid #E2E8F0; }"
    );

    m_bottomBarLayout = new QHBoxLayout(m_bottomBar);
    m_bottomBarLayout->setContentsMargins(16, 8, 16, 8);
    m_bottomBarLayout->setSpacing(12);

    m_quickTaskInput = new QLineEdit(this);
    m_quickTaskInput->setPlaceholderText("快速添加任务...");
    m_quickTaskInput->setStyleSheet(
        "QLineEdit { padding: 10px 16px; border: 1px solid #CBD5E1; border-radius: 8px; "
        "background: white; font-size: 14px; }"
        "QLineEdit:focus { border-color: #3B82F6; border-width: 2px; outline: none; }"
    );
    m_bottomBarLayout->addWidget(m_quickTaskInput);

    m_quickAddButton = new QPushButton("添加", this);
    m_quickAddButton->setStyleSheet(
        "QPushButton { background: #3B82F6; color: white; padding: 10px 24px; "
        "border-radius: 8px; border: none; font-weight: 500; font-size: 14px; }"
        "QPushButton:hover { background: #2563EB; }"
        "QPushButton:pressed { background: #1D4ED8; }"
    );
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
    m_toolbar->setMovable(false);
    m_toolbar->setFixedHeight(64);
    m_toolbar->setStyleSheet(
        "QToolBar { background: #FFFFFF; border-bottom: 1px solid #E2E8F0; "
        "spacing: 12px; padding: 0 16px; }"
    );

    m_searchBox = new QLineEdit(this);
    m_searchBox->setPlaceholderText("搜索任务...");
    m_searchBox->setMinimumWidth(280);
    m_searchBox->setMaximumWidth(400);
    m_searchBox->setFixedHeight(40);
    m_searchBox->setStyleSheet(
        "QLineEdit { padding: 10px 16px; border: 1px solid #E2E8F0; border-radius: 8px; "
        "background: #F8FAFC; font-size: 14px; color: #64748B; }"
        "QLineEdit:focus { border-color: #2563EB; border-width: 2px; outline: none; "
        "background: #FFFFFF; color: #0F172A; }"
        "QLineEdit:hover { border-color: #CBD5E1; }"
    );
    connect(m_searchBox, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);
    m_toolbar->addWidget(m_searchBox);

    QWidget *spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_toolbar->addWidget(spacer);

    m_newTaskButton = new QPushButton("新建任务", this);
    m_newTaskButton->setFixedHeight(40);
    m_newTaskButton->setStyleSheet(
        "QPushButton { background: #2563EB; color: white; padding: 0 24px; "
        "border-radius: 8px; border: none; font-weight: 600; font-size: 14px; }"
        "QPushButton:hover { background: #1D4ED8; }"
        "QPushButton:pressed { background: #1E40AF; }"
    );
    connect(m_newTaskButton, &QPushButton::clicked, this, &MainWindow::onNewTaskClicked);
    m_toolbar->addWidget(m_newTaskButton);

    m_collapseButton = new QPushButton(this);
    m_collapseButton->setFixedSize(40, 40);
    m_collapseButton->setText("◀");
    m_collapseButton->setStyleSheet(
        "QPushButton { background: transparent; border: 1px solid transparent; "
        "border-radius: 8px; color: #64748B; font-size: 16px; }"
        "QPushButton:hover { background: #F1F5F9; border-color: #E2E8F0; "
        "color: #0F172A; }"
        "QPushButton:pressed { background: #E2E8F0; }"
    );
    connect(m_collapseButton, &QPushButton::clicked, this, &MainWindow::onCollapseRequested);
    m_toolbar->addWidget(m_collapseButton);

    m_notificationButton = new QToolButton(this);
    m_notificationButton->setFixedSize(40, 40);
    m_notificationButton->setText("🔔");
    m_notificationButton->setStyleSheet(
        "QToolButton { background: transparent; border: 1px solid transparent; "
        "border-radius: 8px; color: #64748B; font-size: 18px; }"
        "QToolButton:hover { background: #F1F5F9; border-color: #E2E8F0; "
        "color: #0F172A; }"
        "QToolButton:pressed { background: #E2E8F0; }"
    );
    m_notificationButton->setPopupMode(QToolButton::InstantPopup);
    connect(m_notificationButton, &QToolButton::clicked, this, &MainWindow::onNotificationClicked);
    m_toolbar->addWidget(m_notificationButton);

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
    m_notificationPanel->move(globalPos.x() - 340, globalPos.y());
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
