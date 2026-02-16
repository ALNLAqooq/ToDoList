#include "theme_utils.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QRegularExpression>

QString ThemeUtils::loadStyleSheet(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open style sheet file:" << filePath;
        return QString();
    }

    QString content = file.readAll();
    file.close();

    return content;
}

bool ThemeUtils::saveStyleSheet(const QString &filePath, const QString &styleSheet)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open style sheet file for writing:" << filePath;
        return false;
    }

    QTextStream out(&file);
    out << styleSheet;
    file.close();

    return true;
}

QString ThemeUtils::getDefaultLightTheme()
{
    return R"(
QMainWindow {
    background-color: @bg-color;
}
QWidget {
    background-color: @bg-color;
    color: @text-color;
}
QLabel {
    color: @text-color;
}
QLineEdit {
    background-color: @surface-color;
    border: 1px solid @border-color;
    border-radius: 6px;
    padding: 8px;
    color: @text-color;
}
QLineEdit:focus {
    border: 2px solid @primary-color;
}
QPushButton {
    background-color: @primary-color;
    color: white;
    border: none;
    border-radius: 6px;
    padding: 8px 16px;
    font-weight: bold;
}
QPushButton:hover {
    background-color: @primary-hover-color;
}
QPushButton:pressed {
    background-color: @primary-pressed-color;
}
QScrollBar:vertical {
    background-color: @surface-color;
    width: 12px;
    border-radius: 6px;
}
QScrollBar::handle:vertical {
    background-color: @muted-text-color;
    border-radius: 6px;
    min-height: 30px;
}
QScrollBar::handle:vertical:hover {
    background-color: @text-color;
}
QScrollArea {
    border: none;
    background-color: transparent;
}
QMenuBar {
    background-color: @surface-color;
    border-bottom: 1px solid @border-color;
}
QMenuBar::item {
    padding: 6px 12px;
    background-color: transparent;
}
QMenuBar::item:selected {
    background-color: @bg-color;
}
QToolBar {
    background-color: @surface-color;
    border: none;
    border-bottom: 1px solid @border-color;
    spacing: 8px;
    padding: 4px;
}
QStatusBar {
    background-color: @surface-color;
    border-top: 1px solid @border-color;
    color: @muted-text-color;
}
QCheckBox::indicator {
    width: 20px;
    height: 20px;
    border-radius: 4px;
    border: 2px solid @muted-text-color;
    background-color: white;
}
QCheckBox::indicator:checked {
    background-color: @primary-color;
    border-color: @primary-color;
}
QMessageBox {
    background-color: @surface-color;
}
QMessageBox QLabel {
    color: @text-color;
}
QMessageBox QPushButton {
    background-color: @primary-color;
    color: white;
    border: none;
    border-radius: 6px;
    padding: 8px 16px;
    min-width: 80px;
}
QMessageBox QPushButton:hover {
    background-color: @primary-hover-color;
}
)";
}

QString ThemeUtils::getDefaultDarkTheme()
{
    return R"(
QMainWindow {
    background-color: @bg-color;
}
QWidget {
    background-color: @bg-color;
    color: @text-color;
}
QLabel {
    color: @text-color;
}
QLineEdit {
    background-color: @surface-color;
    border: 1px solid @border-color;
    border-radius: 6px;
    padding: 8px;
    color: @text-color;
}
QLineEdit:focus {
    border: 2px solid @primary-color;
}
QPushButton {
    background-color: @primary-color;
    color: white;
    border: none;
    border-radius: 6px;
    padding: 8px 16px;
    font-weight: bold;
}
QPushButton:hover {
    background-color: @primary-hover-color;
}
QPushButton:pressed {
    background-color: @primary-pressed-color;
}
QScrollBar:vertical {
    background-color: @surface-color;
    width: 12px;
    border-radius: 6px;
}
QScrollBar::handle:vertical {
    background-color: @muted-text-color;
    border-radius: 6px;
    min-height: 30px;
}
QScrollBar::handle:vertical:hover {
    background-color: @text-color;
}
QScrollArea {
    border: none;
    background-color: transparent;
}
QMenuBar {
    background-color: @surface-color;
    border-bottom: 1px solid @border-color;
}
QMenuBar::item {
    padding: 6px 12px;
    background-color: transparent;
    color: @text-color;
}
QMenuBar::item:selected {
    background-color: @border-color;
}
QToolBar {
    background-color: @surface-color;
    border: none;
    border-bottom: 1px solid @border-color;
    spacing: 8px;
    padding: 4px;
}
QStatusBar {
    background-color: @surface-color;
    border-top: 1px solid @border-color;
    color: @muted-text-color;
}
QCheckBox::indicator {
    width: 20px;
    height: 20px;
    border-radius: 4px;
    border: 2px solid @muted-text-color;
    background-color: @surface-color;
}
QCheckBox::indicator:checked {
    background-color: @primary-color;
    border-color: @primary-color;
}
QMessageBox {
    background-color: @surface-color;
}
QMessageBox QLabel {
    color: @text-color;
}
QMessageBox QPushButton {
    background-color: @primary-color;
    color: white;
    border: none;
    border-radius: 6px;
    padding: 8px 16px;
    min-width: 80px;
}
QMessageBox QPushButton:hover {
    background-color: @primary-hover-color;
}
)";
}

