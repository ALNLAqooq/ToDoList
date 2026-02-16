#include "file_utils.h"
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QTemporaryDir>
#include <QDebug>
#include <QCoreApplication>

bool FileUtils::exists(const QString &path)
{
    return QFile::exists(path);
}

bool FileUtils::isFile(const QString &path)
{
    QFileInfo info(path);
    return info.isFile();
}

bool FileUtils::isDirectory(const QString &path)
{
    QFileInfo info(path);
    return info.isDir();
}

bool FileUtils::isReadable(const QString &path)
{
    QFileInfo info(path);
    return info.isReadable();
}

bool FileUtils::isWritable(const QString &path)
{
    QFileInfo info(path);
    return info.isWritable();
}

qint64 FileUtils::fileSize(const QString &path)
{
    QFileInfo info(path);
    return info.size();
}

QString FileUtils::fileSizeFormatted(qint64 bytes)
{
    const qint64 KB = 1024;
    const qint64 MB = 1024 * KB;
    const qint64 GB = 1024 * MB;
    const qint64 TB = 1024 * GB;

    if (bytes < KB) {
        return QObject::tr("%1 B").arg(bytes);
    } else if (bytes < MB) {
        return QObject::tr("%1 KB").arg(static_cast<double>(bytes) / KB, 0, 'f', 2);
    } else if (bytes < GB) {
        return QObject::tr("%1 MB").arg(static_cast<double>(bytes) / MB, 0, 'f', 2);
    } else if (bytes < TB) {
        return QObject::tr("%1 GB").arg(static_cast<double>(bytes) / GB, 0, 'f', 2);
    } else {
        return QObject::tr("%1 TB").arg(static_cast<double>(bytes) / TB, 0, 'f', 2);
    }
}

QString FileUtils::fileExtension(const QString &path)
{
    QFileInfo info(path);
    return info.suffix().toLower();
}

QString FileUtils::fileName(const QString &path)
{
    QFileInfo info(path);
    return info.fileName();
}

QString FileUtils::baseName(const QString &path)
{
    QFileInfo info(path);
    return info.baseName();
}

QString FileUtils::directoryPath(const QString &path)
{
    QFileInfo info(path);
    return info.absolutePath();
}

QString FileUtils::joinPath(const QString &path1, const QString &path2)
{
    return QDir::cleanPath(path1 + "/" + path2);
}

QString FileUtils::joinPaths(const QStringList &paths)
{
    if (paths.isEmpty()) {
        return QString();
    }
    QString result = paths.first();
    for (int i = 1; i < paths.size(); ++i) {
        result = joinPath(result, paths[i]);
    }
    return QDir::cleanPath(result);
}

QString FileUtils::normalizePath(const QString &path)
{
    return QDir::cleanPath(path);
}

bool FileUtils::createDirectory(const QString &path)
{
    QDir dir;
    return dir.mkdir(path);
}

bool FileUtils::createDirectories(const QString &path)
{
    QDir dir;
    return dir.mkpath(path);
}

bool FileUtils::removeFile(const QString &path)
{
    QFile file(path);
    return file.remove();
}

bool FileUtils::removeDirectory(const QString &path)
{
    QDir dir(path);
    return dir.rmdir(path);
}

bool FileUtils::removeDirectoryRecursive(const QString &path)
{
    QDir dir(path);
    return dir.removeRecursively();
}

bool FileUtils::copyFile(const QString &source, const QString &destination)
{
    QFile file(source);
    return file.copy(destination);
}

bool FileUtils::moveFile(const QString &source, const QString &destination)
{
    QFile file(source);
    return file.rename(destination);
}

bool FileUtils::renameFile(const QString &oldPath, const QString &newPath)
{
    QFile file(oldPath);
    return file.rename(newPath);
}

QString FileUtils::readTextFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for reading:" << path;
        return QString();
    }

    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    return content;
}

bool FileUtils::writeTextFile(const QString &path, const QString &content)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for writing:" << path;
        return false;
    }

    QTextStream out(&file);
    out << content;
    file.close();

    return true;
}

QByteArray FileUtils::readBinaryFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open file for reading:" << path;
        return QByteArray();
    }

    QByteArray data = file.readAll();
    file.close();

    return data;
}

