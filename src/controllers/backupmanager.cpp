#include "backupmanager.h"
#include "database.h"
#include "../models/notification.h"
#include "../controllers/notificationmanager.h"
#include "../utils/file_utils.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTimer>
#include <QDateTime>
#include <QDebug>
#include <QStandardPaths>
#include <QStorageInfo>
#include <QSqlQuery>
#include <QSqlDatabase>

const QString BackupManager::BACKUP_FILENAME_PREFIX = "todolist_backup_";
const QString BackupManager::BACKUP_FILE_EXTENSION = ".db";
const QString BackupManager::DEFAULT_BACKUP_DIR = "backup";
const QTime BackupManager::DEFAULT_BACKUP_TIME = QTime(2, 0);

BackupManager::BackupManager(QObject *parent)
    : QObject(parent)
    , m_database(nullptr)
    , m_backupTimer(new QTimer(this))
    , m_backupInProgress(false)
    , m_backupProgress(0)
    , m_backupFrequency(DEFAULT_FREQUENCY)
    , m_backupRetention(DEFAULT_RETENTION)
    , m_backupTime(DEFAULT_BACKUP_TIME)
    , m_backupOnExit(false)
    , m_autoBackupEnabled(true)
{
    connect(m_backupTimer, &QTimer::timeout, this, &BackupManager::onBackupTimer);
}

BackupManager::~BackupManager()
{
    if (m_backupOnExit && !m_backupInProgress) {
        performBackup(tr("Backup on exit"));
    }
}

bool BackupManager::initialize()
{
    m_database = &Database::instance();

    m_backupLocation = m_database->getSetting("backup_location", QDir::currentPath() + "/" + DEFAULT_BACKUP_DIR);

    QString frequencyStr = m_database->getSetting("backup_frequency", QString::number(DEFAULT_FREQUENCY));
    m_backupFrequency = static_cast<BackupFrequency>(frequencyStr.toInt());

    m_backupRetention = m_database->getSetting("backup_retention", QString::number(DEFAULT_RETENTION)).toInt();

    QString timeStr = m_database->getSetting("backup_time", DEFAULT_BACKUP_TIME.toString("HH:mm"));
    m_backupTime = QTime::fromString(timeStr, "HH:mm");

    m_backupOnExit = m_database->getSetting("backup_on_exit", "0").toInt() == 1;
    m_autoBackupEnabled = m_database->getSetting("auto_backup_enabled", "1").toInt() == 1;

    if (!ensureBackupDirectory()) {
        qDebug() << "Failed to ensure backup directory";
        return false;
    }

    if (m_autoBackupEnabled) {
        scheduleNextBackup();
    }

    return true;
}

QString BackupManager::backupLocation() const
{
    return m_backupLocation;
}

bool BackupManager::setBackupLocation(const QString &path)
{
    QString normalizedPath = FileUtils::normalizePath(path);

    if (!FileUtils::isValidPath(normalizedPath)) {
        return false;
    }

    if (!QFile::exists(normalizedPath)) {
        if (!QDir().mkpath(normalizedPath)) {
            return false;
        }
    }

    if (!FileUtils::isWritable(normalizedPath)) {
        return false;
    }

    m_backupLocation = normalizedPath;
    m_database->setSetting("backup_location", m_backupLocation);
    return true;
}

BackupManager::BackupFrequency BackupManager::backupFrequency() const
{
    return m_backupFrequency;
}

bool BackupManager::setBackupFrequency(BackupFrequency frequency)
{
    m_backupFrequency = frequency;
    m_database->setSetting("backup_frequency", QString::number(static_cast<int>(frequency)));

    if (m_autoBackupEnabled) {
        scheduleNextBackup();
    }

    return true;
}

int BackupManager::backupRetention() const
{
    return m_backupRetention;
}

bool BackupManager::setBackupRetention(int count)
{
    if (count < 1) {
        return false;
    }

    m_backupRetention = count;
    m_database->setSetting("backup_retention", QString::number(count));
    cleanupOldBackups();
    return true;
}

QTime BackupManager::backupTime() const
{
    return m_backupTime;
}

