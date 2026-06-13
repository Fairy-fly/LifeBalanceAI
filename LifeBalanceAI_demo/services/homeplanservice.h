#ifndef HOMEPLANSERVICE_H
#define HOMEPLANSERVICE_H

#include <QDate>
#include <QString>

#include "models/dto.h"

namespace LifeBalanceAI {
namespace Services {

class HomePlanService
{
public:
    static Models::PlanSlotRef resolveSlotRef(const QString &objectName,
                                              const QDate &baseDate = QDate::currentDate());
    static Models::PlanParseResult parseGeneratedPlan(const QString &json,
                                                      const QDate &baseDate = QDate::currentDate());
    static Models::PlanParseResult parsePartialUpdate(const QString &json,
                                                      const QDate &baseDate = QDate::currentDate(),
                                                      const QString &forcedDate = QString());
};

} // namespace Services
} // namespace LifeBalanceAI

#endif // HOMEPLANSERVICE_H
