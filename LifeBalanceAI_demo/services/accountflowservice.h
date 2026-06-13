#ifndef ACCOUNTFLOWSERVICE_H
#define ACCOUNTFLOWSERVICE_H

#include <QString>

#include "models/dto.h"

namespace LifeBalanceAI {
namespace Services {

class AccountFlowService
{
public:
    static QString mockVerificationCode();
    static QString mockVerificationMessage();
    static bool isValidVerificationCode(const QString &code);
    static QString normalizeRegisterRole(const QString &role);

    static Models::RegistrationResult registerWithPassword(const Models::RegisterInput &input);
    static Models::PasswordResetResult resetPasswordWithCode(const QString &phone,
                                                             const QString &code,
                                                             const QString &newPassword);
};

} // namespace Services
} // namespace LifeBalanceAI

#endif // ACCOUNTFLOWSERVICE_H