bool BackupManager::setBackupTime(const QTime &time)
{
    if (!time.isValid()) {
        return false;
    }

    m_backupTime = time;
    m_database->setSetting("backup_time", time.toString("HH:mm"));

    if (m_autoBackupEnabled) {
        scheduleNextBackup();
    }

    return true;
}

bool BackupManager::backupOnExit() const
{
    return m_backupOnExit;
}

bool BackupManager::setBackupOnExit(bool enabled)
{
    m_backupOnExit = enabled;
    m_database->setSetting("backup_on_exit", enabled ? "1" : "0");
    return true;
}

bool BackupManager::autoBackupEnabled() const
{
    return m_autoBackupEnabled;
}

bool BackupManager::setAutoBackupEnabled(bool enabled)
{
    m_autoBackupEnabled = enabled;
    m_database->setSetting("auto_backup_enabled", enabled ? "1" : "0");

    if (enabled) {
        scheduleNextBackup();
    } else {
        m_backupTimer->stop();
    }

    return true;
}

BackupManager::BackupResult BackupManager::performBackup(const QString &description)
{
    if (m_backupInProgress) {
        return FailedDatabaseLocked;
    }

    if (!ensureBackupDirectory()) {
        return FailedInvalidPath;
    }

    QStorageInfo storage(m_backupLocation);
    if (storage.bytesAvailable() < 50 * 1024 * 1024) {
        return FailedDiskFull;
    }

    m_backupInProgress = true;
    m_backupProgress = 0;
    emit backupStarted();

    QString backupFileName = getBackupFileName(QDateTime::currentDateTime());
    QString backupPath = QDir(m_backupLocation).filePath(backupFileName);
    m_currentBackupFile = backupPath;

    BackupResult result = copyDatabase(backupPath);

    if (result == Success) {
        saveBackupHistory(backupPath, description);
        cleanupOldBackups();
        sendBackupNotification(QString::fromUtf8("\xE5\xA4\x87\xE4\xBB\xBD\xE5\xAE\x8C\xE6\x88\x90\xEF\xBC\x9A\x25\x31").arg(backupFileName), true);
    } else {
        QString errorMessage;
        switch (result) {
            case FailedDiskFull:
                errorMessage = QString::fromUtf8("\xE5\xA4\x87\xE4\xBB\xBD\xE5\xA4\xB1\xE8\xB4\xA5\xEF\xBC\x9A\xE7\xA3\x81\xE7\x9B\x98\xE7\xA9\xBA\xE9\x97\xB4\xE4\xB8\x8D\xE8\xB6\xB3");
                break;
            case FailedPermission:
                errorMessage = QString::fromUtf8("\xE5\xA4\x87\xE4\xBB\xBD\xE5\xA4\xB1\xE8\xB4\xA5\xEF\xBC\x9A\xE6\x9D\x83\xE9\x99\x90\xE4\xB8\x8D\xE8\xB6\xB3");
                break;
            case FailedDatabaseLocked:
                errorMessage = QString::fromUtf8("\xE5\xA4\x87\xE4\xBB\xBD\xE5\xA4\xB1\xE8\xB4\xA5\xEF\xBC\x9A\xE6\x95\xB0\xE6\x8D\xAE\xE5\xBA\x93\xE8\xA2\xAB\xE5\x8D\xA0\xE7\x94\xA8");
                break;
            case FailedInvalidPath:
                errorMessage = QString::fromUtf8("\xE5\xA4\x87\xE4\xBB\xBD\xE5\xA4\xB1\xE8\xB4\xA5\xEF\xBC\x9A\xE5\xA4\x87\xE4\xBB\xBD\xE8\xB7\xAF\xE5\xBE\x84\xE6\x97\xA0\xE6\x95\x88");
                break;
            default:
                errorMessage = QString::fromUtf8("\xE5\xA4\x87\xE4\xBB\xBD\xE5\xA4\xB1\xE8\xB4\xA5\xEF\xBC\x9A\xE6\x9C\xAA\xE7\x9F\xA5\xE9\x94\x99\xE8\xAF\xAF");
                break;
        }
        sendBackupNotification(errorMessage, false);
    }

    m_backupInProgress = false;
    m_backupProgress = 100;
    m_currentBackupFile.clear();

    emit backupFinished(result == Success, backupPath, result);
    emit backupProgressChanged(100);

    return result;
}

