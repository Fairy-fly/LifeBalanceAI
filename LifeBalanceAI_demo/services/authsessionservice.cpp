#include "authsessionservice.h"

#include "databasemanager.h"
#include "services/authflowservice.h"

#include <QSettings>
#include <QUuid>

namespace LifeBalanceAI {
namespace Services {

AuthSessionService::AuthSessionService(QObject *parent)
    : QObject(parent)
{
}

QString AuthSessionService::currentDeviceId() const
{
    QSettings settings(QStringLiteral("LifeBalanceAI"), QStringLiteral("LifeBalanceAI"));
    QString deviceId = settings.value(QStringLiteral("auth/deviceId")).toString();
    if (deviceId.isEmpty()) {
        deviceId = QUuid::createUuid().toString(QUuid::WithoutBraces);
        settings.setValue(QStringLiteral("auth/deviceId"), deviceId);
    }
    return deviceId;
}

bool AuthSessionService::saveSession(int userId, const QString &role, bool remember)
{
    if (!remember || role == QStringLiteral("admin")) {
        clearSession();
        return false;
    }

    const QString deviceId = currentDeviceId();
    const QString token = QUuid::createUuid().toString(QUuid::WithoutBraces)
                        + QUuid::createUuid().toString(QUuid::WithoutBraces);
    if (!DatabaseManager::instance().saveDeviceSession(userId, deviceId, token))
        return false;

    QSettings settings(QStringLiteral("LifeBalanceAI"), QStringLiteral("LifeBalanceAI"));
    settings.setValue(QStringLiteral("auth/autoLoginEnabled"), true);
    settings.setValue(QStringLiteral("auth/userId"), userId);
    settings.setValue(QStringLiteral("auth/deviceId"), deviceId);
    settings.setValue(QStringLiteral("auth/sessionToken"), token);
    return true;
}

void AuthSessionService::clearSession()
{
    QSettings settings(QStringLiteral("LifeBalanceAI"), QStringLiteral("LifeBalanceAI"));
    const int userId = settings.value(QStringLiteral("auth/userId"), -1).toInt();
    const QString deviceId = settings.value(QStringLiteral("auth/deviceId")).toString();
    if (userId > 0)
        DatabaseManager::instance().clearDeviceSession(userId, deviceId);

    settings.remove(QStringLiteral("auth/autoLoginEnabled"));
    settings.remove(QStringLiteral("auth/userId"));
    settings.remove(QStringLiteral("auth/sessionToken"));
}

Models::AuthFlowResult AuthSessionService::tryLoadValidSession()
{
    Models::AuthFlowResult result;
    result.fromAutoLogin = true;

    QSettings settings(QStringLiteral("LifeBalanceAI"), QStringLiteral("LifeBalanceAI"));
    if (!settings.value(QStringLiteral("auth/autoLoginEnabled"), false).toBool()) {
        result.message = QStringLiteral("Auto login is disabled.");
        return result;
    }

    const int userId = settings.value(QStringLiteral("auth/userId"), -1).toInt();
    const QString deviceId = settings.value(QStringLiteral("auth/deviceId")).toString();
    const QString token = settings.value(QStringLiteral("auth/sessionToken")).toString();
    if (!DatabaseManager::instance().validateDeviceSession(userId, deviceId, token)) {
        clearSession();
        result.message = QStringLiteral("Stored session is invalid.");
        return result;
    }

    const Models::UserInfo info = DatabaseManager::instance().getUserInfo(userId);
    if (info.uid <= 0 || info.role == QStringLiteral("admin")) {
        clearSession();
        result.message = QStringLiteral("Stored session user is not eligible.");
        return result;
    }

    result.ok = true;
    result.userId = userId;
    result.phone = info.phone;
    result.role = info.role;
    result.nextRoute = AuthFlowService::resolveNextRoute(
        info.role, DatabaseManager::instance().hasProfile(userId));
    result.message = QStringLiteral("Auto login session restored.");
    return result;
}

} // namespace Services
} // namespace LifeBalanceAI
