#ifndef CHECKINSERVICE_H
#define CHECKINSERVICE_H

#include <QObject>
#include "models/dto.h"

namespace LifeBalanceAI {
namespace Services {

class FreezeCardService;

class CheckinService : public QObject
{
    Q_OBJECT

public:
    explicit CheckinService(QObject *parent = nullptr);

    Models::CheckinResult processDailyCheckin(int userId);
    Models::StreakInfo getStreakInfo(int userId);
    bool shouldForceReprofile(int userId);

    void setFreezeCardService(FreezeCardService *svc);

private:
    FreezeCardService *m_freezeCardService = nullptr;

signals:
    void checkinCompleted(int userId, const Models::CheckinResult &result);
    void reprofileRequired(int userId);

};

} // namespace Services
} // namespace LifeBalanceAI

#endif // CHECKINSERVICE_H