bool BackupManager::restoreBackup(const QString &backupPath)
{
    if (!QFile::exists(backupPath)) {
        return false;
    }

    if (!isValidBackupFile(backupPath)) {
        return false;
    }

    QString dbPath = m_database->database().databaseName();
    QString dbDir = QFileInfo(dbPath).absolutePath();

    QString tempPath = dbDir + "/temp_restore.db";

    if (!FileUtils::copyFile(backupPath, tempPath)) {
        return false;
    }

    m_database->close();

    if (QFile::exists(dbPath)) {
        QFile::remove(dbPath + ".bak");
        QFile::rename(dbPath, dbPath + ".bak");
    }

    bool success = QFile::rename(tempPath, dbPath);

    if (!success) {
        if (QFile::exists(dbPath + ".bak")) {
            QFile::rename(dbPath + ".bak", dbPath);
        }
    }

    m_database->open();
    emit backupRestored(backupPath, success);

    return success;
}

QStringList BackupManager::getBackupList() const
{
    QStringList backups;
    QDir backupDir(m_backupLocation);

    if (!backupDir.exists()) {
        return backups;
    }

    QStringList filters;
    filters << BACKUP_FILENAME_PREFIX + "*" + BACKUP_FILE_EXTENSION;

    QFileInfoList files = backupDir.entryInfoList(filters, QDir::Files, QDir::Time | QDir::Reversed);

    for (const QFileInfo &fileInfo : files) {
        backups.append(fileInfo.absoluteFilePath());
    }

    return backups;
}

QString BackupManager::getLatestBackup() const
{
    QStringList backups = getBackupList();
    return backups.isEmpty() ? QString() : backups.last();
}

bool BackupManager::deleteBackup(const QString &backupPath)
{
    if (!QFile::exists(backupPath)) {
        return false;
    }

    return QFile::remove(backupPath);
}

void BackupManager::cleanupOldBackups()
{
    QStringList backups = getBackupList();

    while (backups.size() > m_backupRetention) {
        QString oldestBackup = backups.takeFirst();
        if (!deleteBackup(oldestBackup)) {
            qDebug() << "Failed to delete old backup:" << oldestBackup;
        }
    }
}

bool BackupManager::isBackupInProgress() const
{
    return m_backupInProgress;
}

int BackupManager::backupProgress() const
{
    return m_backupProgress;
}

QString BackupManager::currentBackupFile() const
{
    return m_currentBackupFile;
}

QString BackupManager::getBackupFileName(const QDateTime &dateTime)
{
    return BACKUP_FILENAME_PREFIX + dateTime.toString("yyyyMMdd_HHmmss") + BACKUP_FILE_EXTENSION;
}

bool BackupManager::isValidBackupFile(const QString &path)
{
    if (!QFile::exists(path)) {
        return false;
    }

    QFileInfo fileInfo(path);
    if (fileInfo.suffix().toLower() != "db") {
        return false;
    }

    QString baseName = fileInfo.baseName();
    if (!baseName.startsWith(BACKUP_FILENAME_PREFIX)) {
        return false;
    }

    return true;
}

void BackupManager::onBackupTimer()
{
    if (!m_autoBackupEnabled) {
        return;
    }

    performBackup(tr("Scheduled backup"));
    scheduleNextBackup();
}

void BackupManager::onBackupProgress(qint64 bytesSent, qint64 bytesTotal)
{
    if (bytesTotal > 0) {
        m_backupProgress = static_cast<int>((bytesSent * 100) / bytesTotal);
        emit backupProgressChanged(m_backupProgress);
    }
}

bool BackupManager::ensureBackupDirectory()
{
    QDir dir(m_backupLocation);

    if (!dir.exists()) {
        return dir.mkpath(".");
    }

    return true;
}

