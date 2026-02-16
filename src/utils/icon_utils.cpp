#include "icon_utils.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QPainter>
#include <QSvgRenderer>
#include <QDebug>
#include <QBitmap>
#include <QtSvg>

QMap<QString, QIcon> IconUtils::m_iconCache;
QMap<QString, QPixmap> IconUtils::m_pixmapCache;
bool IconUtils::m_cacheEnabled = true;

QIcon IconUtils::loadIcon(const QString &iconPath)
{
    if (!iconExists(iconPath)) {
        qWarning() << "Icon does not exist:" << iconPath;
        return QIcon();
    }

    if (m_cacheEnabled) {
        QString key = getCachedIconKey(iconPath, QSize());
        if (m_iconCache.contains(key)) {
            return m_iconCache[key];
        }

        QIcon icon(iconPath);
        m_iconCache[key] = icon;
        return icon;
    }

    return QIcon(iconPath);
}

QPixmap IconUtils::loadPixmap(const QString &iconPath, const QSize &size)
{
    if (!iconExists(iconPath)) {
        qWarning() << "Icon does not exist:" << iconPath;
        return QPixmap();
    }

    if (m_cacheEnabled) {
        QString key = getCachedIconKey(iconPath, size);
        if (m_pixmapCache.contains(key)) {
            return m_pixmapCache[key];
        }

        QPixmap pixmap(iconPath);
        if (!pixmap.isNull() && size.isValid()) {
            pixmap = scalePixmap(pixmap, size);
        }

        m_pixmapCache[key] = pixmap;
        return pixmap;
    }

    QPixmap pixmap(iconPath);
    if (!pixmap.isNull() && size.isValid()) {
        pixmap = scalePixmap(pixmap, size);
    }
    return pixmap;
}

QIcon IconUtils::loadSvgIcon(const QString &iconPath, const QSize &size)
{
    if (!iconExists(iconPath)) {
        qWarning() << "SVG icon does not exist:" << iconPath;
        return QIcon();
    }

    if (m_cacheEnabled) {
        QString key = getCachedIconKey(iconPath, size);
        if (m_iconCache.contains(key)) {
            return m_iconCache[key];
        }

        QPixmap pixmap = loadSvgPixmap(iconPath, size);
        QIcon icon(pixmap);
        m_iconCache[key] = icon;
        return icon;
    }

    QPixmap pixmap = loadSvgPixmap(iconPath, size);
    return QIcon(pixmap);
}

QPixmap IconUtils::loadSvgPixmap(const QString &iconPath, const QSize &size)
{
    if (!iconExists(iconPath)) {
        qWarning() << "SVG icon does not exist:" << iconPath;
        return QPixmap();
    }

    QFile file(iconPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open SVG file:" << iconPath;
        return QPixmap();
    }

    QByteArray data = file.readAll();
    QSvgRenderer renderer(data);

    QPixmap pixmap(size);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    renderer.render(&painter);
    painter.end();

    file.close();

    return pixmap;
}

QIcon IconUtils::getIcon(const QString &iconName, IconSize size)
{
    QString iconPath = getIconPath(iconName, size);
    return loadIcon(iconPath);
}

QPixmap IconUtils::getPixmap(const QString &iconName, IconSize size)
{
    QString iconPath = getIconPath(iconName, size);
    QSize iconSize(size, size);
    return loadPixmap(iconPath, iconSize);
}

QIcon IconUtils::getPriorityIcon(int priority)
{
    QString color;
    switch (priority) {
        case 3:
            color = "red";
            break;
        case 2:
            color = "amber";
            break;
        case 1:
            color = "green";
            break;
        default:
            color = "gray";
            break;
    }

    QString iconName = QString("priority-%1").arg(color);
    return getIcon(iconName, Medium);
}

QIcon IconUtils::getStatusIcon(const QString &status)
{
    QString iconName;
    if (status == "completed") {
        iconName = "check-circle";
    } else if (status == "incomplete") {
        iconName = "circle";
    } else if (status == "in_progress") {
        iconName = "clock";
    } else {
        iconName = "circle";
    }

    return getIcon(iconName, Medium);
}

QIcon IconUtils::getFileIcon(const QString &fileExtension)
{
    QString ext = fileExtension.toLower();
    QString iconName;

    static QMap<QString, QString> iconMap;
    if (iconMap.isEmpty()) {
        iconMap["txt"] = "file-text";
        iconMap["pdf"] = "file-pdf";
        iconMap["doc"] = "file-word";
        iconMap["docx"] = "file-word";
        iconMap["xls"] = "file-excel";
        iconMap["xlsx"] = "file-excel";
        iconMap["ppt"] = "file-powerpoint";
        iconMap["pptx"] = "file-powerpoint";
        iconMap["jpg"] = "file-image";
        iconMap["jpeg"] = "file-image";
        iconMap["png"] = "file-image";
        iconMap["gif"] = "file-image";
        iconMap["svg"] = "file-image";
        iconMap["bmp"] = "file-image";
        iconMap["mp3"] = "file-audio";
        iconMap["wav"] = "file-audio";
        iconMap["ogg"] = "file-audio";
        iconMap["mp4"] = "file-video";
        iconMap["avi"] = "file-video";
        iconMap["mkv"] = "file-video";
        iconMap["mov"] = "file-video";
        iconMap["zip"] = "file-archive";
        iconMap["rar"] = "file-archive";
        iconMap["7z"] = "file-archive";
        iconMap["tar"] = "file-archive";
        iconMap["gz"] = "file-archive";
        iconMap["cpp"] = "file-code";
        iconMap["h"] = "file-code";
        iconMap["hpp"] = "file-code";
        iconMap["c"] = "file-code";
        iconMap["py"] = "file-code";
        iconMap["js"] = "file-code";
        iconMap["html"] = "file-code";
        iconMap["css"] = "file-code";
        iconMap["json"] = "file-code";
        iconMap["xml"] = "file-code";
    }

    iconName = iconMap.value(ext, "file");
    return getIcon(iconName, Medium);
}

