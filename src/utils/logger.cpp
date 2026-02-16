#include "logger.h"
#include <QDir>
#include <QFileInfo>
#include <QDebug>

Logger& Logger::instance()
{
    static Logger instance;
    return instance;
}

Logger::Logger()
    : m_minLevel(INFO)
    , m_logDirectory(QDir::currentPath() + "/logs")
    , m_maxFileSize(10 * 1024 * 1024)
    , m_maxLogFiles(30)
{
    QDir logDir(m_logDirectory);
    if (!logDir.exists()) {
        logDir.mkpath(".");
    }
}

Logger::~Logger()
{
    if (m_logFile.isOpen()) {
        m_logStream.flush();
        m_logFile.close();
    }
}

void Logger::log(Level level, const QString &category, const QString &message)
{
    if (level < m_minLevel) {
        return;
    }

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString levelStr = levelToString(level);
    QString logEntry = QString("[%1] [%2] [%3] %4")
        .arg(timestamp, levelStr, category, message);

    rotateLogIfNeeded();

    if (!m_logFile.isOpen()) {
        m_logFile.setFileName(getLogFilePath());
        if (!m_logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
            qDebug() << "Failed to open log file:" << m_logFile.errorString();
            return;
        }
        m_logStream.setDevice(&m_logFile);
    }

    m_logStream << logEntry << "\n";
    m_logStream.flush();

    if (level >= ERROR) {
        qDebug().noquote() << logEntry;
    }

    if (level == CRITICAL) {
        qCritical().noquote() << logEntry;
    }
}

void Logger::debug(const QString &category, const QString &message)
{
    log(DEBUG, category, message);
}

void Logger::info(const QString &category, const QString &message)
{
    log(INFO, category, message);
}

void Logger::warning(const QString &category, const QString &message)
{
    log(WARNING, category, message);
}

void Logger::error(const QString &category, const QString &message)
{
    log(ERROR, category, message);
}

void Logger::critical(const QString &category, const QString &message)
{
    log(CRITICAL, category, message);
}

void Logger::setMinLevel(Level level)
{
    m_minLevel = level;
}

Logger::Level Logger::minLevel() const
{
    return m_minLevel;
}

void Logger::setLogDirectory(const QString &directory)
{
    if (m_logFile.isOpen()) {
        m_logStream.flush();
        m_logFile.close();
    }

    m_logDirectory = directory;

    QDir logDir(m_logDirectory);
    if (!logDir.exists()) {
        logDir.mkpath(".");
    }
}

QString Logger::logDirectory() const
{
    return m_logDirectory;
}

void Logger::clearAllLogs()
{
    QDir logDir(m_logDirectory);
    QStringList logFiles = logDir.entryList(QStringList() << "*.log", QDir::Files);

    for (const QString &logFile : logFiles) {
        logDir.remove(logFile);
    }

    if (m_logFile.isOpen()) {
        m_logStream.flush();
        m_logFile.close();
    }
}

QString Logger::levelToString(Level level)
{
    switch (level) {
        case DEBUG:    return "DEBUG";
        case INFO:     return "INFO";
        case WARNING:  return "WARNING";
        case ERROR:    return "ERROR";
        case CRITICAL: return "CRITICAL";
        default:       return "UNKNOWN";
    }
}

Logger::Level Logger::stringToLevel(const QString &levelStr)
{
    QString upper = levelStr.toUpper();
    if (upper == "DEBUG") return DEBUG;
    if (upper == "INFO") return INFO;
    if (upper == "WARNING") return WARNING;
    if (upper == "ERROR") return ERROR;
    if (upper == "CRITICAL") return CRITICAL;
    return INFO;
}

void Logger::rotateLogIfNeeded()
{
    if (!m_logFile.isOpen()) {
        return;
    }

    m_logFile.flush();

    QFileInfo fileInfo(m_logFile);
    if (fileInfo.size() >= m_maxFileSize) {
        m_logFile.close();

        QString date = QDateTime::currentDateTime().toString("yyyyMMdd");
        QString rotatedFileName = QString("todolist_%1_1.log").arg(date);

        QString basePath = m_logDirectory + "/todolist_" + date;
        QString rotatedPath = m_logDirectory + "/" + rotatedFileName;

        int sequence = 1;
        while (QFile::exists(rotatedPath)) {
            sequence++;
            rotatedFileName = QString("todolist_%1_%2.log").arg(date).arg(sequence);
            rotatedPath = m_logDirectory + "/" + rotatedFileName;
        }

        QFile::rename(m_logFile.fileName(), rotatedPath);

        cleanOldLogs();
    }
}

void Logger::cleanOldLogs()
{
    QDir logDir(m_logDirectory);
    QStringList logFiles = logDir.entryList(QStringList() << "*.log", QDir::Files, QDir::Time);

    while (logFiles.size() > m_maxLogFiles) {
        QString oldestFile = logFiles.takeLast();
        logDir.remove(oldestFile);
    }
}

QString Logger::getLogFilePath()
{
    QString date = QDateTime::currentDateTime().toString("yyyyMMdd");
    return m_logDirectory + "/todolist_" + date + ".log";
}