BackupManager::BackupResult BackupManager::copyDatabase(const QString &destination)
{
    QString sourcePath = m_database->database().databaseName();

    if (!QFile::exists(sourcePath)) {
        return FailedUnknown;
    }

    if (QFile::exists(destination)) {
        if (!QFile::remove(destination)) {
            return FailedPermission;
        }
    }

    QFile sourceFile(sourcePath);
    if (!sourceFile.open(QIODevice::ReadOnly)) {
        return FailedDatabaseLocked;
    }

    qint64 fileSize = sourceFile.size();
    m_backupProgress = 0;

    QFile destFile(destination);
    if (!destFile.open(QIODevice::WriteOnly)) {
        sourceFile.close();
        return FailedPermission;
    }

    const qint64 chunkSize = 1024 * 1024;
    qint64 bytesCopied = 0;
    char *buffer = new char[chunkSize];

    while (!sourceFile.atEnd()) {
        qint64 bytesRead = sourceFile.read(buffer, chunkSize);
        qint64 bytesWritten = destFile.write(buffer, bytesRead);

        if (bytesWritten != bytesRead) {
            delete[] buffer;
            sourceFile.close();
            destFile.close();
            destFile.remove();
            return FailedDiskFull;
        }

        bytesCopied += bytesWritten;
        onBackupProgress(bytesCopied, fileSize);
    }

    delete[] buffer;
    sourceFile.close();
    destFile.close();

    if (destFile.size() != fileSize) {
        return FailedDiskFull;
    }

    return Success;
}

void BackupManager::scheduleNextBackup()
{
    m_backupTimer->stop();

    if (!m_autoBackupEnabled) {
        return;
    }

    QDateTime nextBackup = getNextBackupTime();
    qint64 milliseconds = QDateTime::currentDateTime().msecsTo(nextBackup);

    if (milliseconds <= 0) {
        milliseconds = 60000;
    }

    m_backupTimer->start(static_cast<int>(milliseconds));
}

QDateTime BackupManager::getNextBackupTime() const
{
    QDateTime now = QDateTime::currentDateTime();
    QDateTime nextBackup;

    switch (m_backupFrequency) {
        case Hourly:
            nextBackup = now.addSecs(3600);
            nextBackup = QDateTime(nextBackup.date(), QTime(nextBackup.time().hour(), 0));
            if (nextBackup <= now) {
                nextBackup = nextBackup.addSecs(3600);
            }
            break;

        case Daily:
            nextBackup = QDateTime(now.date(), m_backupTime);
            if (nextBackup <= now) {
                nextBackup = nextBackup.addDays(1);
            }
            break;

        case Weekly:
            nextBackup = QDateTime(now.date(), m_backupTime);
            while (nextBackup.date().dayOfWeek() != 1) {
                nextBackup = nextBackup.addDays(1);
            }
            if (nextBackup <= now) {
                nextBackup = nextBackup.addDays(7);
            }
            break;

        default:
            nextBackup = now.addDays(1);
            break;
    }

    return nextBackup;
}

void BackupManager::saveBackupHistory(const QString &backupPath, const QString &description)
{
    QFileInfo fileInfo(backupPath);
    QString historyMessage = description.isEmpty() ? tr("Manual backup") : description;

    QSqlQuery query(m_database->database());
    query.prepare(R"(
        INSERT INTO backup_history (file_path, file_size, backup_time, description)
        VALUES (?, ?, ?, ?)
    )");
    query.addBindValue(backupPath);
    query.addBindValue(fileInfo.size());
    query.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    query.addBindValue(historyMessage);
    query.exec();
}

bool BackupManager::sendBackupNotification(const QString &message, bool success)
{
    NotificationManager &manager = NotificationManager::instance();
    return manager.addNotification(Notification::Backup,
                                   success ? QString::fromUtf8("\xE5\xA4\x87\xE4\xBB\xBD\xE5\xAE\x8C\xE6\x88\x90") : QString::fromUtf8("\xE5\xA4\x87\xE4\xBB\xBD\xE5\xA4\xB1\xE8\xB4\xA5"),
                                   message);
}
