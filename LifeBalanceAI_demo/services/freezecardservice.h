#ifndef FREEZECARDSERVICE_H
#define FREEZECARDSERVICE_H

#include <QObject>
#include "../models/dto.h"

namespace LifeBalanceAI {
namespace Services {

class FreezeCardService : public QObject
{
    Q_OBJECT

public:
    explicit FreezeCardService(QObject *parent = nullptr);

    void processMonthlyRefill(int userId);
    Models::FreezeResult tryProtectStreak(int userId);
    void resetGrace(int userId);

signals:
    void cardsRefilled(int userId, int count);
    void streakProtected(int userId, const Models::FreezeResult &result);
    void streakLost(int userId);
};

} // namespace Services
} // namespace LifeBalanceAI

#endif // FREEZECARDSERVICE_H
