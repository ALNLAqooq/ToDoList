#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <QString>
#include <QStringList>
#include <QFileInfo>
#include <QDir>

class FileUtils
{
public:
    static bool exists(const QString &path);
    static bool isFile(const QString &path);
    static bool isDirectory(const QString &path);
    static bool isReadable(const QString &path);
    static bool isWritable(const QString &path);

    static qint64 fileSize(const QString &path);
    static QString fileSizeFormatted(qint64 bytes);

    static QString fileExtension(const QString &path);
    static QString fileName(const QString &path);
    static QString baseName(const QString &path);
    static QString directoryPath(const QString &path);

    static QString joinPath(const QString &path1, const QString &path2);
    static QString joinPaths(const QStringList &paths);
    static QString normalizePath(const QString &path);

    static bool createDirectory(const QString &path);
    static bool createDirectories(const QString &path);
    static bool removeFile(const QString &path);
    static bool removeDirectory(const QString &path);
    static bool removeDirectoryRecursive(const QString &path);

    static bool copyFile(const QString &source, const QString &destination);
    static bool moveFile(const QString &source, const QString &destination);
    static bool renameFile(const QString &oldPath, const QString &newPath);

    static QString readTextFile(const QString &path);
    static bool writeTextFile(const QString &path, const QString &content);
    static QByteArray readBinaryFile(const QString &path);
    static bool writeBinaryFile(const QString &path, const QByteArray &content);

    static QStringList listFiles(const QString &directory, const QStringList &nameFilters = QStringList(),
                                 QDir::Filters filters = QDir::Files | QDir::NoDotAndDotDot);
    static QStringList listDirectories(const QString &directory);
    static QStringList listAll(const QString &directory);

    static QString tempDirectory();
    static QString tempFilePath(const QString &prefix = QString());

    static QString getAppDataDirectory(const QString &appName);
    static QString getAppConfigDirectory(const QString &appName);
    static QString getAppCacheDirectory(const QString &appName);

    static bool isValidPath(const QString &path);
    static bool isValidFileName(const QString &fileName);
    static bool isAbsolutePath(const QString &path);
    static QString toAbsolutePath(const QString &path);

    static QString getFileIconPath(const QString &filePath);
    static QString getFileType(const QString &filePath);

    static QString sanitizeFileName(const QString &fileName);

    static qint64 calculateDirectorySize(const QString &directory);

private:
    FileUtils() = default;
    ~FileUtils() = default;
};

#endif // FILE_UTILS_H
