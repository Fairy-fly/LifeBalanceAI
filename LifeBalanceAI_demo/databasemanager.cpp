#include "databasemanager.h"
#include "services/profileflowservice.h"

#include <QCoreApplication>
#include <QDir>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QDate>
#include <QDateTime>
#include <QCryptographicHash>
#include <QRandomGenerator>
#include <QJsonDocument>
#include <QStandardPaths>

DatabaseManager &DatabaseManager::instance()
{
    static DatabaseManager inst;
    return inst;
}

DatabaseManager::DatabaseManager()
{
    // Database connection will be initialized via initDatabase()
}

DatabaseManager::~DatabaseManager()
{
    if (m_database.isOpen()) {
        m_database.close();
    }
}

bool DatabaseManager::initDatabase()
{
    // Use "lifebalance_connection" as a unique connection name for the singleton
    const QString connectionName = QStringLiteral("lifebalance_connection");

    // If the connection already exists, remove it to avoid duplicate
    if (QSqlDatabase::contains(connectionName)) {
        QSqlDatabase::removeDatabase(connectionName);
    }

    QString dbDirPath = QCoreApplication::applicationDirPath();
#ifdef Q_OS_ANDROID
    dbDirPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (dbDirPath.isEmpty())
        dbDirPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
#endif

    QDir dbDir(dbDirPath);
    if (!dbDir.exists() && !dbDir.mkpath(QStringLiteral("."))) {
        qCritical() << "Failed to create database directory:" << dbDirPath;
        return false;
    }

    const QString dbPath = dbDir.filePath(QStringLiteral("lifebalance.db"));

    // Add the database connection
    m_database = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), connectionName);
    m_database.setDatabaseName(dbPath);

    if (!m_database.open()) {
        qCritical() << "Failed to open database:" << m_database.lastError().text();
        return false;
    }

    // Enable foreign key support (needed for Profiles.user_id reference)
    QSqlQuery pragmaQuery(m_database);
    pragmaQuery.exec(QStringLiteral("PRAGMA foreign_keys = ON;"));

    // Create tables if they don't exist
    if (!createTables()) {
        qCritical() << "Failed to create database tables.";
        m_database.close();
        return false;
    }

    qInfo() << "Database initialized successfully at:" << dbPath;
    return true;
}

QSqlDatabase &DatabaseManager::database()
{
    return m_database;
}