bool FileUtils::writeBinaryFile(const QString &path, const QByteArray &content)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open file for writing:" << path;
        return false;
    }

    qint64 written = file.write(content);
    file.close();

    return written == content.size();
}

QStringList FileUtils::listFiles(const QString &directory, const QStringList &nameFilters, QDir::Filters filters)
{
    QDir dir(directory);
    if (!dir.exists()) {
        return QStringList();
    }
    return dir.entryList(nameFilters, filters);
}

QStringList FileUtils::listDirectories(const QString &directory)
{
    QDir dir(directory);
    if (!dir.exists()) {
        return QStringList();
    }
    return dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
}

QStringList FileUtils::listAll(const QString &directory)
{
    QDir dir(directory);
    if (!dir.exists()) {
        return QStringList();
    }
    return dir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot);
}

QString FileUtils::tempDirectory()
{
    return QDir::tempPath();
}

QString FileUtils::tempFilePath(const QString &prefix)
{
    if (prefix.isEmpty()) {
        return QDir::temp().absoluteFilePath("tmp");
    }
    return QDir::temp().absoluteFilePath(prefix + "XXXXXX");
}

QString FileUtils::getAppDataDirectory(const QString &appName)
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    return QDir::cleanPath(path);
}

QString FileUtils::getAppConfigDirectory(const QString &appName)
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    return QDir::cleanPath(path);
}

QString FileUtils::getAppCacheDirectory(const QString &appName)
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    return QDir::cleanPath(path);
}

bool FileUtils::isValidPath(const QString &path)
{
    if (path.isEmpty()) {
        return false;
    }

    QFileInfo info(path);
    return info.isAbsolute() || info.fileName().length() > 0;
}

bool FileUtils::isValidFileName(const QString &fileName)
{
    if (fileName.isEmpty()) {
        return false;
    }

    QString invalidChars = "<>:\"/\\|?*";
    foreach (QChar c, invalidChars) {
        if (fileName.contains(c)) {
            return false;
        }
    }

    QFileInfo info(fileName);
    if (info.fileName() != fileName) {
        return false;
    }

    return true;
}

bool FileUtils::isAbsolutePath(const QString &path)
{
    QFileInfo info(path);
    return info.isAbsolute();
}

QString FileUtils::toAbsolutePath(const QString &path)
{
    QFileInfo info(path);
    return info.absoluteFilePath();
}

QString FileUtils::getFileIconPath(const QString &filePath)
{
    QString ext = fileExtension(filePath).toLower();

    static QMap<QString, QString> iconMap;
    if (iconMap.isEmpty()) {
        iconMap["txt"] = ":/icons/file-text.svg";
        iconMap["pdf"] = ":/icons/file-pdf.svg";
        iconMap["doc"] = ":/icons/file-word.svg";
        iconMap["docx"] = ":/icons/file-word.svg";
        iconMap["xls"] = ":/icons/file-excel.svg";
        iconMap["xlsx"] = ":/icons/file-excel.svg";
        iconMap["ppt"] = ":/icons/file-powerpoint.svg";
        iconMap["pptx"] = ":/icons/file-powerpoint.svg";
        iconMap["jpg"] = ":/icons/file-image.svg";
        iconMap["jpeg"] = ":/icons/file-image.svg";
        iconMap["png"] = ":/icons/file-image.svg";
        iconMap["gif"] = ":/icons/file-image.svg";
        iconMap["bmp"] = ":/icons/file-image.svg";
        iconMap["svg"] = ":/icons/file-image.svg";
        iconMap["mp3"] = ":/icons/file-audio.svg";
        iconMap["wav"] = ":/icons/file-audio.svg";
        iconMap["ogg"] = ":/icons/file-audio.svg";
        iconMap["mp4"] = ":/icons/file-video.svg";
        iconMap["avi"] = ":/icons/file-video.svg";
        iconMap["mkv"] = ":/icons/file-video.svg";
        iconMap["mov"] = ":/icons/file-video.svg";
        iconMap["zip"] = ":/icons/file-archive.svg";
        iconMap["rar"] = ":/icons/file-archive.svg";
        iconMap["7z"] = ":/icons/file-archive.svg";
        iconMap["tar"] = ":/icons/file-archive.svg";
        iconMap["gz"] = ":/icons/file-archive.svg";
        iconMap["cpp"] = ":/icons/file-code.svg";
        iconMap["h"] = ":/icons/file-code.svg";
        iconMap["hpp"] = ":/icons/file-code.svg";
        iconMap["c"] = ":/icons/file-code.svg";
        iconMap["py"] = ":/icons/file-code.svg";
        iconMap["js"] = ":/icons/file-code.svg";
        iconMap["html"] = ":/icons/file-code.svg";
        iconMap["css"] = ":/icons/file-code.svg";
        iconMap["json"] = ":/icons/file-code.svg";
        iconMap["xml"] = ":/icons/file-code.svg";
    }

    return iconMap.value(ext, ":/icons/file.svg");
}

