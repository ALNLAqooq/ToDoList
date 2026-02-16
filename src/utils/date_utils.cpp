#include "date_utils.h"
#include <QDebug>

QString DateUtils::formatDateTime(const QDateTime &dateTime)
{
    if (!dateTime.isValid()) {
        return QString();
    }
    return dateTime.toString("yyyy-MM-dd HH:mm:ss");
}

QString DateUtils::formatDate(const QDate &date)
{
    if (!date.isValid()) {
        return QString();
    }
    return date.toString("yyyy-MM-dd");
}

QString DateUtils::formatTime(const QTime &time)
{
    if (!time.isValid()) {
        return QString();
    }
    return time.toString("HH:mm:ss");
}

QString DateUtils::formatRelativeDate(const QDateTime &dateTime)
{
    if (!dateTime.isValid()) {
        return QString();
    }

    QDateTime now = QDateTime::currentDateTime();
    qint64 seconds = dateTime.secsTo(now);

    if (seconds < 60) {
        return QObject::tr("Just now");
    } else if (seconds < 3600) {
        int minutes = seconds / 60;
        return QObject::tr("%1 minutes ago").arg(minutes);
    } else if (seconds < 86400) {
        int hours = seconds / 3600;
        return QObject::tr("%1 hours ago").arg(hours);
    } else if (seconds < 172800) {
        return QObject::tr("Yesterday");
    } else if (isThisWeek(dateTime)) {
        return dateTime.toString("dddd");
    } else if (isThisMonth(dateTime)) {
        return dateTime.toString("MMM d");
    } else {
        return dateTime.toString("MMM d, yyyy");
    }
}

QDate DateUtils::getStartOfWeek(const QDate &date)
{
    if (!date.isValid()) {
        return QDate();
    }
    return date.addDays(1 - date.dayOfWeek());
}

QDate DateUtils::getEndOfWeek(const QDate &date)
{
    if (!date.isValid()) {
        return QDate();
    }
    return date.addDays(7 - date.dayOfWeek());
}

QDate DateUtils::getStartOfMonth(const QDate &date)
{
    if (!date.isValid()) {
        return QDate();
    }
    return QDate(date.year(), date.month(), 1);
}

QDate DateUtils::getEndOfMonth(const QDate &date)
{
    if (!date.isValid()) {
        return QDate();
    }
    return QDate(date.year(), date.month(), date.daysInMonth());
}

QDate DateUtils::getStartOfYear(const QDate &date)
{
    if (!date.isValid()) {
        return QDate();
    }
    return QDate(date.year(), 1, 1);
}

QDate DateUtils::getEndOfYear(const QDate &date)
{
    if (!date.isValid()) {
        return QDate();
    }
    return QDate(date.year(), 12, 31);
}

bool DateUtils::isToday(const QDateTime &dateTime)
{
    if (!dateTime.isValid()) {
        return false;
    }
    return dateTime.date() == QDate::currentDate();
}

bool DateUtils::isToday(const QDate &date)
{
    if (!date.isValid()) {
        return false;
    }
    return date == QDate::currentDate();
}

bool DateUtils::isThisWeek(const QDateTime &dateTime)
{
    if (!dateTime.isValid()) {
        return false;
    }
    QDate today = QDate::currentDate();
    QDate startOfWeek = getStartOfWeek(today);
    QDate endOfWeek = getEndOfWeek(today);
    return dateTime.date() >= startOfWeek && dateTime.date() <= endOfWeek;
}

bool DateUtils::isThisMonth(const QDateTime &dateTime)
{
    if (!dateTime.isValid()) {
        return false;
    }
    QDate today = QDate::currentDate();
    return dateTime.date().year() == today.year() && dateTime.date().month() == today.month();
}

bool DateUtils::isOverdue(const QDateTime &deadline)
{
    if (!deadline.isValid()) {
        return false;
    }
    return deadline < QDateTime::currentDateTime();
}