bool DatabaseManager::createTables()
{
    QSqlQuery query(m_database);

    // ── Users table ──────────────────────────────────────────────
    const QString createUsersTable = QStringLiteral(
        "CREATE TABLE IF NOT EXISTS Users ("
        "    id          INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    username    TEXT    NOT NULL UNIQUE,"
        "    password    TEXT    NOT NULL,"
        "    salt        TEXT    NOT NULL DEFAULT '',"
        "    role        TEXT    NOT NULL DEFAULT 'Explorer',"
        "    create_time TEXT    NOT NULL DEFAULT (datetime('now','localtime'))"
        ");"
    );

    if (!query.exec(createUsersTable)) {
        qCritical() << "Failed to create Users table:" << query.lastError().text();
        return false;
    }

    // ── V3.0 migration: add streak / freeze / checkin / grace columns ──
    {
        struct Col { QString name; QString def; };
        const QList<Col> cols = {
            {QStringLiteral("streak_days"),    QStringLiteral("INTEGER DEFAULT 0")},
            {QStringLiteral("freeze_cards"),   QStringLiteral("INTEGER DEFAULT 0")},
            {QStringLiteral("total_checkins"), QStringLiteral("INTEGER DEFAULT 0")},
            {QStringLiteral("grace_days"),     QStringLiteral("INTEGER DEFAULT 0")},
            {QStringLiteral("last_streak_date"), QStringLiteral("TEXT DEFAULT ''")},
        };
        for (const auto &col : cols) {
            QString sql = QStringLiteral("ALTER TABLE Users ADD COLUMN %1 %2")
                              .arg(col.name, col.def);
            query.exec(sql); // ignore duplicate column error
        }
    }

    // ── Profiles table ──────────────────────────────────────────
    const QString createDeviceSessionsTable = QStringLiteral(
        "CREATE TABLE IF NOT EXISTS DeviceSessions ("
        "    id           INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    user_id      INTEGER NOT NULL,"
        "    device_id    TEXT    NOT NULL,"
        "    token_hash   TEXT    NOT NULL,"
        "    created_at   TEXT    NOT NULL DEFAULT (datetime('now','localtime')),"
        "    last_used_at TEXT    NOT NULL DEFAULT (datetime('now','localtime')),"
        "    expires_at   TEXT    NOT NULL,"
        "    UNIQUE(user_id, device_id),"
        "    FOREIGN KEY (user_id) REFERENCES Users(id) ON DELETE CASCADE"
        ");"
    );

    if (!query.exec(createDeviceSessionsTable)) {
        qCritical() << "Failed to create DeviceSessions table:" << query.lastError().text();
        return false;
    }

    const QString createProfilesTable = QStringLiteral(
        "CREATE TABLE IF NOT EXISTS Profiles ("
        "    id          INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    user_id     INTEGER NOT NULL UNIQUE,"
        "    nickname    TEXT DEFAULT '',"
        "    age         INTEGER,"
        "    height      REAL,"
        "    weight      REAL,"
        "    gender      TEXT,"
        "    goal        TEXT,"
        "    allergy     TEXT,"
        "    preferences TEXT,"
        "    FOREIGN KEY (user_id) REFERENCES Users(id) ON DELETE CASCADE"
        ");"
    );

    if (!query.exec(createProfilesTable)) {
        qCritical() << "Failed to create Profiles table:" << query.lastError().text();
        return false;
    }

    // ── Plans table ─────────────────────────────────────────────
    const QString createPlansTable = QStringLiteral(
        "CREATE TABLE IF NOT EXISTS Plans ("
        "    id              INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    user_id         INTEGER NOT NULL,"
        "    type            TEXT    NOT NULL DEFAULT '3-day',"
        "    long_term_goal  TEXT,"
        "    created_at      TEXT    NOT NULL DEFAULT (datetime('now','localtime')),"
        "    FOREIGN KEY (user_id) REFERENCES Users(id) ON DELETE CASCADE"
        ");"
    );

    if (!query.exec(createPlansTable)) {
        qCritical() << "Failed to create Plans table:" << query.lastError().text();
        return false;
    }

    // V3.0 migration: add status column
    query.exec(QStringLiteral(
        "ALTER TABLE Plans ADD COLUMN status TEXT DEFAULT 'active'"
    ));


    // ── Daily_Items table ───────────────────────────────────────
    const QString createDailyItemsTable = QStringLiteral(
        "CREATE TABLE IF NOT EXISTS Daily_Items ("
        "    id        INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    plan_id   INTEGER NOT NULL,"
        "    date      TEXT    NOT NULL,"
        "    time_slot TEXT    NOT NULL,"
        "    content   TEXT,"
        "    is_done   INTEGER NOT NULL DEFAULT 0,"
        "    is_adjusted INTEGER NOT NULL DEFAULT 0,"
        "    FOREIGN KEY (plan_id) REFERENCES Plans(id) ON DELETE CASCADE"
        ");"
    );

    if (!query.exec(createDailyItemsTable)) {
        qCritical() << "Failed to create Daily_Items table:" << query.lastError().text();
        return false;
    }

    // Migration: add is_adjusted column for existing databases
    {
        QSqlQuery q(m_database);
        q.exec(QStringLiteral("ALTER TABLE Daily_Items ADD COLUMN is_adjusted INTEGER NOT NULL DEFAULT 0"));
        // ignore error if column already exists
    }

    // ── Feedbacks table ──────────────────────────────────────────
    const QString createFeedbacksTable = QStringLiteral(
        "CREATE TABLE IF NOT EXISTS Feedbacks ("
        "    id         INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    user_id    INTEGER NOT NULL,"
        "    item_id    INTEGER NOT NULL,"
        "    content    TEXT,"
        "    image_path TEXT,"
        "    timestamp  DATETIME NOT NULL DEFAULT (datetime('now','localtime')),"
        "    FOREIGN KEY (user_id) REFERENCES Users(id) ON DELETE CASCADE,"
        "    FOREIGN KEY (item_id) REFERENCES Daily_Items(id) ON DELETE CASCADE"
        ");"
    );

    if (!query.exec(createFeedbacksTable)) {
        qCritical() << "Failed to create Feedbacks table:" << query.lastError().text();
        return false;
    }

    // V3.0 migration: add feedback_date / feedback_time
    {
        struct Col { QString name; QString def; };
        const QList<Col> cols = {
            {QStringLiteral("feedback_date"), QStringLiteral("TEXT DEFAULT ''")},
            {QStringLiteral("feedback_time"), QStringLiteral("TEXT DEFAULT ''")},
        };
        for (const auto &col : cols) {
            QString sql = QStringLiteral("ALTER TABLE Feedbacks ADD COLUMN %1 %2")
                              .arg(col.name, col.def);
            query.exec(sql);
        }
    }

    // Backfill feedback_date/feedback_time from timestamp
    {
        QSqlQuery backfill(m_database);
        backfill.prepare(QStringLiteral(
            "UPDATE Feedbacks SET feedback_date = date(timestamp),"
            " feedback_time = time(timestamp)"
            " WHERE feedback_date = '' OR feedback_date IS NULL"
        ));
        if (!backfill.exec()) {
            qWarning() << "Feedbacks backfill warning:" << backfill.lastError().text();
        }
    }

    // ── AiRequestLog table ─────────────────────────────────────
    const QString createAiRequestLogTable = QStringLiteral(
        "CREATE TABLE IF NOT EXISTS AiRequestLog ("
        "    LogID        INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    request_type TEXT,"
        "    prompt       TEXT,"
        "    response     TEXT,"
        "    error        TEXT,"
        "    created_at   TEXT DEFAULT (datetime('now','localtime'))"
        ");"
    );

    if (!query.exec(createAiRequestLogTable)) {
        qCritical() << "Failed to create AiRequestLog table:" << query.lastError().text();
        return false;
    }

    // ── Reports table (V3.0 new) ────────────────────────────────
    const QString createReportsTable = QStringLiteral(
        "CREATE TABLE IF NOT EXISTS Reports ("
        "    RID          INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    UID          INTEGER NOT NULL,"
        "    report_type  TEXT,"
        "    start_date   TEXT,"
        "    end_date     TEXT,"
        "    ai_summary   TEXT,"
        "    export_path  TEXT,"
        "    poster_path  TEXT,"
        "    created_at   TEXT DEFAULT (datetime('now','localtime')),"
        "    FOREIGN KEY (UID) REFERENCES Users(id) ON DELETE CASCADE"
        ");"
    );

    if (!query.exec(createReportsTable)) {
        qCritical() << "Failed to create Reports table:" << query.lastError().text();
        return false;
    }

    // ── Freeze_Cards_Log table (V3.0 new) ───────────────────────
    const QString createFreezeCardsLogTable = QStringLiteral(
        "CREATE TABLE IF NOT EXISTS Freeze_Cards_Log ("
        "    LogID     INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    UID       INTEGER NOT NULL,"
        "    used_date TEXT,"
        "    reason    TEXT,"
        "    FOREIGN KEY (UID) REFERENCES Users(id) ON DELETE CASCADE"
        ");"
    );

    if (!query.exec(createFreezeCardsLogTable)) {
        qCritical() << "Failed to create Freeze_Cards_Log table:"
                     << query.lastError().text();
        return false;
    }


    // Migration: add nickname column if missing (safe for existing DBs)
    {
        QSqlQuery q(m_database);
        q.exec(QStringLiteral("ALTER TABLE Profiles ADD COLUMN nickname TEXT DEFAULT ''"));
        // Ignore error - column may already exist
    }

    // Admin user is no longer auto-created; create manually in DB if needed

    return true;
}

