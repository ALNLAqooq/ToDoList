#include "theme_manager.h"
#include <QFile>
#include <QDebug>

ThemeManager& ThemeManager::instance()
{
    static ThemeManager instance;
    return instance;
}

ThemeManager::ThemeManager(QObject *parent)
    : QObject(parent)
    , m_currentTheme(Light)
{
    loadLightTheme();
    loadDarkTheme();
}

ThemeManager::~ThemeManager()
{
}

void ThemeManager::loadLightTheme()
{
    m_lightStyleSheet = R"(
        QMainWindow {
            background-color: #F9FAFB;
        }
        QWidget {
            background-color: #F9FAFB;
            color: #1F2937;
        }
        QLabel {
            color: #1F2937;
        }
        QLineEdit {
            background-color: #FFFFFF;
            border: 1px solid #D1D5DB;
            border-radius: 6px;
            padding: 8px;
            color: #1F2937;
        }
        QLineEdit:focus {
            border: 2px solid #3B82F6;
        }
        QPushButton {
            background-color: #3B82F6;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 8px 16px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #2563EB;
        }
        QPushButton:pressed {
            background-color: #1D4ED8;
        }
        QScrollBar:vertical {
            background-color: #F3F4F6;
            width: 12px;
            border-radius: 6px;
        }
        QScrollBar::handle:vertical {
            background-color: #9CA3AF;
            border-radius: 6px;
            min-height: 30px;
        }
        QScrollBar::handle:vertical:hover {
            background-color: #6B7280;
        }
        QScrollArea {
            border: none;
            background-color: transparent;
        }
        QMenuBar {
            background-color: #FFFFFF;
            border-bottom: 1px solid #E5E7EB;
        }
        QMenuBar::item {
            padding: 6px 12px;
            background-color: transparent;
        }
        QMenuBar::item:selected {
            background-color: #F3F4F6;
        }
        QToolBar {
            background-color: #FFFFFF;
            border: none;
            border-bottom: 1px solid #E5E7EB;
            spacing: 8px;
            padding: 4px;
        }
        QStatusBar {
            background-color: #FFFFFF;
            border-top: 1px solid #E5E7EB;
            color: #6B7280;
        }
        QCheckBox::indicator {
            width: 20px;
            height: 20px;
            border-radius: 4px;
            border: 2px solid #9CA3AF;
            background-color: white;
        }
        QCheckBox::indicator:checked {
            background-color: #3B82F6;
            border-color: #3B82F6;
        }
        QMessageBox {
            background-color: #FFFFFF;
        }
        QMessageBox QLabel {
            color: #1F2937;
        }
        QMessageBox QPushButton {
            background-color: #3B82F6;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 8px 16px;
            min-width: 80px;
        }
        QMessageBox QPushButton:hover {
            background-color: #2563EB;
        }
    )";
}

void ThemeManager::loadDarkTheme()
{
    m_darkStyleSheet = R"(
        QMainWindow {
            background-color: #111827;
        }
        QWidget {
            background-color: #111827;
            color: #F9FAFB;
        }
        QLabel {
            color: #F9FAFB;
        }
        QLineEdit {
            background-color: #1F2937;
            border: 1px solid #374151;
            border-radius: 6px;
            padding: 8px;
            color: #F9FAFB;
        }
        QLineEdit:focus {
            border: 2px solid #3B82F6;
        }
        QPushButton {
            background-color: #3B82F6;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 8px 16px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #2563EB;
        }
        QPushButton:pressed {
            background-color: #1D4ED8;
        }
        QScrollBar:vertical {
            background-color: #1F2937;
            width: 12px;
            border-radius: 6px;
        }
        QScrollBar::handle:vertical {
            background-color: #4B5563;
            border-radius: 6px;
            min-height: 30px;
        }
        QScrollBar::handle:vertical:hover {
            background-color: #6B7280;
        }
        QScrollArea {
            border: none;
            background-color: transparent;
        }
        QMenuBar {
            background-color: #1F2937;
            border-bottom: 1px solid #374151;
        }
        QMenuBar::item {
            padding: 6px 12px;
            background-color: transparent;
            color: #F9FAFB;
        }
        QMenuBar::item:selected {
            background-color: #374151;
        }
        QToolBar {
            background-color: #1F2937;
            border: none;
            border-bottom: 1px solid #374151;
            spacing: 8px;
            padding: 4px;
        }
        QStatusBar {
            background-color: #1F2937;
            border-top: 1px solid #374151;
            color: #9CA3AF;
        }
        QCheckBox::indicator {
            width: 20px;
            height: 20px;
            border-radius: 4px;
            border: 2px solid #4B5563;
            background-color: #1F2937;
        }
        QCheckBox::indicator:checked {
            background-color: #3B82F6;
            border-color: #3B82F6;
        }
        QMessageBox {
            background-color: #1F2937;
        }
        QMessageBox QLabel {
            color: #F9FAFB;
        }
        QMessageBox QPushButton {
            background-color: #3B82F6;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 8px 16px;
            min-width: 80px;
        }
        QMessageBox QPushButton:hover {
            background-color: #2563EB;
        }
    )";
}

void ThemeManager::setTheme(Theme theme)
{
    m_currentTheme = theme;
    qApp->setStyleSheet(getStyleSheet());
    emit themeChanged(theme);
}

void ThemeManager::toggleTheme()
{
    if (m_currentTheme == Light) {
        setTheme(Dark);
    } else {
        setTheme(Light);
    }
}

QString ThemeManager::getStyleSheet() const
{
    if (m_currentTheme == Light) {
        return m_lightStyleSheet;
    } else {
        return m_darkStyleSheet;
    }
}
