#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QTextStream>
#include <QFile>
#include <QDateTime>

class Logger
{
public:
    enum Level {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        CRITICAL
    };

    static Logger& instance();

    void log(Level level, const QString &category, const QString &message);
    void debug(const QString &category, const QString &message);
    void info(const QString &category, const QString &message);
    void warning(const QString &category, const QString &message);
    void error(const QString &category, const QString &message);
    void critical(const QString &category, const QString &message);

    void setMinLevel(Level level);
    Level minLevel() const;

    void setLogDirectory(const QString &directory);
    QString logDirectory() const;

    void clearAllLogs();

    static QString levelToString(Level level);
    static Level stringToLevel(const QString &levelStr);

private:
    Logger();
    ~Logger();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void rotateLogIfNeeded();
    void cleanOldLogs();
    QString getLogFilePath();

    QFile m_logFile;
    QTextStream m_logStream;
    Level m_minLevel;
    QString m_logDirectory;
    qint64 m_maxFileSize;
    int m_maxLogFiles;
};

#define LOG_DEBUG(category, message) Logger::instance().debug(category, message)
#define LOG_INFO(category, message) Logger::instance().info(category, message)
#define LOG_WARNING(category, message) Logger::instance().warning(category, message)
#define LOG_ERROR(category, message) Logger::instance().error(category, message)
#define LOG_CRITICAL(category, message) Logger::instance().critical(category, message)

#endif // LOGGER_H
