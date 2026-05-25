#include "userservice.h"
#include "databasemanager.h"
#include <QDebug>

namespace LifeBalanceAI {
namespace Services {

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
    QString prefs = QStringLiteral("饮食:") + data.dietPref
                  + QStringLiteral(" | 运动:") + data.sportPref
                  + QStringLiteral(" | 目标:") + data.goal;
    bool ok = DatabaseManager::instance().saveProfile(
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

bool UserService::resetPassword(int userId, const QString &newPassword)
{
    return DatabaseManager::instance().resetPassword(userId, newPassword);
}

bool UserService::updateUserPhone(int userId, const QString &phone)
{
    return DatabaseManager::instance().updateUserPhone(userId, phone);
}

// ── Validation helpers (Phase 9 will add full i18n messages) ──

QString UserService::validatePhone(const QString &input)
{
    if (input.isEmpty())
        return QStringLiteral("手机号不能为空");
    if (input.length() != 11)
        return QStringLiteral("手机号必须为11位");
    for (const QChar &ch : input) {
        if (!ch.isDigit())
            return QStringLiteral("手机号只能包含数字");
    }
    return QString();
}

QString UserService::validatePassword(const QString &input)
{
    if (input.isEmpty())
        return QStringLiteral("密码不能为空");
    if (input.length() < 6)
        return QStringLiteral("密码至少6位");
    return QString();
}

QString UserService::validateAge(const QString &input)
{
    if (input.isEmpty())
        return QStringLiteral("年龄不能为空");
    bool ok = false;
    int age = input.toInt(&ok);
    if (!ok || age < 1 || age > 150)
        return QStringLiteral("请输入有效年龄 (1-150)");
    return QString();
}

QString UserService::validateHeight(const QString &input)
{
    if (input.isEmpty())
        return QStringLiteral("身高不能为空");
    bool ok = false;
    double h = input.toDouble(&ok);
    if (!ok || h < 50.0 || h > 250.0)
        return QStringLiteral("请输入有效身高 (50-250 cm)");
    return QString();
}

QString UserService::validateWeight(const QString &input)
{
    if (input.isEmpty())
        return QStringLiteral("体重不能为空");
    bool ok = false;
    double w = input.toDouble(&ok);
    if (!ok || w < 20.0 || w > 300.0)
        return QStringLiteral("请输入有效体重 (20-300 kg)");
    return QString();
}

} // namespace Services
} // namespace LifeBalanceAI