// ============================================================
// Profile helpers
// ============================================================

bool DatabaseManager::hasProfile(int userId)
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral("SELECT 1 FROM Profiles WHERE user_id = :uid"));
    query.bindValue(QStringLiteral(":uid"), userId);

    if (!query.exec()) {
        qWarning() << "hasProfile query failed:" << query.lastError().text();
        return false;
    }
    return query.next();
}

bool DatabaseManager::saveProfile(int userId, int age, double height, double weight,
                                   const QString &gender, const QString &goal,
                                   const QString &allergy, const QString &preferences,
                                   const QString &nickname)
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral(
        "INSERT OR REPLACE INTO Profiles "
        "(id, user_id, nickname, age, height, weight, gender, goal, allergy, preferences) "
        "VALUES ("
        "    (SELECT id FROM Profiles WHERE user_id = :uid),"
        "    :uid, :nickname, :age, :height, :weight, :gender, :goal, :allergy, :preferences"
        ")"
    ));
    query.bindValue(QStringLiteral(":uid"),         userId);
    query.bindValue(QStringLiteral(":nickname"),    nickname);
    query.bindValue(QStringLiteral(":age"),          age);
    query.bindValue(QStringLiteral(":height"),       height);
    query.bindValue(QStringLiteral(":weight"),       weight);
    query.bindValue(QStringLiteral(":gender"),       gender);
    query.bindValue(QStringLiteral(":goal"),         goal);
    query.bindValue(QStringLiteral(":allergy"),      allergy);
    query.bindValue(QStringLiteral(":preferences"),  preferences);

    if (!query.exec()) {
        qCritical() << "[DB_FAIL] query=" << query.lastQuery() << " error=" << query.lastError().text();
        return false;
    }
    return true;
}

// ============================================================
// Plan helpers
// ============================================================

bool DatabaseManager::hasPlan(int userId)
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral("SELECT 1 FROM Plans WHERE user_id = :uid"));
    query.bindValue(QStringLiteral(":uid"), userId);

    if (!query.exec()) {
        qWarning() << "hasPlan query failed:" << query.lastError().text();
        return false;
    }
    return query.next();
}

QString DatabaseManager::getLongTermGoal(int userId)
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral(
        "SELECT long_term_goal FROM Plans WHERE user_id = :uid "
        "ORDER BY created_at DESC LIMIT 1"
    ));
    query.bindValue(QStringLiteral(":uid"), userId);

    if (!query.exec()) {
        qWarning() << "getLongTermGoal query failed:" << query.lastError().text();
        return QString();
    }

    if (query.next()) {
        return query.value(0).toString();
    }
    return QString();
}

int DatabaseManager::createPlan(int userId, const QString &type, const QString &longTermGoal)
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral(
        "INSERT INTO Plans (user_id, type, long_term_goal) VALUES (:uid, :type, :goal)"
    ));
    query.bindValue(QStringLiteral(":uid"),  userId);
    query.bindValue(QStringLiteral(":type"), type);
    query.bindValue(QStringLiteral(":goal"), longTermGoal);

    if (!query.exec()) {
        qCritical() << "[DB_FAIL] query=" << query.lastQuery() << " error=" << query.lastError().text();
        return -1;
    }

    // Return the auto-generated plan id
    return query.lastInsertId().toInt();
}

bool DatabaseManager::saveDailyItem(int planId, const QString &date,
                                     const QString &timeSlot, const QString &content)
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral(
        "INSERT INTO Daily_Items (plan_id, date, time_slot, content) "
        "VALUES (:pid, :date, :slot, :content)"
    ));
    query.bindValue(QStringLiteral(":pid"),     planId);
    query.bindValue(QStringLiteral(":date"),    date);
    query.bindValue(QStringLiteral(":slot"),    timeSlot);
    query.bindValue(QStringLiteral(":content"), content);

    if (!query.exec()) {
        qCritical() << "[DB_FAIL] query=" << query.lastQuery() << " error=" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::updateDailyItemContent(int itemId, const QString &newContent)
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral(
        "UPDATE Daily_Items SET content = :content WHERE id = :iid AND is_adjusted = 0"
    ));
    query.bindValue(QStringLiteral(":content"), newContent);
    query.bindValue(QStringLiteral(":iid"),     itemId);

    if (!query.exec()) {
        qCritical() << "[DB_FAIL] query=" << query.lastQuery() << " error=" << query.lastError().text();
        return false;
    }
    if (query.numRowsAffected() <= 0) {
        qWarning() << "updateDailyItemContent skipped: item not found or already manually adjusted. itemId=" << itemId;
        return false;
    }
    return true;
}

int DatabaseManager::getLatestPlanId(int userId)
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral(
        "SELECT id FROM Plans WHERE user_id = :uid ORDER BY created_at DESC LIMIT 1"
    ));
    query.bindValue(QStringLiteral(":uid"), userId);

    if (!query.exec()) {
        qWarning() << "getLatestPlanId query failed:" << query.lastError().text();
        return -1;
    }

    if (query.next()) {
        return query.value(0).toInt();
    }
    return -1;
}

// ============================================================
// Daily_Items helpers
// ============================================================

int DatabaseManager::getItemId(int planId, const QString &date, const QString &timeSlot)
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral(
        "SELECT id FROM Daily_Items WHERE plan_id = :pid AND date = :dt AND time_slot = :slot"
    ));
    query.bindValue(QStringLiteral(":pid"),  planId);
    query.bindValue(QStringLiteral(":dt"),   date);
    query.bindValue(QStringLiteral(":slot"), timeSlot);

    if (!query.exec()) {
        qWarning() << "getItemId failed:" << query.lastError().text();
        return -1;
    }

    if (query.next()) {
        return query.value(0).toInt();
    }
    return -1;
}