QString FileUtils::getFileType(const QString &filePath)
{
    QString ext = fileExtension(filePath).toLower();

    static QMap<QString, QString> typeMap;
    if (typeMap.isEmpty()) {
        typeMap["txt"] = QObject::tr("Text File");
        typeMap["pdf"] = QObject::tr("PDF Document");
        typeMap["doc"] = QObject::tr("Word Document");
        typeMap["docx"] = QObject::tr("Word Document");
        typeMap["xls"] = QObject::tr("Excel Spreadsheet");
        typeMap["xlsx"] = QObject::tr("Excel Spreadsheet");
        typeMap["ppt"] = QObject::tr("PowerPoint Presentation");
        typeMap["pptx"] = QObject::tr("PowerPoint Presentation");
        typeMap["jpg"] = QObject::tr("Image");
        typeMap["jpeg"] = QObject::tr("Image");
        typeMap["png"] = QObject::tr("Image");
        typeMap["gif"] = QObject::tr("Image");
        typeMap["bmp"] = QObject::tr("Image");
        typeMap["svg"] = QObject::tr("SVG Image");
        typeMap["mp3"] = QObject::tr("Audio File");
        typeMap["wav"] = QObject::tr("Audio File");
        typeMap["ogg"] = QObject::tr("Audio File");
        typeMap["mp4"] = QObject::tr("Video File");
        typeMap["avi"] = QObject::tr("Video File");
        typeMap["mkv"] = QObject::tr("Video File");
        typeMap["mov"] = QObject::tr("Video File");
        typeMap["zip"] = QObject::tr("Archive");
        typeMap["rar"] = QObject::tr("Archive");
        typeMap["7z"] = QObject::tr("Archive");
        typeMap["tar"] = QObject::tr("Archive");
        typeMap["gz"] = QObject::tr("Archive");
        typeMap["cpp"] = QObject::tr("C++ Source");
        typeMap["h"] = QObject::tr("C++ Header");
        typeMap["hpp"] = QObject::tr("C++ Header");
        typeMap["c"] = QObject::tr("C Source");
        typeMap["py"] = QObject::tr("Python Source");
        typeMap["js"] = QObject::tr("JavaScript");
        typeMap["html"] = QObject::tr("HTML");
        typeMap["css"] = QObject::tr("CSS");
        typeMap["json"] = QObject::tr("JSON");
        typeMap["xml"] = QObject::tr("XML");
    }

    return typeMap.value(ext, QObject::tr("Unknown File"));
}

QString FileUtils::sanitizeFileName(const QString &fileName)
{
    QString result = fileName;
    QString invalidChars = "<>:\"/\\|?*";

    foreach (QChar c, invalidChars) {
        result.remove(c);
    }

    result = result.simplified().replace(" ", "_");

    return result;
}

qint64 FileUtils::calculateDirectorySize(const QString &directory)
{
    qint64 totalSize = 0;
    QDir dir(directory);

    if (!dir.exists()) {
        return 0;
    }

    QFileInfoList fileList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    foreach (QFileInfo fileInfo, fileList) {
        totalSize += fileInfo.size();
    }

    QFileInfoList dirList = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    foreach (QFileInfo dirInfo, dirList) {
        totalSize += calculateDirectorySize(dirInfo.absoluteFilePath());
    }

    return totalSize;
}
