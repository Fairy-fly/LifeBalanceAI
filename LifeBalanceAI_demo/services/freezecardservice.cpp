#include "freezecardservice.h"
#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDate>
#include <QDebug>

namespace LifeBalanceAI {
namespace Services {

FreezeCardService::FreezeCardService(QObject *parent)
    : QObject(parent)
{
}

void FreezeCardService::processMonthlyRefill(int userId)
{
    auto &db = DatabaseManager::instance();
    QDate today = QDate::currentDate();

    // Grant cards on first login of each month (no longer restricted to 1st)

    // Only for Ascendant users
    Models::UserInfo info = db.getUserInfo(userId);
    if (info.role != QStringLiteral("Ascendant"))
        return;

    // Check if already refilled this month (anti-duplicate)
    // Query Freeze_Cards_Log for 'monthly_refill' in current month
    QString thisMonth = today.toString("yyyy-MM");
    QSqlQuery checkQuery(db.database());
    checkQuery.prepare(QStringLiteral(
        "SELECT 1 FROM Freeze_Cards_Log WHERE UID = :uid"
        " AND reason = 'monthly_refill'"
        " AND used_date LIKE :month"
    ));
    checkQuery.bindValue(QStringLiteral(":uid"),   userId);
    checkQuery.bindValue(QStringLiteral(":month"), thisMonth + QStringLiteral("%"));

    if (checkQuery.exec() && checkQuery.next()) {
        qDebug() << "processMonthlyRefill: already refilled this month for user" << userId;
        return;
    }

    // Refill: set freeze_cards to 3
    int current = db.getFreezeCards(userId);
    db.setFreezeCards(userId, qMin(current + 3, 5));
    db.insertFreezeCardLog(userId, today.toString("yyyy-MM-dd"),
                           QStringLiteral("monthly_refill"));

    qDebug() << "processMonthlyRefill: refilled 3 cards for user" << userId;
    emit cardsRefilled(userId, 3);
}

Models::FreezeResult FreezeCardService::tryProtectStreak(int userId)
{
    Models::FreezeResult result;
    auto &db = DatabaseManager::instance();
    QDate today = QDate::currentDate();

    int cards = db.getFreezeCards(userId);

    if (cards > 0) {
        // Consume one freeze card
        cards--;
        db.setFreezeCards(userId, cards);
        db.insertFreezeCardLog(userId, today.toString("yyyy-MM-dd"),
                               QStringLiteral("auto_protect"));
        result.streakProtected = true;
        result.remainingCards  = cards;
        result.graceDaysUsed   = 0;
        result.streakLost      = false;
        emit streakProtected(userId, result);
        return result;
    }

    // No freeze cards — check grace period
    int grace = db.getGraceDays(userId);

    if (grace < 3) {
        // Within grace period
        grace++;
        db.setGraceDays(userId, grace);
        result.streakProtected = true;
        result.remainingCards  = 0;
        result.graceDaysUsed   = grace;
        result.streakLost      = false;
        emit streakProtected(userId, result);
        return result;
    }

    // Grace exhausted — streak lost
    result.streakProtected = false;
    result.remainingCards  = 0;
    result.graceDaysUsed   = 3;
    result.streakLost      = true;
    emit streakLost(userId);
    return result;
}

void FreezeCardService::resetGrace(int userId)
{
    DatabaseManager::instance().setGraceDays(userId, 0);
}

} // namespace Services
} // namespace LifeBalanceAI
