#include "airesponseparser.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>

namespace LifeBalanceAI {
namespace Services {
namespace {

QString cleanAiJson(const QString &raw)
{
    QString cleaned = raw.trimmed();
    if (cleaned.startsWith(QStringLiteral("```"))) {
        const int firstNewline = cleaned.indexOf(QLatin1Char('\n'));
        const int lastFence = cleaned.lastIndexOf(QStringLiteral("```"));
        if (firstNewline >= 0 && lastFence > firstNewline)
            cleaned = cleaned.mid(firstNewline + 1, lastFence - firstNewline - 1).trimmed();
    }

    cleaned.remove(QStringLiteral("`json"));
    cleaned.remove(QStringLiteral("```json"));
    cleaned.remove(QStringLiteral("```"));
    cleaned.remove(QStringLiteral("`"));
    return cleaned.trimmed();
}

} // namespace

Models::AiJsonParseResult AiResponseParser::parseJsonObject(const QString &raw)
{
    Models::AiJsonParseResult result;
    const QString cleaned = cleanAiJson(raw);
    if (cleaned.isEmpty()) {
        result.errorMessage = QStringLiteral("AI response is empty");
        return result;
    }

    QJsonParseError error;
    const QJsonDocument doc = QJsonDocument::fromJson(cleaned.toUtf8(), &error);
    if (error.error != QJsonParseError::NoError || !doc.isObject()) {
        result.errorMessage = error.error == QJsonParseError::NoError
            ? QStringLiteral("AI response is not a JSON object")
            : error.errorString();
        return result;
    }

    result.ok = true;
    result.object = doc.object();
    return result;
}

Models::DeepAnalysisParseResult AiResponseParser::parseDeepAnalysis(const QString &raw)
{
    Models::DeepAnalysisParseResult result;
    const auto json = parseJsonObject(raw);
    if (!json.ok) {
        result.errorMessage = json.errorMessage;
        return result;
    }

    const QJsonObject &obj = json.object;
    result.result.nutritionProtein = obj.value(QStringLiteral("nutrition_protein")).toString();
    result.result.nutritionCarbs = obj.value(QStringLiteral("nutrition_carbs")).toString();
    result.result.nutritionFat = obj.value(QStringLiteral("nutrition_fat")).toString();
    result.result.nutritionAdvice = obj.value(QStringLiteral("nutrition_advice")).toString();
    result.result.exerciseChange = obj.value(QStringLiteral("exercise_change")).toString();
    result.result.exerciseConfidence = obj.value(QStringLiteral("exercise_confidence")).toString();
    result.result.lazySlot = obj.value(QStringLiteral("lazy_slot")).toString();
    result.result.lazyAdvice = obj.value(QStringLiteral("lazy_advice")).toString();

    const QJsonArray recipes = obj.value(QStringLiteral("recipes")).toArray();
    for (const QJsonValue &value : recipes) {
        if (!value.isObject())
            continue;

        const QJsonObject recipeObject = value.toObject();
        Models::RecipeItem item;
        item.name = recipeObject.value(QStringLiteral("name")).toString();
        item.reason = recipeObject.value(QStringLiteral("reason")).toString();
        item.calories = recipeObject.value(QStringLiteral("calories")).toInt();
        result.result.recipes.append(item);
    }

    result.ok = true;
    return result;
}

Models::ReportSummaryParseResult AiResponseParser::parseReportSummary(const QString &raw)
{
    Models::ReportSummaryParseResult result;
    const auto json = parseJsonObject(raw);
    if (!json.ok) {
        result.errorMessage = json.errorMessage;
        return result;
    }

    result.ok = true;
    result.summaryObject = json.object;
    return result;
}

} // namespace Services
} // namespace LifeBalanceAI