QString IconUtils::getIconPath(const QString &iconName)
{
    return getIconPath(iconName, Medium);
}

QString IconUtils::getIconPath(const QString &iconName, IconSize size)
{
    QString iconsDir = getIconsDirectory();
    QString sizeSuffix = getSizeSuffix(size);

    QString svgPath = QString("%1/%2%3.svg").arg(iconsDir, iconName, sizeSuffix);
    if (QFile::exists(svgPath)) {
        return svgPath;
    }

    QString pngPath = QString("%1/%2%3.png").arg(iconsDir, iconName, sizeSuffix);
    if (QFile::exists(pngPath)) {
        return pngPath;
    }

    QString defaultSvgPath = QString(":/icons/%1.svg").arg(iconName);
    if (QFile::exists(defaultSvgPath)) {
        return defaultSvgPath;
    }

    return QString();
}

bool IconUtils::iconExists(const QString &iconPath)
{
    return QFile::exists(iconPath);
}

QStringList IconUtils::getAvailableIcons()
{
    return getAvailableIcons(getIconsDirectory());
}

QStringList IconUtils::getAvailableIcons(const QString &directory)
{
    QStringList icons;
    QDir dir(directory);

    if (!dir.exists()) {
        return icons;
    }

    QStringList filters;
    filters << "*.svg" << "*.png";
    dir.setNameFilters(filters);

    QFileInfoList fileInfoList = dir.entryInfoList(QDir::Files);
    foreach (QFileInfo fileInfo, fileInfoList) {
        QString baseName = fileInfo.baseName();
        QRegularExpression re("_\\d+$");
        baseName.remove(re);
        if (!icons.contains(baseName)) {
            icons.append(baseName);
        }
    }

    return icons;
}

QIcon IconUtils::tintIcon(const QIcon &icon, const QColor &color)
{
    QPixmap pixmap = icon.pixmap(icon.availableSizes().first());
    return QIcon(tintPixmap(pixmap, color));
}

QPixmap IconUtils::tintPixmap(const QPixmap &pixmap, const QColor &color)
{
    if (pixmap.isNull()) {
        return pixmap;
    }

    QPixmap result(pixmap.size());
    result.fill(Qt::transparent);

    QPainter painter(&result);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.drawPixmap(0, 0, pixmap);
    painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
    painter.fillRect(result.rect(), color);
    painter.end();

    return result;
}

QIcon IconUtils::scaleIcon(const QIcon &icon, const QSize &size)
{
    QPixmap pixmap = icon.pixmap(icon.availableSizes().first());
    return QIcon(scalePixmap(pixmap, size));
}

QPixmap IconUtils::scalePixmap(const QPixmap &pixmap, const QSize &size, Qt::AspectRatioMode mode)
{
    if (pixmap.isNull() || !size.isValid()) {
        return pixmap;
    }

    return pixmap.scaled(size, mode, Qt::SmoothTransformation);
}

QIcon IconUtils::roundIcon(const QIcon &icon, int radius)
{
    QPixmap pixmap = icon.pixmap(icon.availableSizes().first());
    return QIcon(roundPixmap(pixmap, radius));
}

QPixmap IconUtils::roundPixmap(const QPixmap &pixmap, int radius)
{
    if (pixmap.isNull()) {
        return pixmap;
    }

    QPixmap rounded(pixmap.size());
    rounded.fill(Qt::transparent);

    QPainter painter(&rounded);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    QBrush brush(pixmap);
    painter.setBrush(brush);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(pixmap.rect(), radius, radius, Qt::RelativeSize);
    painter.end();

    return rounded;
}

QString IconUtils::getFontAwesomeIcon(const QString &iconName, int size, const QColor &color)
{
    QString colorHex = color.name();
    return QString("<span style='font-family: FontAwesome; font-size: %1px; color: %2;'>&#x%3;</span>")
        .arg(size)
        .arg(colorHex)
        .arg(iconName);
}

void IconUtils::clearCache()
{
    m_iconCache.clear();
    m_pixmapCache.clear();
}

void IconUtils::setCacheEnabled(bool enabled)
{
    m_cacheEnabled = enabled;
}

bool IconUtils::isCacheEnabled()
{
    return m_cacheEnabled;
}

QString IconUtils::getIconsDirectory()
{
    return ":/icons";
}

QString IconUtils::getDefaultIconPath(const QString &iconName)
{
    return QString(":/icons/%1.svg").arg(iconName);
}

QString IconUtils::getCachedIconKey(const QString &iconPath, const QSize &size)
{
    if (size.isValid()) {
        return QString("%1_%2x%3").arg(iconPath).arg(size.width()).arg(size.height());
    }
    return iconPath;
}

QString IconUtils::getSizeSuffix(IconSize size)
{
    switch (size) {
        case Small:
            return "-16";
        case Medium:
            return "-24";
        case Large:
            return "-32";
        case XLarge:
            return "-48";
        default:
            return "";
    }
}
