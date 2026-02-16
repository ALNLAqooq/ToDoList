#ifndef ICON_UTILS_H
#define ICON_UTILS_H

#include <QIcon>
#include <QPixmap>
#include <QSize>
#include <QString>
#include <QMap>

class IconUtils
{
public:
    enum IconSize {
        Small = 16,
        Medium = 24,
        Large = 32,
        XLarge = 48
    };

    static QIcon loadIcon(const QString &iconPath);
    static QPixmap loadPixmap(const QString &iconPath, const QSize &size = QSize(24, 24));

    static QIcon loadSvgIcon(const QString &iconPath, const QSize &size = QSize(24, 24));
    static QPixmap loadSvgPixmap(const QString &iconPath, const QSize &size = QSize(24, 24));

    static QIcon getIcon(const QString &iconName, IconSize size = Medium);
    static QPixmap getPixmap(const QString &iconName, IconSize size = Medium);

    static QIcon getPriorityIcon(int priority);
    static QIcon getStatusIcon(const QString &status);
    static QIcon getFileIcon(const QString &fileExtension);

    static QString getIconPath(const QString &iconName);
    static QString getIconPath(const QString &iconName, IconSize size);

    static bool iconExists(const QString &iconPath);
    static QStringList getAvailableIcons();
    static QStringList getAvailableIcons(const QString &directory);

    static QIcon tintIcon(const QIcon &icon, const QColor &color);
    static QPixmap tintPixmap(const QPixmap &pixmap, const QColor &color);

    static QIcon scaleIcon(const QIcon &icon, const QSize &size);
    static QPixmap scalePixmap(const QPixmap &pixmap, const QSize &size, Qt::AspectRatioMode mode = Qt::KeepAspectRatio);

    static QIcon roundIcon(const QIcon &icon, int radius);
    static QPixmap roundPixmap(const QPixmap &pixmap, int radius);

    static QString getFontAwesomeIcon(const QString &iconName, int size = 24, const QColor &color = QColor(0, 0, 0));

    static void clearCache();
    static void setCacheEnabled(bool enabled);
    static bool isCacheEnabled();

    static QString getIconsDirectory();
    static QString getDefaultIconPath(const QString &iconName);

private:
    IconUtils() = default;
    ~IconUtils() = default;

    static QMap<QString, QIcon> m_iconCache;
    static QMap<QString, QPixmap> m_pixmapCache;
    static bool m_cacheEnabled;

    static QString getCachedIconKey(const QString &iconPath, const QSize &size);
    static QString getSizeSuffix(IconSize size);
};

#endif // ICON_UTILS_H
