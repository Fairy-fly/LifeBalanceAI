#ifndef AUTHSESSIONSERVICE_H
#define AUTHSESSIONSERVICE_H

#include <QObject>
#include <QString>

#include "models/dto.h"

namespace LifeBalanceAI {
namespace Services {

class AuthSessionService : public QObject
{
    Q_OBJECT

public:
    explicit AuthSessionService(QObject *parent = nullptr);

    QString currentDeviceId() const;
    bool saveSession(int userId, const QString &role, bool remember);
    void clearSession();
    Models::AuthFlowResult tryLoadValidSession();
};

} // namespace Services
} // namespace LifeBalanceAI

#endif // AUTHSESSIONSERVICE_H
