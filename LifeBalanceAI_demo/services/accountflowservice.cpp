#include "accountflowservice.h"

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

Models::RegistrationResult failedRegistration(const QString &message)
{
    Models::RegistrationResult result;
    result.ok = false;
    result.userId = -1;
    result.nextRoute = AppRoute::Login;
    result.message = message;
    return result;
}

Models::PasswordResetResult failedPasswordReset(const QString &message, const QString &phone = QString())
{
    Models::PasswordResetResult result;
    result.ok = false;
    result.userId = -1;
    result.phone = phone;
    result.message = message;
    return result;
}

} // namespace

QString AccountFlowService::mockVerificationCode()
{
    return QStringLiteral("1234");
}

QString AccountFlowService::mockVerificationMessage()
{
    return zh("\u3010\u667a\u8861\u5065\u5eb7\u3011\u60a8\u7684\u9a8c\u8bc1\u7801\u4e3a 1234\uff0c\u8bf7\u5728 5 \u5206\u949f\u5185\u8f93\u5165\u3002");
}

bool AccountFlowService::isValidVerificationCode(const QString &code)
{
    return code.trimmed() == mockVerificationCode();
}

QString AccountFlowService::normalizeRegisterRole(const QString &role)
{
    if (role == QStringLiteral("Ascendant"))
        return QStringLiteral("Ascendant");
    return QStringLiteral("Explorer");
}

Models::RegistrationResult AccountFlowService::registerWithPassword(const Models::RegisterInput &input)
{
    const QString phone = input.phone.trimmed();
    const QString password = input.password;
    const QString verifyCode = input.verifyCode.trimmed();
    const QString role = normalizeRegisterRole(input.role);

    if (phone.isEmpty())
        return failedRegistration(zh("\u624b\u673a\u53f7\u4e0d\u80fd\u4e3a\u7a7a\uff01"));
    if (password.isEmpty())
        return failedRegistration(zh("\u5bc6\u7801\u4e0d\u80fd\u4e3a\u7a7a\uff01"));
    if (verifyCode.isEmpty())
        return failedRegistration(zh("\u8bf7\u8f93\u5165\u9a8c\u8bc1\u7801\uff01"));
    if (!isValidVerificationCode(verifyCode))
        return failedRegistration(zh("\u9a8c\u8bc1\u7801\u9519\u8bef\uff0c\u8bf7\u91cd\u65b0\u8f93\u5165\uff01"));

    const QString phoneError = UserService::validatePhone(phone);
    if (!phoneError.isEmpty())
        return failedRegistration(phoneError);

    const QString passwordError = UserService::validatePassword(password);
    if (!passwordError.isEmpty())
        return failedRegistration(passwordError);

    QSqlDatabase &db = DatabaseManager::instance().database();
    QSqlQuery query(db);
    query.prepare(QStringLiteral("SELECT id FROM Users WHERE username = :phone"));
    query.bindValue(QStringLiteral(":phone"), phone);
    if (!query.exec()) {
        return failedRegistration(zh("\u67e5\u8be2\u7528\u6237\u65f6\u51fa\u9519\uff1a%1")
                                      .arg(query.lastError().text()));
    }

    if (query.next()) {
        return failedRegistration(zh("\u8be5\u624b\u673a\u53f7\u5df2\u88ab\u6ce8\u518c\uff0c\u8bf7\u76f4\u63a5\u767b\u5f55\u6216\u4f7f\u7528\u5176\u4ed6\u53f7\u7801\uff01"));
    }

    const QString salt = DatabaseManager::instance().generateSalt();
    const QString hashedPassword = DatabaseManager::instance().hashPassword(password, salt);

    query.prepare(QStringLiteral(
        "INSERT INTO Users (username, password, salt, role) "
        "VALUES (:phone, :password, :salt, :role)"));
    query.bindValue(QStringLiteral(":phone"), phone);
    query.bindValue(QStringLiteral(":password"), hashedPassword);
    query.bindValue(QStringLiteral(":salt"), salt);
    query.bindValue(QStringLiteral(":role"), role);

    if (!query.exec()) {
        return failedRegistration(zh("\u63d2\u5165\u7528\u6237\u65f6\u51fa\u9519\uff1a%1")
                                      .arg(query.lastError().text()));
    }
    if (query.numRowsAffected() <= 0) {
        return failedRegistration(zh("\u5199\u5165\u6570\u636e\u5e93\u5931\u8d25\uff0c\u8bf7\u7a0d\u540e\u91cd\u8bd5\uff01"));
    }

    Models::RegistrationResult result;
    result.ok = true;
    result.userId = query.lastInsertId().toInt();
    result.phone = phone;
    result.role = role;
    result.nextRoute = AppRoute::Login;
    result.message = zh("\u8d26\u53f7\u6ce8\u518c\u6210\u529f\uff01\u8bf7\u8fd4\u56de\u767b\u5f55\u3002");
    return result;
}

Models::PasswordResetResult AccountFlowService::resetPasswordWithCode(const QString &phone,
                                                                      const QString &code,
                                                                      const QString &newPassword)
{
    const QString normalizedPhone = phone.trimmed();
    const QString normalizedCode = code.trimmed();

    if (normalizedPhone.isEmpty())
        return failedPasswordReset(zh("\u624b\u673a\u53f7\u4e0d\u80fd\u4e3a\u7a7a\uff01"));

    const QString phoneError = UserService::validatePhone(normalizedPhone);
    if (!phoneError.isEmpty())
        return failedPasswordReset(phoneError, normalizedPhone);

    if (normalizedCode.isEmpty())
        return failedPasswordReset(zh("\u8bf7\u8f93\u5165\u9a8c\u8bc1\u7801\uff01"), normalizedPhone);
    if (!isValidVerificationCode(normalizedCode))
        return failedPasswordReset(zh("\u9a8c\u8bc1\u7801\u9519\u8bef\uff0c\u8bf7\u91cd\u8bd5\u3002"), normalizedPhone);

    const QString passwordError = UserService::validatePassword(newPassword);
    if (!passwordError.isEmpty())
        return failedPasswordReset(passwordError, normalizedPhone);

    UserService userService;
    const int userId = userService.getUserIdByPhone(normalizedPhone);
    if (userId < 0) {
        return failedPasswordReset(zh("\u8be5\u624b\u673a\u53f7\u672a\u6ce8\u518c\uff0c\u8bf7\u5148\u6ce8\u518c\u8d26\u53f7\u3002"),
                                   normalizedPhone);
    }

    if (!userService.resetPassword(userId, newPassword)) {
        return failedPasswordReset(zh("\u91cd\u7f6e\u5bc6\u7801\u5931\u8d25\uff0c\u8bf7\u7a0d\u540e\u91cd\u8bd5\u3002"),
                                   normalizedPhone);
    }

    Models::PasswordResetResult result;
    result.ok = true;
    result.userId = userId;
    result.phone = normalizedPhone;
    result.message = zh("\u5bc6\u7801\u5df2\u91cd\u7f6e\uff0c\u8bf7\u4f7f\u7528\u65b0\u5bc6\u7801\u767b\u5f55\u3002");
    return result;
}

} // namespace Services
} // namespace LifeBalanceAI
