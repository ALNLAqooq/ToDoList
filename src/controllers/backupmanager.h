#ifndef BACKUPMANAGER_H
#define BACKUPMANAGER_H

#include <QObject>
#include <QTimer>
#include <QDateTime>
#include <QString>
#include <QStringList>
#include <QFileInfo>

class Database;
class Notification;

class BackupManager : public QObject
{
    Q_OBJECT

public:
    enum BackupFrequency {
        Hourly,
        Daily,
        Weekly,
        Manual
    };

    enum BackupResult {
        Success,
        FailedDiskFull,
        FailedPermission,
        FailedDatabaseLocked,
        FailedInvalidPath,
        FailedUnknown
    };

    explicit BackupManager(QObject *parent = nullptr);
    ~BackupManager();

    bool initialize();

    QString backupLocation() const;
    bool setBackupLocation(const QString &path);

    BackupFrequency backupFrequency() const;
    bool setBackupFrequency(BackupFrequency frequency);

    int backupRetention() const;
    bool setBackupRetention(int count);

    QTime backupTime() const;
    bool setBackupTime(const QTime &time);

    bool backupOnExit() const;
    bool setBackupOnExit(bool enabled);

    bool autoBackupEnabled() const;
    bool setAutoBackupEnabled(bool enabled);

    BackupResult performBackup(const QString &description = QString());
    bool restoreBackup(const QString &backupPath);
    QStringList getBackupList() const;
    QString getLatestBackup() const;
    bool deleteBackup(const QString &backupPath);
    void cleanupOldBackups();

    bool isBackupInProgress() const;
    int backupProgress() const;
    QString currentBackupFile() const;

    static QString getBackupFileName(const QDateTime &dateTime);
    static bool isValidBackupFile(const QString &path);

signals:
    void backupStarted();
    void backupFinished(bool success, const QString &backupPath, BackupResult result);
    void backupProgressChanged(int progress);
    void backupRestored(const QString &backupPath, bool success);

private slots:
    void onBackupTimer();
    void onBackupProgress(qint64 bytesSent, qint64 bytesTotal);

private:
    bool ensureBackupDirectory();
    BackupResult copyDatabase(const QString &destination);
    void scheduleNextBackup();
    QDateTime getNextBackupTime() const;
    void saveBackupHistory(const QString &backupPath, const QString &description);
    bool sendBackupNotification(const QString &message, bool success);

    Database *m_database;
    QTimer *m_backupTimer;
    bool m_backupInProgress;
    int m_backupProgress;
    QString m_currentBackupFile;

    QString m_backupLocation;
    BackupFrequency m_backupFrequency;
    int m_backupRetention;
    QTime m_backupTime;
    bool m_backupOnExit;
    bool m_autoBackupEnabled;

    static constexpr int DEFAULT_RETENTION = 7;
    static constexpr BackupFrequency DEFAULT_FREQUENCY = Daily;
    static const QTime DEFAULT_BACKUP_TIME;
    static const QString BACKUP_FILENAME_PREFIX;
    static const QString BACKUP_FILE_EXTENSION;
    static const QString DEFAULT_BACKUP_DIR;
};

#endif // BACKUPMANAGER_H