QString ThemeUtils::getThemeForType(Theme theme)
{
    if (theme == Light) {
        return getDefaultLightTheme();
    } else {
        return getDefaultDarkTheme();
    }
}

QColor ThemeUtils::getPrimaryColor(Theme theme)
{
    return QColor("#3B82F6");
}

QColor ThemeUtils::getSecondaryColor(Theme theme)
{
    return QColor("#6366F1");
}

QColor ThemeUtils::getBackgroundColor(Theme theme)
{
    if (theme == Light) {
        return QColor("#F9FAFB");
    } else {
        return QColor("#111827");
    }
}

QColor ThemeUtils::getSurfaceColor(Theme theme)
{
    if (theme == Light) {
        return QColor("#FFFFFF");
    } else {
        return QColor("#1F2937");
    }
}

QColor ThemeUtils::getTextColor(Theme theme)
{
    if (theme == Light) {
        return QColor("#1F2937");
    } else {
        return QColor("#F9FAFB");
    }
}

QColor ThemeUtils::getMutedTextColor(Theme theme)
{
    if (theme == Light) {
        return QColor("#6B7280");
    } else {
        return QColor("#9CA3AF");
    }
}

QColor ThemeUtils::getBorderColor(Theme theme)
{
    if (theme == Light) {
        return QColor("#E5E7EB");
    } else {
        return QColor("#374151");
    }
}

QColor ThemeUtils::getPriorityColor(Priority priority)
{
    switch (priority) {
        case High:
            return QColor("#EF4444");
        case Medium:
            return QColor("#F59E0B");
        case Low:
            return QColor("#10B981");
        default:
            return QColor("#6B7280");
    }
}

QString ThemeUtils::getPriorityColorName(Priority priority)
{
    switch (priority) {
        case High:
            return "#EF4444";
        case Medium:
            return "#F59E0B";
        case Low:
            return "#10B981";
        default:
            return "#6B7280";
    }
}

QString ThemeUtils::getPriorityStyleSheet(Priority priority)
{
    QString color = getPriorityColorName(priority);
    return QString("background-color: %1;").arg(color);
}

QString ThemeUtils::getColorVariable(const QString &name, Theme theme)
{
    QMap<QString, QString> variables = getColorVariables(theme);
    return variables.value(name);
}

QMap<QString, QString> ThemeUtils::getColorVariables(Theme theme)
{
    if (theme == Light) {
        return getLightColorVariables();
    } else {
        return getDarkColorVariables();
    }
}

QString ThemeUtils::applyThemeVariables(const QString &styleSheet, Theme theme)
{
    QMap<QString, QString> variables = getColorVariables(theme);
    return replaceColorVariables(styleSheet, variables);
}

QString ThemeUtils::replaceColorVariables(const QString &styleSheet, const QMap<QString, QString> &variables)
{
    QString result = styleSheet;
    for (auto it = variables.begin(); it != variables.end(); ++it) {
        result.replace(QString("@%1").arg(it.key()), it.value());
    }
    return result;
}

QString ThemeUtils::formatColor(const QColor &color)
{
    return color.name();
}

QColor ThemeUtils::parseColor(const QString &colorString)
{
    QColor color(colorString);
    return color;
}

QString ThemeUtils::getContrastColor(const QColor &backgroundColor)
{
    if (isColorLight(backgroundColor)) {
        return "#000000";
    } else {
        return "#FFFFFF";
    }
}

bool ThemeUtils::isColorLight(const QColor &color)
{
    int brightness = (color.red() * 299 + color.green() * 587 + color.blue() * 114) / 1000;
    return brightness > 128;
}

QString ThemeUtils::blendColors(const QColor &color1, const QColor &color2, double ratio)
{
    int r = color1.red() + (color2.red() - color1.red()) * ratio;
    int g = color1.green() + (color2.green() - color1.green()) * ratio;
    int b = color1.blue() + (color2.blue() - color1.blue()) * ratio;

    return QColor(r, g, b).name();
}

