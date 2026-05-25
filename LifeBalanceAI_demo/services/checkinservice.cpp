#include "checkinservice.h"
#include "freezecardservice.h"
#include "databasemanager.h"
#include <QDate>
#include <QDebug>

namespace LifeBalanceAI {
namespace Services {

CheckinService::CheckinService(QObject *parent)
    : QObject(parent)
{
}

Models::CheckinResult CheckinService::processDailyCheckin(int userId)
{
    Models::CheckinResult result;
    auto &db = DatabaseManager::instance();
    QDate today = QDate::currentDate();
    QString todayStr = today.toString("yyyy-MM-dd");

    bool hasCheckinToday = db.hasCheckinOnDate(userId, today);
    qDebug() << "processDailyCheckin: userId=" << userId << "today=" << todayStr << "hasCheckin=" << hasCheckinToday;

    if (!hasCheckinToday) {
        result.checkedIn = false;
        result.streakBroken = false;
        Models::UserInfo info = db.getUserInfo(userId);
        QDate yesterday = today.addDays(-1);
        QString yesterdayStr = yesterday.toString("yyyy-MM-dd");
        bool yesterdayHadCheckin = db.hasCheckinOnDate(userId, yesterday);
        QString lastDate = db.getLastStreakDate(userId);

        // Freeze card protection: only trigger if YESTERDAY also had no checkin
        // (i.e. we missed a day — not just haven't checked in yet today)
        if (info.streakDays > 0 && !yesterdayHadCheckin && lastDate == yesterdayStr) {
            result.newStreak = info.streakDays;
            result.message = QStringLiteral("昨日未打卡，连续天数已保护");
        } else if (info.streakDays > 0 && !yesterdayHadCheckin &&
            m_freezeCardService && info.role == QStringLiteral("Ascendant")) {
            Models::FreezeResult fr = m_freezeCardService->tryProtectStreak(userId);
            if (fr.streakLost) {
                db.setStreakDays(userId, 0);
                db.setGraceDays(userId, 0);
                result.newStreak = 0;
                result.streakBroken = true;
                result.message = QStringLiteral("冻结卡和宽限期已耗尽，打卡中断！");
            } else {
                db.setLastStreakDate(userId, yesterdayStr);
                result.newStreak = info.streakDays;
                result.message = fr.remainingCards > 0
                    ? QStringLiteral("昨日未打卡，已使用冻结卡（剩余 %1 张）").arg(fr.remainingCards)
                    : QStringLiteral("昨日未打卡，宽限期第 %1/3 天").arg(fr.graceDaysUsed);
            }
        } else {
            result.newStreak = info.streakDays;
            result.message = QStringLiteral("今日尚未打卡");
        }

        result.reprofileRequired = shouldForceReprofile(userId);
        emit checkinCompleted(userId, result);
        return result;
    }

    // Idempotent: already processed today?
    QString lastDate = db.getLastStreakDate(userId);
    qDebug() << "processDailyCheckin: lastDate=" << lastDate << "todayStr=" << todayStr;
    if (lastDate == todayStr) {
        result.checkedIn = true;
        result.streakBroken = false;
        Models::UserInfo info = db.getUserInfo(userId);
        result.newStreak = info.streakDays;
        result.message = QStringLiteral("今日已打卡！连续 %1 天").arg(info.streakDays);
        result.reprofileRequired = shouldForceReprofile(userId);
        emit checkinCompleted(userId, result);
        return result;
    }

    // First checkin today — update streak
    Models::UserInfo info = db.getUserInfo(userId);
    int streak = info.streakDays;
    QDate yesterday = today.addDays(-1);
    QString yesterdayStr = yesterday.toString("yyyy-MM-dd");
    bool yesterdayHadCheckin = db.hasCheckinOnDate(userId, yesterday);
    bool yesterdayProtected = (lastDate == yesterdayStr);
    qDebug() << "processDailyCheckin: yesterday=" << yesterday.toString("yyyy-MM-dd") << "hadCheckin=" << yesterdayHadCheckin << "streak=" << streak;

    if (yesterdayHadCheckin || yesterdayProtected) {
        streak++;
        result.streakBroken = false;
    } else if (streak == 0) {
        streak = 1;
        result.streakBroken = false;
    } else if (info.role == QStringLiteral("Ascendant") && m_freezeCardService) {
        Models::FreezeResult fr = m_freezeCardService->tryProtectStreak(userId);
        if (!fr.streakLost) {
            streak++;
            result.streakBroken = false;
        } else {
            streak = 1;
            result.streakBroken = true;
        }
    } else {
        streak = 1;
        result.streakBroken = true;
    }

    db.setStreakDays(userId, streak);
    db.incrementTotalCheckins(userId);
    db.setGraceDays(userId, 0);
    db.setLastStreakDate(userId, todayStr);

    result.checkedIn = true;
    result.newStreak = streak;
    result.message = result.streakBroken
        ? QStringLiteral("打卡中断后重新开始！连续 %1 天").arg(streak)
        : QStringLiteral("今日已打卡！连续 %1 天").arg(streak);

    result.reprofileRequired = shouldForceReprofile(userId);

    emit checkinCompleted(userId, result);
    if (result.reprofileRequired)
        emit reprofileRequired(userId);

    return result;
}

Models::StreakInfo CheckinService::getStreakInfo(int userId)
{
    Models::StreakInfo info;
    Models::UserInfo u = DatabaseManager::instance().getUserInfo(userId);
    info.streakDays    = u.streakDays;
    info.totalCheckins = u.totalCheckins;
    info.freezeCards   = u.freezeCards;
    info.graceDays     = u.graceDays;
    return info;
}

bool CheckinService::shouldForceReprofile(int userId)
{
    int undone = DatabaseManager::instance().countUndoneSlots(userId, 7);
    qDebug() << "shouldForceReprofile: undone slots =" << undone << "for user" << userId;
    return undone >= 20;
}


void CheckinService::setFreezeCardService(FreezeCardService *svc)
{
    m_freezeCardService = svc;
}

} // namespace Services
} // namespace LifeBalanceAI
