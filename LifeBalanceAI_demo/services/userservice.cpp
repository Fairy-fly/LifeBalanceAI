#include "userservice.h"

#include "databasemanager.h"
#include "services/profileflowservice.h"

#include <QSqlQuery>

namespace LifeBalanceAI {
namespace Services {

namespace {

QString zh(const char *text)
{
    return QString::fromUtf8(text);
}

} // namespace

UserService::UserService(QObject *parent)
    : QObject(parent)
{
}

Models::ProfileData UserService::loadProfile(int userId)
{
    return DatabaseManager::instance().getProfile(userId);
}

bool UserService::saveProfile(int userId, const Models::ProfileData &data)
{
    const QString prefs = ProfileFlowService::buildPreferences(
        data.dietPref, data.sportPref, data.goal);
    const bool ok = DatabaseManager::instance().saveProfile(
        userId, data.age, data.height, data.weight,
        data.gender, data.goal, data.allergy, prefs, data.nickname);
    if (ok)
        emit profileUpdated(userId);
    return ok;
}

Models::UserInfo UserService::getUserInfo(int userId)
{
    return DatabaseManager::instance().getUserInfo(userId);
}

QVector<Models::UserInfo> UserService::getAllUsers()
{
    return DatabaseManager::instance().getAllUsers();
}

bool UserService::requestUpgrade(int userId)
{
    return DatabaseManager::instance().updateUserRole(userId, QStringLiteral("Ascendant"));
}

bool UserService::deleteUser(int userId)
{
    return DatabaseManager::instance().deleteUser(userId);
}

int UserService::getUserIdByPhone(const QString &phone)
{
    DatabaseManager &db = DatabaseManager::instance();
    QSqlQuery query(db.database());
    query.prepare(QStringLiteral("SELECT id FROM Users WHERE username = :phone"));
    query.bindValue(QStringLiteral(":phone"), phone);
    if (!query.exec() || !query.next())
        return -1;
    return query.value(0).toInt();
}

bool UserService::resetPassword(int userId, const QString &newPassword)
{
    return DatabaseManager::instance().resetPassword(userId, newPassword);
}

bool UserService::updateUserPhone(int userId, const QString &phone)
{
    return DatabaseManager::instance().updateUserPhone(userId, phone);
}

QString UserService::validatePhone(const QString &input)
{
    if (input.isEmpty())
        return zh("\u624b\u673a\u53f7\u4e0d\u80fd\u4e3a\u7a7a");
    if (input.length() != 11)
        return zh("\u624b\u673a\u53f7\u5fc5\u987b\u4e3a11\u4f4d");
    for (const QChar &ch : input) {
        if (!ch.isDigit())
            return zh("\u624b\u673a\u53f7\u53ea\u80fd\u5305\u542b\u6570\u5b57");
    }
    return QString();
}

QString UserService::validatePassword(const QString &input)
{
    if (input.isEmpty())
        return zh("\u5bc6\u7801\u4e0d\u80fd\u4e3a\u7a7a");
    if (input.length() < 6)
        return zh("\u5bc6\u7801\u81f3\u5c116\u4f4d");
    return QString();
}

QString UserService::validateAge(const QString &input)
{
    if (input.isEmpty())
        return zh("\u5e74\u9f84\u4e0d\u80fd\u4e3a\u7a7a");
    bool ok = false;
    const int age = input.toInt(&ok);
    if (!ok || age < 1 || age > 150)
        return zh("\u8bf7\u8f93\u5165\u6709\u6548\u5e74\u9f84(1-150)");
    return QString();
}

QString UserService::validateHeight(const QString &input)
{
    if (input.isEmpty())
        return zh("\u8eab\u9ad8\u4e0d\u80fd\u4e3a\u7a7a");
    bool ok = false;
    const double h = input.toDouble(&ok);
    if (!ok || h < 50.0 || h > 250.0)
        return zh("\u8bf7\u8f93\u5165\u6709\u6548\u8eab\u9ad8(50-250 cm)");
    return QString();
}

QString UserService::validateWeight(const QString &input)
{
    if (input.isEmpty())
        return zh("\u4f53\u91cd\u4e0d\u80fd\u4e3a\u7a7a");
    bool ok = false;
    const double w = input.toDouble(&ok);
    if (!ok || w < 20.0 || w > 300.0)
        return zh("\u8bf7\u8f93\u5165\u6709\u6548\u4f53\u91cd(20-300 kg)");
    return QString();
}

} // namespace Services
} // namespace LifeBalanceAI
