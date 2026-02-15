#include "app.h"
#include "../database/database_manager.h"
#include "../views/main_window.h"
#include "../utils/theme_manager.h"
#include <QApplication>

App::App(QObject *parent)
    : QObject(parent)
{
}

App::~App()
{
}

void App::init()
{
    initDatabase();
    initTheme();
    initWindow();
}

void App::initDatabase()
{
    DatabaseManager::instance().open();
}

void App::initTheme()
{
    ThemeManager::instance().setTheme(ThemeManager::Light);
}

void App::initWindow()
{
    MainWindow *window = new MainWindow();
    window->show();
}