QList<DatabaseManager::DailyItemRow> DatabaseManager::getItemsForDate(int planId, const QDate &date)
{
    QList<DailyItemRow> results;
    QString dateStr = date.toString(QStringLiteral("yyyy-MM-dd"));

    QSqlQuery query(m_database);
    query.prepare(QStringLiteral(
        "SELECT id, date, time_slot, content, is_done FROM Daily_Items "
        "WHERE plan_id = :pid AND date = :dt ORDER BY id"
    ));
    query.bindValue(QStringLiteral(":pid"), planId);
    query.bindValue(QStringLiteral(":dt"),  dateStr);

    if (!query.exec()) {
        qWarning() << "getItemsForDate failed:" << query.lastError().text();
        return results;
    }

    while (query.next()) {
        DailyItemRow row;
        row.id       = query.value(0).toInt();
        row.date     = query.value(1).toString();
        row.timeSlot = query.value(2).toString();
        row.content  = query.value(3).toString();
        row.isDone   = (query.value(4).toString() == QStringLiteral("1"));
        results.append(row);
    }
    return results;
}

int DatabaseManager::countUndoneToday(int planId)
{
    QString todayStr = QDate::currentDate().toString(QStringLiteral("yyyy-MM-dd"));

    QSqlQuery query(m_database);
    query.prepare(QStringLiteral(
        "SELECT COUNT(*) FROM Daily_Items "
        "WHERE plan_id = :pid AND date = :dt AND is_done = 0"
    ));
    query.bindValue(QStringLiteral(":pid"), planId);
    query.bindValue(QStringLiteral(":dt"),  todayStr);

    if (!query.exec()) {
        qWarning() << "countUndoneToday failed:" << query.lastError().text();
        return -1;
    }

    if (query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

QJsonObject DatabaseManager::getPlanAsJson(int planId)
{
    QJsonObject planObj;

    // Get the plan's long_term_goal
    {
        QSqlQuery query(m_database);
        query.prepare(QStringLiteral(
            "SELECT long_term_goal FROM Plans WHERE id = :pid"
        ));
        query.bindValue(QStringLiteral(":pid"), planId);
        if (query.exec() && query.next()) {
            planObj[QStringLiteral("long_term_goal")] = query.value(0).toString();
        }
    }

    // Get all daily items
    QJsonArray dailyArr;
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral(
        "SELECT date, time_slot, content, is_done FROM Daily_Items "
        "WHERE plan_id = :pid ORDER BY date, id"
    ));
    query.bindValue(QStringLiteral(":pid"), planId);

    if (query.exec()) {
        while (query.next()) {
            QJsonObject item;
            item[QStringLiteral("date")]     = query.value(0).toString();
            item[QStringLiteral("time_slot")]= query.value(1).toString();
            item[QStringLiteral("content")]  = query.value(2).toString();
            item[QStringLiteral("is_done")]  = query.value(3).toString();
            dailyArr.append(item);
        }
    }

    planObj[QStringLiteral("daily")] = dailyArr;
    return planObj;
}

// ============================================================
// Feedback helpers (idempotent)
// ============================================================

QString DatabaseManager::getFeedbackContent(int itemId)
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral(
        "SELECT content FROM Feedbacks WHERE item_id = :iid ORDER BY timestamp DESC LIMIT 1"
    ));
    query.bindValue(QStringLiteral(":iid"), itemId);

    if (!query.exec()) {
        qWarning() << "getFeedbackContent failed:" << query.lastError().text();
        return QString();
    }

    if (query.next()) {
        return query.value(0).toString();
    }
    return QString();
}

bool DatabaseManager::saveFeedback(int userId, int itemId,
                                    const QString &content,
                                    const QString &imgPath)
{
    // Idempotent: check if a feedback row already exists for this item_id
    QSqlQuery checkQuery(m_database);
    checkQuery.prepare(QStringLiteral("SELECT id FROM Feedbacks WHERE item_id = :iid"));
    checkQuery.bindValue(QStringLiteral(":iid"), itemId);

    if (checkQuery.exec() && checkQuery.next()) {
        // Existing feedback �?UPDATE it
        int existingId = checkQuery.value(0).toInt();
        QSqlQuery updateQuery(m_database);
        updateQuery.prepare(QStringLiteral(
            "UPDATE Feedbacks SET"
            " content = :content,"
            " image_path = CASE WHEN :img = '' THEN image_path ELSE :img END,"
            " timestamp = datetime('now','localtime'),"
            " feedback_date = date('now','localtime'),"
            " feedback_time = time('now','localtime')"
            " WHERE id = :fid"
        ));
        updateQuery.bindValue(QStringLiteral(":content"), content);
        updateQuery.bindValue(QStringLiteral(":img"),     imgPath);
        updateQuery.bindValue(QStringLiteral(":fid"),     existingId);

        if (!updateQuery.exec()) {
            qCritical() << "[DB_FAIL] query=" << updateQuery.lastQuery() << " error=" << updateQuery.lastError().text();
            return false;
        }
        qDebug() << "saveFeedback: updated existing feedback (id=" << existingId << ")";
        return true;
    }

    // No existing feedback �?INSERT new row
    QSqlQuery insertQuery(m_database);
    insertQuery.prepare(QStringLiteral(
        "INSERT INTO Feedbacks (user_id, item_id, content, image_path, feedback_date, feedback_time) "
        "VALUES (:uid, :iid, :content, :img, date('now','localtime'), time('now','localtime'))"
    ));
    insertQuery.bindValue(QStringLiteral(":uid"),     userId);
    insertQuery.bindValue(QStringLiteral(":iid"),     itemId);
    insertQuery.bindValue(QStringLiteral(":content"), content);
    insertQuery.bindValue(QStringLiteral(":img"),     imgPath);

    if (!insertQuery.exec()) {
        qCritical() << "[DB_FAIL] query=" << insertQuery.lastQuery() << " error=" << insertQuery.lastError().text();
        return false;
    }
    qDebug() << "saveFeedback: inserted new feedback.";
    return true;
}

