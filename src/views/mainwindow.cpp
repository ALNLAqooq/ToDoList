#include "mainwindow.h"
#include "../utils/logger.h"
#include <QSettings>
#include <QApplication>
#include <QScreen>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_mainLayout(nullptr)
    , m_contentLayout(nullptr)
    , m_placeholderLabel(nullptr)
{
    loadSettings();
    setupUI();
    setupLayout();
}

MainWindow::~MainWindow()
{
    saveSettings();
}

void MainWindow::setupUI()
{
    setWindowTitle("ToDoList");
    setMinimumSize(1024, 768);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    m_mainLayout = new QVBoxLayout(centralWidget);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    m_contentLayout = new QHBoxLayout();
    m_contentLayout->setContentsMargins(0, 0, 0, 0);
    m_contentLayout->setSpacing(0);

    m_placeholderLabel = new QLabel("ToDoList Application - Setup Complete!", this);
    m_placeholderLabel->setAlignment(Qt::AlignCenter);
    m_placeholderLabel->setStyleSheet("font-size: 24px; color: #94A3B8; padding: 20px;");

    LOG_INFO("MainWindow", "Main window UI setup complete");
}

void MainWindow::setupLayout()
{
    m_contentLayout->addWidget(m_placeholderLabel);
    m_mainLayout->addLayout(m_contentLayout);

    LOG_INFO("MainWindow", "Main window layout setup complete");
}

void MainWindow::loadSettings()
{
    QSettings settings;

    int width = settings.value("window_width", 1280).toInt();
    int height = settings.value("window_height", 720).toInt();
    int x = settings.value("window_x", -1).toInt();
    int y = settings.value("window_y", -1).toInt();

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

    LOG_INFO("MainWindow", QString("Window loaded with size: %1x%2").arg(width).arg(height));
}

void MainWindow::saveSettings()
{
    QSettings settings;

    settings.setValue("window_width", width());
    settings.setValue("window_height", height());
    settings.setValue("window_x", x());
    settings.setValue("window_y", y());

    LOG_INFO("MainWindow", QString("Window settings saved: %1x%2").arg(width()).arg(height()));
}
