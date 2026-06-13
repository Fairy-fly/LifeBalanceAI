#include "homeplanservice.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStringList>

namespace {

QString cleanAiJson(QString json)
{
    json = json.trimmed();
    if (json.startsWith(QStringLiteral("```"))) {
        json.remove(QStringLiteral("```json"));
        json.remove(QStringLiteral("```JSON"));
        json.remove(QStringLiteral("```"));
        json = json.trimmed();
    }
    return json;
}

QJsonDocument parseJsonDocument(const QString &json, QString *errorMessage)
{
    if (json.trimmed().isEmpty()) {
        if (errorMessage)
            *errorMessage = QStringLiteral("AI response is empty.");
        return QJsonDocument();
    }

    QJsonParseError error;
    const QJsonDocument doc = QJsonDocument::fromJson(cleanAiJson(json).toUtf8(), &error);
    if (error.error != QJsonParseError::NoError && errorMessage) {
        *errorMessage = QStringLiteral("Invalid JSON: %1").arg(error.errorString());
    }
    return doc;
}

QString slotTitleFor(int dayOffset, const QString &slotKey)
{
    const QString dayPrefix = dayOffset == 0
        ? QStringLiteral("今天")
        : (dayOffset == 1 ? QStringLiteral("明天") : QStringLiteral("后天"));

    QString slotChinese;
    if (slotKey == QStringLiteral("breakfast"))
        slotChinese = QStringLiteral("早餐");
    else if (slotKey == QStringLiteral("lunch"))
        slotChinese = QStringLiteral("午餐");
    else if (slotKey == QStringLiteral("dinner"))
        slotChinese = QStringLiteral("晚餐");
    else if (slotKey == QStringLiteral("sports"))
        slotChinese = QStringLiteral("运动");
    else
        slotChinese = slotKey;

    return dayPrefix + slotChinese;
}

bool appendIfContent(QVector<LifeBalanceAI::Models::PlanUpdateItem> *items,
                     const QDate &date,
                     const QString &slotKey,
                     const QJsonObject &source)
{
    const QString content = source.value(slotKey).toString().trimmed();
    if (content.isEmpty())
        return false;

    LifeBalanceAI::Models::PlanUpdateItem item;
    item.date = date;
    item.timeSlot = slotKey;
    item.content = content;
    items->append(item);
    return true;
}

} // namespace