bool DatabaseManager::markItemDone(int itemId)
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral(
        "UPDATE Daily_Items SET is_done = 1 WHERE id = :iid"
    ));
    query.bindValue(QStringLiteral(":iid"), itemId);

    if (!query.exec()) {
        qCritical() << "[DB_FAIL] query=" << query.lastQuery() << " error=" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::markItemAdjusted(int itemId)
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral(
        "UPDATE Daily_Items SET is_adjusted = 1 WHERE id = :iid"
    ));
    query.bindValue(QStringLiteral(":iid"), itemId);
    if (!query.exec()) {
        qCritical() << "[DB_FAIL] markItemAdjusted:" << query.lastError().text();
        return false;
    }
    return true;
}

QString DatabaseManager::getUserRole(int userId)
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral(
        "SELECT role FROM Users WHERE id = :uid"
    ));
    query.bindValue(QStringLiteral(":uid"), userId);

    if (!query.exec()) {
        qWarning() << "getUserRole query failed:" << query.lastError().text();
        return QString();
    }

    if (query.next()) {
        return query.value(0).toString();
    }
    return QString();
}

bool DatabaseManager::updateUserRole(int userId, const QString &role)
{
    qDebug() << "updateUserRole: userId=" << userId << "role=" << role << "dbOpen=" << m_database.isOpen();
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral("UPDATE Users SET role = :role WHERE id = :uid"));
    query.bindValue(QStringLiteral(":role"), role);
    query.bindValue(QStringLiteral(":uid"), userId);
    if (!query.exec()) {
        qWarning() << "updateUserRole failed:" << query.lastError().text();
        return false;
    }
    qDebug() << "updateUserRole: success, rowsAffected=" << query.numRowsAffected();
    return true;
}

// ============================================================
// Yesterday helpers
// ============================================================

QString DatabaseManager::generateSalt()
{
    QByteArray saltBytes(16, Qt::Uninitialized);
    QRandomGenerator::global()->generate(saltBytes.begin(), saltBytes.end());
    return QString::fromLatin1(saltBytes.toHex());
}

QString DatabaseManager::hashPassword(const QString &password, const QString &salt)
{
    QByteArray data = (salt + password).toUtf8();
    QByteArray hash = QCryptographicHash::hash(data, QCryptographicHash::Sha256);
    return QString::fromLatin1(hash.toHex());
}

QList<DatabaseManager::DailyItemRow> DatabaseManager::getYesterdayItems(int planId)
{
    QDate yesterday = QDate::currentDate().addDays(-1);
    return getItemsForDate(planId, yesterday);
}

// ============================================================
// AI request logging
// ============================================================

bool DatabaseManager::logAiRequest(const QString &type, const QString &prompt,
                                   const QString &response, const QString &error)
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral(
        "INSERT INTO AiRequestLog (request_type, prompt, response, error) "
        "VALUES (:type, :prompt, :response, :error)"
    ));
    query.bindValue(QStringLiteral(":type"),     type);
    query.bindValue(QStringLiteral(":prompt"),   prompt);
    query.bindValue(QStringLiteral(":response"), response);
    query.bindValue(QStringLiteral(":error"),    error);

    if (!query.exec()) {
        qCritical() << "[DB_FAIL] query=" << query.lastQuery()
                     << " error=" << query.lastError().text();
        return false;
    }
    return true;
}
// ============================================================
// V3.0 CRUD helpers
// ============================================================

LifeBalanceAI::Models::UserInfo DatabaseManager::getUserInfo(int userId)
{
    LifeBalanceAI::Models::UserInfo info;
    info.uid = userId;

    QSqlQuery query(m_database);
    query.prepare(QStringLiteral(
        "SELECT username, role, streak_days, freeze_cards, grace_days,"
        " total_checkins, create_time FROM Users WHERE id = :uid"
    ));
    query.bindValue(QStringLiteral(":uid"), userId);

    if (!query.exec()) {
        qCritical() << "[DB_FAIL] query=" << query.lastQuery()
                     << " error=" << query.lastError().text();
        return info;
    }

    if (query.next()) {
        info.phone         = query.value(0).toString();
        info.role          = query.value(1).toString();
        info.streakDays    = query.value(2).toInt();
        info.freezeCards   = query.value(3).toInt();
        info.graceDays     = query.value(4).toInt();
        info.totalCheckins = query.value(5).toInt();
        info.createdAt     = query.value(6).toDate();
    }
    return info;
}

LifeBalanceAI::Models::ProfileData DatabaseManager::getProfile(int userId)
{
    LifeBalanceAI::Models::ProfileData data;
    data.uid = userId;

    QSqlQuery query(m_database);
    query.prepare(QStringLiteral(
        "SELECT nickname, age, height, weight, gender, goal, allergy, preferences"
        " FROM Profiles WHERE user_id = :uid"
    ));
    query.bindValue(QStringLiteral(":uid"), userId);

    if (!query.exec()) {
        qCritical() << "[DB_FAIL] query=" << query.lastQuery()
                     << " error=" << query.lastError().text();
        return data;
    }

    if (query.next()) {
        data.nickname = query.value(0).toString();
        data.age      = query.value(1).toInt();
        data.height   = query.value(2).toDouble();
        data.weight   = query.value(3).toDouble();
        data.gender   = query.value(4).toString();
        data.goal     = query.value(5).toString();
        data.allergy  = query.value(6).toString();

        const QString preferencesValue = query.value(7).toString();
        const auto parsedPrefs =
            LifeBalanceAI::Services::ProfileFlowService::parsePreferences(preferencesValue);
        if (!parsedPrefs.dietPref.isEmpty())
            data.dietPref = parsedPrefs.dietPref;
        if (!parsedPrefs.sportPref.isEmpty())
            data.sportPref = parsedPrefs.sportPref;
        if (!parsedPrefs.goal.isEmpty() && data.goal.isEmpty())
            data.goal = parsedPrefs.goal;

        // Parse dietPref and sportPref from preferences column
        QString prefs = query.value(7).toString();
        QStringList parts = prefs.split(QStringLiteral(" | "));
        for (const QString &part : parts) {
            if (part.startsWith(QStringLiteral("饮食:"))) {
                data.dietPref = part.mid(3);
            } else if (part.startsWith(QStringLiteral("运动:"))) {
                data.sportPref = part.mid(3);
            }
        }
    }
    return data;
}

