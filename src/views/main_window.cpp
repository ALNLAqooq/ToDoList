#include "main_window.h"
#include "task_list_widget.h"
#include "../utils/theme_manager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMenuBar>
#include <QToolBar>
#include <QAction>
#include <QStatusBar>
#include <QLabel>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_taskController(new TaskController(this))
{
    setupUI();
    resize(1200, 800);
    setWindowTitle("ToDoList");
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    setupMenuBar();
    setupToolBar();
    setupCentralWidget();

    statusBar()->showMessage("就绪");
}

void MainWindow::setupMenuBar()
{
    QMenuBar *menuBar = this->menuBar();

    QMenu *fileMenu = menuBar->addMenu("文件(&F)");

    QAction *newTaskAction = fileMenu->addAction("新建任务(&N)");
    newTaskAction->setShortcut(QKeySequence::New);
    connect(newTaskAction, &QAction::triggered, this, &MainWindow::onAddTaskClicked);

    fileMenu->addSeparator();

    QAction *exitAction = fileMenu->addAction("退出(&X)");
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);

    QMenu *viewMenu = menuBar->addMenu("视图(&V)");

    QAction *themeAction = viewMenu->addAction("切换主题(&T)");
    themeAction->setShortcut(QKeySequence("Ctrl+T"));
    connect(themeAction, &QAction::triggered, this, &MainWindow::onThemeToggled);

    QMenu *helpMenu = menuBar->addMenu("帮助(&H)");

    QAction *aboutAction = helpMenu->addAction("关于(&A)");
    connect(aboutAction, &QAction::triggered, this, &MainWindow::onAboutClicked);
}

void MainWindow::setupToolBar()
{
    QToolBar *toolBar = addToolBar("工具栏");

    QAction *addTaskAction = toolBar->addAction("新建任务");
    addTaskAction->setToolTip("创建新任务");
    connect(addTaskAction, &QAction::triggered, this, &MainWindow::onAddTaskClicked);

    toolBar->addSeparator();

    QAction *themeAction = toolBar->addAction("切换主题");
    themeAction->setToolTip("切换深色/浅色模式");
    connect(themeAction, &QAction::triggered, this, &MainWindow::onThemeToggled);
}

void MainWindow::setupCentralWidget()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    TaskListWidget *taskListWidget = new TaskListWidget(m_taskController, this);
    mainLayout->addWidget(taskListWidget);
}

void MainWindow::onAddTaskClicked()
{
    statusBar()->showMessage("新建任务功能待实现");
}

void MainWindow::onThemeToggled()
{
    ThemeManager::instance().toggleTheme();
    QString themeName = ThemeManager::instance().currentTheme() == ThemeManager::Light ? "浅色" : "深色";
    statusBar()->showMessage(QString("已切换到%1主题").arg(themeName));
}

void MainWindow::onAboutClicked()
{
    QMessageBox aboutBox(this);
    aboutBox.setWindowTitle("关于 ToDoList");
    aboutBox.setText("ToDoList v1.0.0\n\n个人任务管理工具");
    aboutBox.setIcon(QMessageBox::Information);
    aboutBox.exec();
}