namespace LifeBalanceAI {
namespace Services {

Models::PlanSlotRef HomePlanService::resolveSlotRef(const QString &objectName, const QDate &baseDate)
{
    Models::PlanSlotRef result;

    const bool isToday = objectName.startsWith(QStringLiteral("txtToday"));
    const bool isTomorrow = objectName.startsWith(QStringLiteral("txtTomorrow"));
    const bool isDayAfter = objectName.startsWith(QStringLiteral("txtDayAfter"));
    if (!isToday && !isTomorrow && !isDayAfter)
        return result;

    if (objectName.endsWith(QStringLiteral("Morning")))
        result.slotKey = QStringLiteral("breakfast");
    else if (objectName.endsWith(QStringLiteral("Lunch")))
        result.slotKey = QStringLiteral("lunch");
    else if (objectName.endsWith(QStringLiteral("Dinner")))
        result.slotKey = QStringLiteral("dinner");
    else if (objectName.endsWith(QStringLiteral("Exercise")))
        result.slotKey = QStringLiteral("sports");
    else
        return result;

    result.valid = true;
    result.dayOffset = isToday ? 0 : (isTomorrow ? 1 : 2);
    result.date = baseDate.addDays(result.dayOffset);
    result.slotTitle = slotTitleFor(result.dayOffset, result.slotKey);
    return result;
}

Models::PlanParseResult HomePlanService::parseGeneratedPlan(const QString &json, const QDate &baseDate)
{
    Models::PlanParseResult result;
    QString errorMessage;
    const QJsonDocument doc = parseJsonDocument(json, &errorMessage);
    if (doc.isNull() || !doc.isObject()) {
        result.errorMessage = errorMessage.isEmpty()
            ? QStringLiteral("Generated plan must be a JSON object.")
            : errorMessage;
        return result;
    }

    const QJsonObject root = doc.object();
    if (root.contains(QStringLiteral("long_term"))) {
        const QJsonObject lt = root.value(QStringLiteral("long_term")).toObject();
        QStringList parts;
        if (lt.contains(QStringLiteral("week")))
            parts << QStringLiteral("本周: %1").arg(lt.value(QStringLiteral("week")).toString());
        if (lt.contains(QStringLiteral("month")))
            parts << QStringLiteral("本月: %1").arg(lt.value(QStringLiteral("month")).toString());
        if (lt.contains(QStringLiteral("year")))
            parts << QStringLiteral("本年: %1").arg(lt.value(QStringLiteral("year")).toString());
        result.longTermText = parts.join(QStringLiteral("\n"));
    }
    if (result.longTermText.isEmpty())
        result.longTermText = QStringLiteral("（未提供长期目标）");

    const QJsonArray daily = root.value(QStringLiteral("daily")).toArray();
    if (daily.isEmpty()) {
        result.errorMessage = QStringLiteral("Generated plan contains no daily items.");
        return result;
    }

    for (const QJsonValue &value : daily) {
        const QJsonObject dayObj = value.toObject();
        const int dayIndex = dayObj.value(QStringLiteral("day")).toInt(1);
        const int normalizedDayIndex = dayIndex < 1 ? 1 : dayIndex;
        const QDate date = baseDate.addDays(normalizedDayIndex - 1);
        appendIfContent(&result.items, date, QStringLiteral("breakfast"), dayObj);
        appendIfContent(&result.items, date, QStringLiteral("lunch"), dayObj);
        appendIfContent(&result.items, date, QStringLiteral("dinner"), dayObj);
        appendIfContent(&result.items, date, QStringLiteral("sports"), dayObj);
    }

    result.ok = !result.items.isEmpty();
    if (!result.ok)
        result.errorMessage = QStringLiteral("Generated plan contains no usable slot content.");
    return result;
}

Models::PlanParseResult HomePlanService::parsePartialUpdate(const QString &json,
                                                            const QDate &baseDate,
                                                            const QString &forcedDate)
{
    Models::PlanParseResult result;
    QString errorMessage;
    const QJsonDocument doc = parseJsonDocument(json, &errorMessage);
    if (doc.isNull()) {
        result.errorMessage = errorMessage.isEmpty()
            ? QStringLiteral("Partial update response is invalid.")
            : errorMessage;
        return result;
    }

    QJsonArray updates;
    if (doc.isArray()) {
        updates = doc.array();
    } else if (doc.isObject() && doc.object().contains(QStringLiteral("daily"))) {
        updates = doc.object().value(QStringLiteral("daily")).toArray();
    } else {
        result.errorMessage = QStringLiteral("Partial update must be an array or contain daily.");
        return result;
    }

    if (updates.isEmpty()) {
        result.ok = true;
        return result;
    }

    const QDate forced = QDate::fromString(forcedDate, QStringLiteral("yyyy-MM-dd"));
    for (const QJsonValue &value : updates) {
        const QJsonObject itemObj = value.toObject();

        QDate date = forced;
        if (!date.isValid()) {
            const int dayIndex = itemObj.value(QStringLiteral("day")).toInt(0);
            if (dayIndex > 0)
                date = baseDate.addDays(dayIndex - 1);
            else
                date = QDate::fromString(itemObj.value(QStringLiteral("date")).toString(),
                                         QStringLiteral("yyyy-MM-dd"));
        }

        const QString timeSlot = itemObj.value(QStringLiteral("time_slot")).toString();
        const QString content = itemObj.value(QStringLiteral("content")).toString().trimmed();
        if (!date.isValid() || timeSlot.isEmpty() || content.isEmpty())
            continue;

        Models::PlanUpdateItem update;
        update.date = date;
        update.timeSlot = timeSlot;
        update.content = content;
        result.items.append(update);
    }

    result.ok = true;
    return result;
}

} // namespace Services
} // namespace LifeBalanceAI