bool DatabaseManager::setStreakDays(int userId, int days)
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral(
        "UPDATE Users SET streak_days = :d WHERE id = :uid"
    ));
    query.bindValue(QStringLiteral(":d"),   days);
    query.bindValue(QStringLiteral(":uid"), userId);

    if (!query.exec()) {
        qCritical() << "[DB_FAIL] query=" << query.lastQuery()
                     << " error=" << query.lastError().text();
        return false;
    }
    return true;
}

int DatabaseManager::getFreezeCards(int userId)
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral(
        "SELECT freeze_cards FROM Users WHERE id = :uid"
    ));
    query.bindValue(QStringLiteral(":uid"), userId);

    if (query.exec() && query.next())
        return query.value(0).toInt();
    return 0;
}

bool DatabaseManager::setFreezeCards(int userId, int count)
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral(
        "UPDATE Users SET freeze_cards = :c WHERE id = :uid"
    ));
    query.bindValue(QStringLiteral(":c"),   count);
    query.bindValue(QStringLiteral(":uid"), userId);

    if (!query.exec()) {
        qCritical() << "[DB_FAIL] query=" << query.lastQuery()
                     << " error=" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::incrementTotalCheckins(int userId)
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral(
        "UPDATE Users SET total_checkins = total_checkins + 1"
        " WHERE id = :uid"
    ));
    query.bindValue(QStringLiteral(":uid"), userId);

    if (!query.exec()) {
        qCritical() << "[DB_FAIL] query=" << query.lastQuery()
                     << " error=" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::insertFreezeCardLog(int userId, const QString &date,
                                           const QString &reason)
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral(
        "INSERT INTO Freeze_Cards_Log (UID, used_date, reason)"
        " VALUES (:uid, :date, :reason)"
    ));
    query.bindValue(QStringLiteral(":uid"),    userId);
    query.bindValue(QStringLiteral(":date"),   date);
    query.bindValue(QStringLiteral(":reason"), reason);

    if (!query.exec()) {
        qCritical() << "[DB_FAIL] query=" << query.lastQuery()
                     << " error=" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::hasCheckinOnDate(int userId, const QDate &date)
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral(
        "SELECT 1 FROM Daily_Items di"
        " JOIN Plans p ON di.plan_id = p.id"
        " WHERE p.user_id = :uid AND di.date = :date AND di.is_done = 1"
    ));
    query.bindValue(QStringLiteral(":uid"),  userId);
    query.bindValue(QStringLiteral(":date"), date.toString("yyyy-MM-dd"));

    if (query.exec() && query.next())
        return true;
    return false;
}

int DatabaseManager::countUndoneSlots(int userId, int recentDays)
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral(
        "SELECT COUNT(*) FROM Daily_Items di"
        " JOIN Plans p ON di.plan_id = p.id"
        " WHERE p.user_id = :uid AND di.is_done = 0"
        " AND di.date >= date('now', '-' || :days || ' days')"
    ));
    query.bindValue(QStringLiteral(":uid"),  userId);
    query.bindValue(QStringLiteral(":days"), recentDays);

    if (query.exec() && query.next())
        return query.value(0).toInt();
    return 0;
}

QDate DatabaseManager::getLastCheckinDate(int userId)
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral(
        "SELECT MAX(di.date) FROM Daily_Items di"
        " JOIN Plans p ON di.plan_id = p.id"
        " WHERE p.user_id = :uid AND di.is_done = 1"
    ));
    query.bindValue(QStringLiteral(":uid"), userId);

    if (query.exec() && query.next() && !query.value(0).isNull())
        return QDate::fromString(query.value(0).toString(), "yyyy-MM-dd");
    return QDate();
}

bool DatabaseManager::setGraceDays(int userId, int days)
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral(
        "UPDATE Users SET grace_days = :d WHERE id = :uid"
    ));
    query.bindValue(QStringLiteral(":d"),   days);
    query.bindValue(QStringLiteral(":uid"), userId);

    if (!query.exec()) {
        qCritical() << "[DB_FAIL] query=" << query.lastQuery()
                     << " error=" << query.lastError().text();
        return false;
    }
    return true;
}

int DatabaseManager::getGraceDays(int userId)
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral(
        "SELECT grace_days FROM Users WHERE id = :uid"
    ));
    query.bindValue(QStringLiteral(":uid"), userId);

    if (query.exec() && query.next())
        return query.value(0).toInt();
    return 0;
}

QString DatabaseManager::getLastStreakDate(int userId)
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral(
        "SELECT last_streak_date FROM Users WHERE id = :uid"
    ));
    query.bindValue(QStringLiteral(":uid"), userId);

    if (query.exec() && query.next())
        return query.value(0).toString();
    return QString();
}

bool DatabaseManager::setLastStreakDate(int userId, const QString &date)
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral(
        "UPDATE Users SET last_streak_date = :d WHERE id = :uid"
    ));
    query.bindValue(QStringLiteral(":d"),   date);
    query.bindValue(QStringLiteral(":uid"), userId);

    if (!query.exec()) {
        qCritical() << "[DB_FAIL] query=" << query.lastQuery()
                     << " error=" << query.lastError().text();
        return false;
    }
    return true;
}

QVector<LifeBalanceAI::Models::UserInfo> DatabaseManager::getAllUsers()
{
    QVector<LifeBalanceAI::Models::UserInfo> list;
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral(
        "SELECT id, username, role, streak_days, freeze_cards, grace_days,"
        " total_checkins, create_time FROM Users"
    ));

    if (!query.exec()) {
        qCritical() << "[DB_FAIL] query=" << query.lastQuery()
                     << " error=" << query.lastError().text();
        return list;
    }

    while (query.next()) {
        LifeBalanceAI::Models::UserInfo info;
        info.uid           = query.value(0).toInt();
        info.phone         = query.value(1).toString();
        info.role          = query.value(2).toString();
        info.streakDays    = query.value(3).toInt();
        info.freezeCards   = query.value(4).toInt();
        info.graceDays     = query.value(5).toInt();
        info.totalCheckins = query.value(6).toInt();
        info.createdAt     = query.value(7).toDate();
        list.append(info);
    }
    return list;
}

