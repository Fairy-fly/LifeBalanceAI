#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QString>
#include <QSqlDatabase>
#include <QJsonObject>
#include <QJsonArray>
#include <QVector>
#include "models/dto.h"

class DatabaseManager
{
public:
    static DatabaseManager &instance();

    bool initDatabase();

    QSqlDatabase &database();

    // ── Profile helpers ──────────────────────────────────────────
    bool hasProfile(int userId);

    bool saveProfile(int userId, int age, double height, double weight,
                     const QString &gender, const QString &goal,
                     const QString &allergy, const QString &preferences,
                     const QString &nickname = QString());

    // ── Plan helpers ─────────────────────────────────────────────
    bool hasPlan(int userId);

    /** Creates a new plan and returns the new plan_id, or -1 on failure */
    int createPlan(int userId, const QString &type, const QString &longTermGoal);

    /** Saves one daily item row; returns true on success */
    bool saveDailyItem(int planId, const QString &date,
                       const QString &timeSlot, const QString &content);

    /** Updates the content of an existing daily item (for adjust-plan) */
    bool updateDailyItemContent(int itemId, const QString &newContent);

    /** Return the plan_id of the user's latest plan, or -1 if none */
    int getLatestPlanId(int userId);

    QString getLongTermGoal(int userId);

    // ── Daily_Items helpers ──────────────────────────────────────
    /** Return the id of a daily item, or -1 if not found */
    int getItemId(int planId, const QString &date, const QString &timeSlot);

    /** Get all daily items for a given date and plan */
    struct DailyItemRow {
        int     id;
        QString date;
        QString timeSlot;
        QString content;
        bool    isDone;
    };

    /** Get items for a specific date and plan, optionally filtering by is_done */
    QList<DailyItemRow> getItemsForDate(int planId, const QDate &date);

    /** Count how many items for today are not yet done (is_done=0) */
    int countUndoneToday(int planId);

    /** Get the full plan as JSON for context (used by partial update) */
    QJsonObject getPlanAsJson(int planId);

    // ── Feedback helpers ─────────────────────────────────────────
    /** Get the latest feedback content for an item, or empty string */
    QString getFeedbackContent(int itemId);

    /**
     * Save feedback in a idempotent way:
     * If a feedback row already exists for this item_id → UPDATE content.
     * Otherwise → INSERT new row.
     */
    bool saveFeedback(int userId, int itemId, const QString &content,
                      const QString &imgPath = QString());

    /** Mark a Daily_Items row as done (is_done = 1) */
    bool markItemDone(int itemId);
    bool markItemAdjusted(int itemId);

    /** Get user role string from Users table */
    QString getUserRole(int userId);
    bool updateUserRole(int userId, const QString &role);

    // ── Yesterday helpers ────────────────────────────────────────
    /** Get items for yesterday (today - 1 day), given a plan_id */
    QList<DailyItemRow> getYesterdayItems(int planId);


    // ── V3.0 CRUD helpers ────────────────────────────────────
    LifeBalanceAI::Models::UserInfo getUserInfo(int userId);
    LifeBalanceAI::Models::ProfileData getProfile(int userId);
    bool setStreakDays(int userId, int days);
    int getFreezeCards(int userId);
    bool setFreezeCards(int userId, int count);
    bool incrementTotalCheckins(int userId);
    bool insertFreezeCardLog(int userId, const QString &date,
                              const QString &reason);
    bool hasCheckinOnDate(int userId, const QDate &date);
    int countUndoneSlots(int userId, int recentDays);
    QDate getLastCheckinDate(int userId);
    bool setGraceDays(int userId, int days);
    int getGraceDays(int userId);
    QString getLastStreakDate(int userId);
    bool setLastStreakDate(int userId, const QString &date);
    QVector<LifeBalanceAI::Models::UserInfo> getAllUsers();
    QVector<LifeBalanceAI::Models::ReportData> getReportHistory(int userId);
    LifeBalanceAI::Models::ReportData getReportById(int reportId);
    bool saveReport(LifeBalanceAI::Models::ReportData &report);
    bool cleanOldData(int daysOld);
        // AI request logging
    bool logAiRequest(const QString &type, const QString &prompt,
                      const QString &response, const QString &error);

    // Password security (salt + SHA-256)
    QString hashPassword(const QString &password, const QString &salt);
    QString generateSalt();

    // Same-device auto-login sessions
    bool saveDeviceSession(int userId, const QString &deviceId, const QString &token);
    bool validateDeviceSession(int userId, const QString &deviceId, const QString &token);
    bool clearDeviceSession(int userId, const QString &deviceId);

    // Admin helpers
    bool deleteUser(int userId);
    bool resetPassword(int userId, const QString &newPassword);
    bool updateNickname(int userId, const QString &nickname);
    bool updateUserPhone(int userId, const QString &phone);

    // Delete copy/move constructors and assignment operators
    DatabaseManager(const DatabaseManager &) = delete;
    DatabaseManager &operator=(const DatabaseManager &) = delete;
    DatabaseManager(DatabaseManager &&) = delete;
    DatabaseManager &operator=(DatabaseManager &&) = delete;

private:
    DatabaseManager();
    ~DatabaseManager();

    bool createTables();

    QSqlDatabase m_database;
};

#endif // DATABASEMANAGER_H
