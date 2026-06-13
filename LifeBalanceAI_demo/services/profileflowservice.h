#ifndef PROFILEFLOWSERVICE_H
#define PROFILEFLOWSERVICE_H

#include "models/dto.h"

#include <QString>

namespace LifeBalanceAI {
namespace Services {

class ProfileFlowService
{
public:
    static Models::ProfileSaveResult validateProfile(const Models::ProfileInput &input);
    static Models::ProfileSaveResult saveProfile(int userId,
                                                 const Models::ProfileInput &input,
                                                 Models::ProfileSaveMode mode);

    static QString buildPreferences(const QString &dietPref,
                                    const QString &sportPref,
                                    const QString &goal);
    static Models::ProfilePreferenceParts parsePreferences(const QString &preferences);
    static AppRoute routeForSaveMode(Models::ProfileSaveMode mode);
};

} // namespace Services
} // namespace LifeBalanceAI

#endif // PROFILEFLOWSERVICE_H
