#ifndef USERSERVICE_H
#define USERSERVICE_H

#include <QObject>
#include <QVector>
#include "models/dto.h"

namespace LifeBalanceAI {
namespace Services {

class UserService : public QObject
{
    Q_OBJECT

public:
    explicit UserService(QObject *parent = nullptr);

    Models::ProfileData loadProfile(int userId);
    bool saveProfile(int userId, const Models::ProfileData &data);
    Models::UserInfo getUserInfo(int userId);
    QVector<Models::UserInfo> getAllUsers();
    bool requestUpgrade(int userId);

    // Admin operations
    bool deleteUser(int userId);
    bool resetPassword(int userId, const QString &newPassword);
    bool updateUserPhone(int userId, const QString &phone);
    int getUserIdByPhone(const QString &phone);

    // Validation helpers (implemented, returns empty on success, error msg on failure)
    static QString validatePhone(const QString &input);
    static QString validatePassword(const QString &input);
    static QString validateAge(const QString &input);
    static QString validateHeight(const QString &input);
    static QString validateWeight(const QString &input);

signals:
    void profileUpdated(int userId);
    void upgradeRequested(int userId);
};

} // namespace Services
} // namespace LifeBalanceAI

#endif // USERSERVICE_H
