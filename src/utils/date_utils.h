#ifndef DATE_UTILS_H
#define DATE_UTILS_H

#include <QDateTime>
#include <QDate>
#include <QTime>
#include <QString>
#include <QList>

class DateUtils
{
public:
    static QString formatDateTime(const QDateTime &dateTime);
    static QString formatDate(const QDate &date);
    static QString formatTime(const QTime &time);
    static QString formatRelativeDate(const QDateTime &dateTime);

    static QDate getStartOfWeek(const QDate &date = QDate::currentDate());
    static QDate getEndOfWeek(const QDate &date = QDate::currentDate());
    static QDate getStartOfMonth(const QDate &date = QDate::currentDate());
    static QDate getEndOfMonth(const QDate &date = QDate::currentDate());
    static QDate getStartOfYear(const QDate &date = QDate::currentDate());
    static QDate getEndOfYear(const QDate &date = QDate::currentDate());

    static bool isToday(const QDateTime &dateTime);
    static bool isToday(const QDate &date);
    static bool isThisWeek(const QDateTime &dateTime);
    static bool isThisMonth(const QDateTime &dateTime);
    static bool isOverdue(const QDateTime &deadline);

    static int daysUntil(const QDateTime &target);
    static int daysBetween(const QDateTime &start, const QDateTime &end);

    static QDateTime addDays(const QDateTime &dateTime, int days);
    static QDateTime addWeeks(const QDateTime &dateTime, int weeks);
    static QDateTime addMonths(const QDateTime &dateTime, int months);

    static QList<QDate> getDaysInWeek(const QDate &date = QDate::currentDate());
    static QList<QDate> getDaysInMonth(const QDate &date = QDate::currentDate());

    static QString getDayName(int dayOfWeek);
    static QString getMonthName(int month);
    static QString getDayNameShort(int dayOfWeek);
    static QString getMonthNameShort(int month);

    static bool isValidDate(const QString &dateString, const QString &format = "yyyy-MM-dd");
    static QDateTime parseDateTime(const QString &dateTimeString, const QString &format = "yyyy-MM-dd HH:mm:ss");

private:
    DateUtils() = default;
    ~DateUtils() = default;
};

#endif // DATE_UTILS_H
