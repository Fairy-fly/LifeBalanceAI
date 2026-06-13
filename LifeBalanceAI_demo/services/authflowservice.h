#ifndef AUTHFLOWSERVICE_H
#define AUTHFLOWSERVICE_H

#include <QString>

#include "models/dto.h"

namespace LifeBalanceAI {
namespace Services {

class AuthFlowService
{
public:
    static Models::AuthFlowResult loginWithPassword(const QString &phone,
                                                    const QString &password);
    static AppRoute resolveNextRoute(const QString &role, bool hasProfile);
};

} // namespace Services
} // namespace LifeBalanceAI

#endif // AUTHFLOWSERVICE_H
