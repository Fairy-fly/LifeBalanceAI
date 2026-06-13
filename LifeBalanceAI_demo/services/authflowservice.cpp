#include "authflowservice.h"

#include "databasemanager.h"
#include "services/userservice.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

namespace LifeBalanceAI {
namespace Services {

namespace {

QString zh(const char *text)
{
    return QString::fromUtf8(text);
}

Models::AuthFlowResult failedAuthResult(const QString &message)
{
    Models::AuthFlowResult result;
    result.ok = false;
    result.userId = -1;
    result.nextRoute = AppRoute::Login;
    result.message = message;
    result.fromAutoLogin = false;
    return result;
}

} // namespace

Models::AuthFlowResult AuthFlowService::loginWithPassword(const QString &phone,
                                                          const QString &password)
{
    const QString normalizedPhone = phone.trimmed();
    if (normalizedPhone.isEmpty())
        return failedAuthResult(zh("\u8bf7\u8f93\u5165\u624b\u673a\u53f7\uff01"));
    if (password.isEmpty())
        return failedAuthResult(zh("\u8bf7\u8f93\u5165\u5bc6\u7801\uff01"));

    const QString phoneError = UserService::validatePhone(normalizedPhone);
    if (!phoneError.isEmpty())
        return failedAuthResult(phoneError);

    const QString passwordError = UserService::validatePassword(password);
    if (!passwordError.isEmpty())
        return failedAuthResult(passwordError);

    QSqlDatabase &db = DatabaseManager::instance().database();
    QSqlQuery query(db);
    query.prepare(QStringLiteral(
        "SELECT id, role, password, salt FROM Users WHERE username = :phone"));
    query.bindValue(QStringLiteral(":phone"), normalizedPhone);

    if (!query.exec()) {
        return failedAuthResult(zh("\u67e5\u8be2\u7528\u6237\u65f6\u51fa\u9519\uff1a%1")
                                    .arg(query.lastError().text()));
    }

    if (!query.next()) {
        return failedAuthResult(zh(
            "\u624b\u673a\u53f7\u6216\u5bc6\u7801\u9519\u8bef\uff0c\u8bf7\u91cd\u8bd5\uff01"));
    }

    const int userId = query.value(0).toInt();
    const QString role = query.value(1).toString();
    const QString storedHash = query.value(2).toString();
    const QString salt = query.value(3).toString();

    if (DatabaseManager::instance().hashPassword(password, salt) != storedHash) {
        return failedAuthResult(zh(
            "\u624b\u673a\u53f7\u6216\u5bc6\u7801\u9519\u8bef\uff0c\u8bf7\u91cd\u8bd5\u3002"));
    }

    Models::AuthFlowResult result;
    result.ok = true;
    result.userId = userId;
    result.phone = normalizedPhone;
    result.role = role;
    result.nextRoute = resolveNextRoute(role, DatabaseManager::instance().hasProfile(userId));
    result.fromAutoLogin = false;
    result.message = QStringLiteral("Login succeeded.");
    return result;
}

AppRoute AuthFlowService::resolveNextRoute(const QString &role, bool hasProfile)
{
    if (role == QStringLiteral("admin"))
        return AppRoute::Admin;
    return hasProfile ? AppRoute::Home : AppRoute::ProfileSetup;
}

} // namespace Services
} // namespace LifeBalanceAI