QString ThemeUtils::getGlassmorphismStyle(Theme theme, double opacity)
{
    QString bgColor = getBackgroundColor(theme).name();
    return QString("background-color: rgba(%1, %2, %3, %4); backdrop-filter: blur(10px);")
        .arg(bgColor.mid(1, 2).toInt(nullptr, 16))
        .arg(bgColor.mid(3, 2).toInt(nullptr, 16))
        .arg(bgColor.mid(5, 2).toInt(nullptr, 16))
        .arg(opacity);
}

QString ThemeUtils::getShadowStyle(Theme theme)
{
    if (theme == Light) {
        return "box-shadow: 0 1px 3px rgba(0, 0, 0, 0.1), 0 1px 2px rgba(0, 0, 0, 0.06);";
    } else {
        return "box-shadow: 0 1px 3px rgba(0, 0, 0, 0.5), 0 1px 2px rgba(0, 0, 0, 0.3);";
    }
}

QString ThemeUtils::getButtonStyle(Theme theme)
{
    return QString("background-color: %1; color: white; border: none; border-radius: 6px; padding: 8px 16px; font-weight: bold;")
        .arg(getPrimaryColor(theme).name());
}

QString ThemeUtils::getButtonHoverStyle(Theme theme)
{
    QColor primary = getPrimaryColor(theme);
    QColor hover = primary.darker(110);
    return QString("background-color: %1;").arg(hover.name());
}

QString ThemeUtils::getButtonPressedStyle(Theme theme)
{
    QColor primary = getPrimaryColor(theme);
    QColor pressed = primary.darker(120);
    return QString("background-color: %1;").arg(pressed.name());
}

QString ThemeUtils::getInputStyle(Theme theme)
{
    QString surface = getSurfaceColor(theme).name();
    QString border = getBorderColor(theme).name();
    QString text = getTextColor(theme).name();

    return QString("background-color: %1; border: 1px solid %2; border-radius: 6px; padding: 8px; color: %3;")
        .arg(surface, border, text);
}

QString ThemeUtils::getInputFocusStyle(Theme theme)
{
    QString primary = getPrimaryColor(theme).name();
    return QString("border: 2px solid %1;").arg(primary);
}

QString ThemeUtils::getCardStyle(Theme theme)
{
    QString surface = getSurfaceColor(theme).name();
    QString border = getBorderColor(theme).name();
    QString shadow = getShadowStyle(theme);

    return QString("background-color: %1; border: 1px solid %2; border-radius: 8px; %3")
        .arg(surface, border, shadow);
}

QString ThemeUtils::getCardHoverStyle(Theme theme)
{
    QString surface = getSurfaceColor(theme).name();
    QString border = getBorderColor(theme).name();
    QColor hoverBorder = theme == Light ? QColor("#D1D5DB") : QColor("#4B5563");

    return QString("background-color: %1; border: 1px solid %2; border-radius: 8px;")
        .arg(surface, hoverBorder.name());
}

QString ThemeUtils::getScrollbarStyle(Theme theme)
{
    QString surface = getSurfaceColor(theme).name();
    QString handle = getMutedTextColor(theme).name();

    return QString("QScrollBar:vertical { background-color: %1; width: 12px; border-radius: 6px; } "
                   "QScrollBar::handle:vertical { background-color: %2; border-radius: 6px; min-height: 30px; }")
        .arg(surface, handle);
}

QMap<QString, QString> ThemeUtils::getLightColorVariables()
{
    QMap<QString, QString> variables;
    variables["primary-color"] = "#3B82F6";
    variables["primary-hover-color"] = "#2563EB";
    variables["primary-pressed-color"] = "#1D4ED8";
    variables["secondary-color"] = "#6366F1";
    variables["bg-color"] = "#F9FAFB";
    variables["surface-color"] = "#FFFFFF";
    variables["text-color"] = "#1F2937";
    variables["muted-text-color"] = "#6B7280";
    variables["border-color"] = "#E5E7EB";
    variables["success-color"] = "#10B981";
    variables["warning-color"] = "#F59E0B";
    variables["error-color"] = "#EF4444";
    return variables;
}

QMap<QString, QString> ThemeUtils::getDarkColorVariables()
{
    QMap<QString, QString> variables;
    variables["primary-color"] = "#3B82F6";
    variables["primary-hover-color"] = "#60A5FA";
    variables["primary-pressed-color"] = "#93C5FD";
    variables["secondary-color"] = "#6366F1";
    variables["bg-color"] = "#111827";
    variables["surface-color"] = "#1F2937";
    variables["text-color"] = "#F9FAFB";
    variables["muted-text-color"] = "#9CA3AF";
    variables["border-color"] = "#374151";
    variables["success-color"] = "#10B981";
    variables["warning-color"] = "#F59E0B";
    variables["error-color"] = "#EF4444";
    return variables;
}