QVector<LifeBalanceAI::Models::ReportData> DatabaseManager::getReportHistory(int userId)
{
    QVector<LifeBalanceAI::Models::ReportData> list;
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral(
        "SELECT RID, UID, report_type, start_date, end_date, ai_summary,"
        " export_path, poster_path, created_at FROM Reports"
        " WHERE UID = :uid ORDER BY created_at DESC"
    ));
    query.bindValue(QStringLiteral(":uid"), userId);

    if (!query.exec()) {
        qCritical() << "[DB_FAIL] query=" << query.lastQuery()
                     << " error=" << query.lastError().text();
        return list;
    }

    while (query.next()) {
        LifeBalanceAI::Models::ReportData r;
        r.rid        = query.value(0).toInt();
        r.uid        = query.value(1).toInt();
        r.type       = query.value(2).toString();
        r.startDate  = query.value(3).toString();
        r.endDate    = query.value(4).toString();
        r.aiSummary  = query.value(5).toString();
        r.exportPath = query.value(6).toString();
        r.posterPath = query.value(7).toString();
        r.createdAt  = query.value(8).toString();
        list.append(r);
    }
    return list;
}


LifeBalanceAI::Models::ReportData DatabaseManager::getReportById(int reportId)
{
    LifeBalanceAI::Models::ReportData data;
    data.rid = reportId;
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral(
        "SELECT RID, UID, report_type, start_date, end_date, ai_summary,"
        " export_path, poster_path, created_at FROM Reports WHERE RID = :rid"
    ));
    query.bindValue(QStringLiteral(":rid"), reportId);
    if (!query.exec()) {
        qCritical() << "[DB_FAIL] getReportById:" << query.lastError().text();
        return data;
    }
    if (query.next()) {
        data.rid       = query.value(0).toInt();
        data.uid       = query.value(1).toInt();
        data.type      = query.value(2).toString();
        data.startDate = query.value(3).toString();
        data.endDate   = query.value(4).toString();
        data.aiSummary = query.value(5).toString();
        data.exportPath = query.value(6).toString();
        data.posterPath = query.value(7).toString();
        data.createdAt = query.value(8).toString();
    }
    return data;
}

bool DatabaseManager::saveReport(LifeBalanceAI::Models::ReportData &report)
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral(
        "INSERT INTO Reports (UID, report_type, start_date, end_date,"
        " ai_summary, export_path, poster_path) "
        "VALUES (:uid, :type, :start, :end, :summary, :export, :poster)"
    ));
    query.bindValue(QStringLiteral(":uid"),     report.uid);
    query.bindValue(QStringLiteral(":type"),    report.type);
    query.bindValue(QStringLiteral(":start"),   report.startDate);
    query.bindValue(QStringLiteral(":end"),     report.endDate);
    query.bindValue(QStringLiteral(":summary"), report.aiSummary);
    query.bindValue(QStringLiteral(":export"),  report.exportPath);
    query.bindValue(QStringLiteral(":poster"),  report.posterPath);

    if (!query.exec()) {
        qCritical() << "[DB_FAIL] query=" << query.lastQuery()
                     << " error=" << query.lastError().text();
        return false;
    }
    report.rid = query.lastInsertId().toInt();
    return true;
}

bool DatabaseManager::cleanOldData(int daysOld)
{
    QSqlQuery query(m_database);

    query.prepare(QStringLiteral(
        "DELETE FROM Daily_Items WHERE date < date('now', '-' || :d || ' days')"
    ));
    query.bindValue(QStringLiteral(":d"), daysOld);
    if (!query.exec()) {
        qCritical() << "[DB_FAIL] query=" << query.lastQuery()
                     << " error=" << query.lastError().text();
        return false;
    }

    query.prepare(QStringLiteral(
        "DELETE FROM Feedbacks WHERE feedback_date < date('now', '-' || :d || ' days')"
    ));
    query.bindValue(QStringLiteral(":d"), daysOld);
    if (!query.exec()) {
        qCritical() << "[DB_FAIL] query=" << query.lastQuery()
                     << " error=" << query.lastError().text();
        return false;
    }

    qInfo() << "cleanOldData: cleaned data older than" << daysOld << "days";
    return true;
}


// ── Admin helpers ───────────────────────────────────────────────

