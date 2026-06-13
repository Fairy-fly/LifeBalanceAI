#include "profileflowservice.h"

#include "databasemanager.h"
#include "services/userservice.h"

#include <QStringList>

namespace LifeBalanceAI {
namespace Services {

namespace {

QString zh(const char *text)
{
    return QString::fromUtf8(text);
}

QString normalizedNumberText(const QString &text)
{
    return text.trimmed();
}

Models::ProfileSaveResult failedProfileResult(const QString &message)
{
    Models::ProfileSaveResult result;
    result.ok = false;
    result.message = message;
    result.nextRoute = AppRoute::ProfileSetup;
    return result;
}

void setPart(Models::ProfilePreferenceParts &parts, const QString &label, const QString &value)
{
    if (label == zh("\u996e\u98df") || label == QStringLiteral("楗"))
        parts.dietPref = value;
    else if (label == zh("\u8fd0\u52a8") || label == QStringLiteral("杩愬姩"))
        parts.sportPref = value;
    else if (label == zh("\u76ee\u6807") || label == QStringLiteral("鐩爣"))
        parts.goal = value;
}

} // namespace

Models::ProfileSaveResult ProfileFlowService::validateProfile(const Models::ProfileInput &input)
{
    Models::ProfileSaveResult result;

    const QString ageText = normalizedNumberText(input.ageText);
    const QString heightText = normalizedNumberText(input.heightText);
    const QString weightText = normalizedNumberText(input.weightText);

    QString error = UserService::validateAge(ageText);
    if (!error.isEmpty())
        return failedProfileResult(error);

    error = UserService::validateHeight(heightText);
    if (!error.isEmpty())
        return failedProfileResult(error);

    error = UserService::validateWeight(weightText);
    if (!error.isEmpty())
        return failedProfileResult(error);

    result.ok = true;
    result.profile.nickname = input.nickname.trimmed();
    result.profile.age = ageText.toInt();
    result.profile.height = heightText.toDouble();
    result.profile.weight = weightText.toDouble();
    result.profile.gender = input.gender.trimmed();
    result.profile.goal = input.goal.trimmed();
    result.profile.allergy = input.allergy.trimmed();
    result.profile.dietPref = input.dietPref.trimmed();
    result.profile.sportPref = input.sportPref.trimmed();
    result.requestNickname = result.profile.nickname.isEmpty();
    return result;
}

Models::ProfileSaveResult ProfileFlowService::saveProfile(int userId,
                                                          const Models::ProfileInput &input,
                                                          Models::ProfileSaveMode mode)
{
    if (userId <= 0)
        return failedProfileResult(zh("\u672a\u68c0\u6d4b\u5230\u767b\u5f55\u7528\u6237\uff0c\u8bf7\u91cd\u65b0\u767b\u5f55\uff01"));

    Models::ProfileSaveResult result = validateProfile(input);
    if (!result.ok)
        return result;

    const QString preferences = buildPreferences(
        result.profile.dietPref, result.profile.sportPref, result.profile.goal);

    const bool saved = DatabaseManager::instance().saveProfile(
        userId,
        result.profile.age,
        result.profile.height,
        result.profile.weight,
        result.profile.gender,
        result.profile.goal,
        result.profile.allergy,
        preferences,
        result.profile.nickname);
    if (!saved)
        return failedProfileResult(zh("\u6570\u636e\u5e93\u5199\u5165\u5f02\u5e38\uff0c\u8bf7\u7a0d\u540e\u91cd\u8bd5\uff01"));

    result.ok = true;
    result.profile.uid = userId;
    result.nextRoute = routeForSaveMode(mode);
    result.needsInitialPlan = !DatabaseManager::instance().hasPlan(userId);
    result.requestNickname = result.profile.nickname.isEmpty();
    result.message = mode == Models::ProfileSaveMode::InitialSetup
        ? zh("\u753b\u50cf\u5df2\u5efa\u7acb\uff01")
        : zh("\u753b\u50cf\u5df2\u66f4\u65b0\uff01");
    return result;
}

QString ProfileFlowService::buildPreferences(const QString &dietPref,
                                             const QString &sportPref,
                                             const QString &goal)
{
    QStringList parts;
    const QString diet = dietPref.trimmed();
    const QString sport = sportPref.trimmed();
    const QString target = goal.trimmed();
    if (!diet.isEmpty())
        parts << zh("\u996e\u98df:") + diet;
    if (!sport.isEmpty())
        parts << zh("\u8fd0\u52a8:") + sport;
    if (!target.isEmpty())
        parts << zh("\u76ee\u6807:") + target;
    return parts.join(QStringLiteral(" | "));
}

Models::ProfilePreferenceParts ProfileFlowService::parsePreferences(const QString &preferences)
{
    Models::ProfilePreferenceParts parts;
    const QStringList rawParts = preferences.split(QStringLiteral(" | "), Qt::SkipEmptyParts);
    for (const QString &rawPart : rawParts) {
        const int colonIndex = rawPart.indexOf(QLatin1Char(':'));
        if (colonIndex <= 0)
            continue;
        const QString label = rawPart.left(colonIndex).trimmed();
        const QString value = rawPart.mid(colonIndex + 1).trimmed();
        setPart(parts, label, value);
    }
    return parts;
}

AppRoute ProfileFlowService::routeForSaveMode(Models::ProfileSaveMode mode)
{
    switch (mode) {
    case Models::ProfileSaveMode::AdminSetup:
        return AppRoute::Admin;
    case Models::ProfileSaveMode::EditFromProfile:
    case Models::ProfileSaveMode::InitialSetup:
        return AppRoute::Home;
    }
    return AppRoute::Home;
}

} // namespace Services
} // namespace LifeBalanceAI