int DateUtils::daysUntil(const QDateTime &target)
{
    if (!target.isValid()) {
        return 0;
    }
    QDateTime now = QDateTime::currentDateTime();
    return static_cast<int>(now.daysTo(target));
}

int DateUtils::daysBetween(const QDateTime &start, const QDateTime &end)
{
    if (!start.isValid() || !end.isValid()) {
        return 0;
    }
    return static_cast<int>(start.daysTo(end));
}

QDateTime DateUtils::addDays(const QDateTime &dateTime, int days)
{
    return dateTime.addDays(days);
}

QDateTime DateUtils::addWeeks(const QDateTime &dateTime, int weeks)
{
    return dateTime.addDays(weeks * 7);
}

QDateTime DateUtils::addMonths(const QDateTime &dateTime, int months)
{
    return dateTime.addMonths(months);
}

QList<QDate> DateUtils::getDaysInWeek(const QDate &date)
{
    QList<QDate> days;
    QDate start = getStartOfWeek(date);
    for (int i = 0; i < 7; ++i) {
        days.append(start.addDays(i));
    }
    return days;
}

QList<QDate> DateUtils::getDaysInMonth(const QDate &date)
{
    QList<QDate> days;
    QDate start = getStartOfMonth(date);
    int daysInMonth = start.daysInMonth();
    for (int i = 0; i < daysInMonth; ++i) {
        days.append(start.addDays(i));
    }
    return days;
}

QString DateUtils::getDayName(int dayOfWeek)
{
    switch (dayOfWeek) {
        case 1: return QObject::tr("Monday");
        case 2: return QObject::tr("Tuesday");
        case 3: return QObject::tr("Wednesday");
        case 4: return QObject::tr("Thursday");
        case 5: return QObject::tr("Friday");
        case 6: return QObject::tr("Saturday");
        case 7: return QObject::tr("Sunday");
        default: return QString();
    }
}

QString DateUtils::getMonthName(int month)
{
    switch (month) {
        case 1: return QObject::tr("January");
        case 2: return QObject::tr("February");
        case 3: return QObject::tr("March");
        case 4: return QObject::tr("April");
        case 5: return QObject::tr("May");
        case 6: return QObject::tr("June");
        case 7: return QObject::tr("July");
        case 8: return QObject::tr("August");
        case 9: return QObject::tr("September");
        case 10: return QObject::tr("October");
        case 11: return QObject::tr("November");
        case 12: return QObject::tr("December");
        default: return QString();
    }
}

QString DateUtils::getDayNameShort(int dayOfWeek)
{
    switch (dayOfWeek) {
        case 1: return QObject::tr("Mon");
        case 2: return QObject::tr("Tue");
        case 3: return QObject::tr("Wed");
        case 4: return QObject::tr("Thu");
        case 5: return QObject::tr("Fri");
        case 6: return QObject::tr("Sat");
        case 7: return QObject::tr("Sun");
        default: return QString();
    }
}

QString DateUtils::getMonthNameShort(int month)
{
    switch (month) {
        case 1: return QObject::tr("Jan");
        case 2: return QObject::tr("Feb");
        case 3: return QObject::tr("Mar");
        case 4: return QObject::tr("Apr");
        case 5: return QObject::tr("May");
        case 6: return QObject::tr("Jun");
        case 7: return QObject::tr("Jul");
        case 8: return QObject::tr("Aug");
        case 9: return QObject::tr("Sep");
        case 10: return QObject::tr("Oct");
        case 11: return QObject::tr("Nov");
        case 12: return QObject::tr("Dec");
        default: return QString();
    }
}

bool DateUtils::isValidDate(const QString &dateString, const QString &format)
{
    QDateTime dateTime = QDateTime::fromString(dateString, format);
    return dateTime.isValid();
}

QDateTime DateUtils::parseDateTime(const QString &dateTimeString, const QString &format)
{
    return QDateTime::fromString(dateTimeString, format);
}