bool DatabaseManager::saveDeviceSession(int userId, const QString &deviceId, const QString &token)
{
    if (userId <= 0 || deviceId.isEmpty() || token.isEmpty())
        return false;

    QSqlQuery query(m_database);
    query.prepare(QStringLiteral(
        "DELETE FROM DeviceSessions WHERE user_id = :uid AND device_id = :device"
    ));
    query.bindValue(QStringLiteral(":uid"), userId);
    query.bindValue(QStringLiteral(":device"), deviceId);
    if (!query.exec()) {
        qCritical() << "[DB_FAIL] query=" << query.lastQuery()
                    << " error=" << query.lastError().text();
        return false;
    }

    const QString tokenHash = hashPassword(token, deviceId);
    const QString expiresAt = QDateTime::currentDateTime()
                                  .addDays(90)
                                  .toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"));

    query.prepare(QStringLiteral(
        "INSERT INTO DeviceSessions (user_id, device_id, token_hash, expires_at)"
        " VALUES (:uid, :device, :token_hash, :expires_at)"
    ));
    query.bindValue(QStringLiteral(":uid"), userId);
    query.bindValue(QStringLiteral(":device"), deviceId);
    query.bindValue(QStringLiteral(":token_hash"), tokenHash);
    query.bindValue(QStringLiteral(":expires_at"), expiresAt);
    if (!query.exec()) {
        qCritical() << "[DB_FAIL] query=" << query.lastQuery()
                    << " error=" << query.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::validateDeviceSession(int userId, const QString &deviceId, const QString &token)
{
    if (userId <= 0 || deviceId.isEmpty() || token.isEmpty())
        return false;

    QSqlQuery query(m_database);
    query.prepare(QStringLiteral(
        "SELECT token_hash, expires_at FROM DeviceSessions"
        " WHERE user_id = :uid AND device_id = :device"
    ));
    query.bindValue(QStringLiteral(":uid"), userId);
    query.bindValue(QStringLiteral(":device"), deviceId);
    if (!query.exec() || !query.next()) {
        if (query.lastError().isValid())
            qWarning() << "validateDeviceSession failed:" << query.lastError().text();
        return false;
    }

    const QString expectedHash = query.value(0).toString();
    const QString expiresAt = query.value(1).toString();
    if (hashPassword(token, deviceId) != expectedHash)
        return false;

    const QDateTime expiry = QDateTime::fromString(expiresAt, QStringLiteral("yyyy-MM-dd HH:mm:ss"));
    if (expiry.isValid() && expiry < QDateTime::currentDateTime())
        return false;

    query.prepare(QStringLiteral(
        "UPDATE DeviceSessions SET last_used_at = datetime('now','localtime')"
        " WHERE user_id = :uid AND device_id = :device"
    ));
    query.bindValue(QStringLiteral(":uid"), userId);
    query.bindValue(QStringLiteral(":device"), deviceId);
    query.exec();

    return true;
}

bool DatabaseManager::clearDeviceSession(int userId, const QString &deviceId)
{
    QSqlQuery query(m_database);
    if (userId > 0 && !deviceId.isEmpty()) {
        query.prepare(QStringLiteral(
            "DELETE FROM DeviceSessions WHERE user_id = :uid AND device_id = :device"
        ));
        query.bindValue(QStringLiteral(":uid"), userId);
        query.bindValue(QStringLiteral(":device"), deviceId);
    } else if (userId > 0) {
        query.prepare(QStringLiteral("DELETE FROM DeviceSessions WHERE user_id = :uid"));
        query.bindValue(QStringLiteral(":uid"), userId);
    } else {
        return true;
    }

    if (!query.exec()) {
        qCritical() << "[DB_FAIL] query=" << query.lastQuery()
                    << " error=" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::deleteUser(int userId)
{
    QSqlQuery query(m_database);
    // Delete related data first (order matters due to foreign keys)
    query.prepare(QStringLiteral("DELETE FROM Feedbacks WHERE user_id = :uid"));
    query.bindValue(QStringLiteral(":uid"), userId);
    query.exec(); // Best-effort, may not have this table or relation

    query.prepare(QStringLiteral("DELETE FROM Daily_Items WHERE plan_id IN (SELECT id FROM Plans WHERE user_id = :uid)"));
    query.bindValue(QStringLiteral(":uid"), userId);
    if (!query.exec()) {
        qCritical() << "[DB_FAIL] query=" << query.lastQuery() << " error=" << query.lastError().text();
    }

    query.prepare(QStringLiteral("DELETE FROM Plans WHERE user_id = :uid"));
    query.bindValue(QStringLiteral(":uid"), userId);
    if (!query.exec()) {
        qCritical() << "[DB_FAIL] query=" << query.lastQuery() << " error=" << query.lastError().text();
        return false;
    }

    query.prepare(QStringLiteral("DELETE FROM Reports WHERE UID = :uid"));
    query.bindValue(QStringLiteral(":uid"), userId);
    if (!query.exec()) {
        qCritical() << "[DB_FAIL] query=" << query.lastQuery() << " error=" << query.lastError().text();
        return false;
    }

    query.prepare(QStringLiteral("DELETE FROM Freeze_Cards_Log WHERE UID = :uid"));
    query.bindValue(QStringLiteral(":uid"), userId);
    if (!query.exec()) {
        qCritical() << "[DB_FAIL] query=" << query.lastQuery() << " error=" << query.lastError().text();
        return false;
    }

    query.prepare(QStringLiteral("DELETE FROM Profiles WHERE user_id = :uid"));
    query.bindValue(QStringLiteral(":uid"), userId);
    if (!query.exec()) {
        qCritical() << "[DB_FAIL] query=" << query.lastQuery() << " error=" << query.lastError().text();
        return false;
    }

    query.prepare(QStringLiteral("DELETE FROM Users WHERE id = :uid"));
    query.bindValue(QStringLiteral(":uid"), userId);
    if (!query.exec()) {
        qCritical() << "[DB_FAIL] query=" << query.lastQuery() << " error=" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::updateNickname(int userId, const QString &nickname)
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral("UPDATE Profiles SET nickname = :nk WHERE user_id = :uid"));
    query.bindValue(QStringLiteral(":nk"), nickname);
    query.bindValue(QStringLiteral(":uid"), userId);
    if (!query.exec()) {
        qCritical() << "[DB_FAIL] updateNickname:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::resetPassword(int userId, const QString &newPassword)
{
    QString salt = generateSalt();
    QString hashed = hashPassword(newPassword, salt);
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral("UPDATE Users SET password = :pw, salt = :salt WHERE id = :uid"));
    query.bindValue(QStringLiteral(":pw"), hashed);
    query.bindValue(QStringLiteral(":salt"), salt);
    query.bindValue(QStringLiteral(":uid"), userId);
    if (!query.exec()) {
        qCritical() << "[DB_FAIL] query=" << query.lastQuery() << " error=" << query.lastError().text();
        return false;
    }
    clearDeviceSession(userId, QString());
    return true;
}

bool DatabaseManager::updateUserPhone(int userId, const QString &phone)
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral("UPDATE Users SET username = :ph WHERE id = :uid"));
    query.bindValue(QStringLiteral(":ph"), phone);
    query.bindValue(QStringLiteral(":uid"), userId);
    if (!query.exec()) {
        qCritical() << "[DB_FAIL] query=" << query.lastQuery() << " error=" << query.lastError().text();
        return false;
    }
    return true;
}
