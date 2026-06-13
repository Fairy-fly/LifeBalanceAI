#include <QtTest/QtTest>
#include "services/airesponseparser.h"

class AiResponseParserTest : public QObject
{
    Q_OBJECT

private slots:
    void parsesPlainJsonObject();
    void parsesMarkdownFenceJsonObject();
    void rejectsEmptyAndMalformedJson();
    void parsesDeepAnalysis();
    void parsesDeepAnalysisWithMissingFields();
    void parsesReportSummary();
    void parsesReportSummaryWithMissingFields();
};

void AiResponseParserTest::parsesPlainJsonObject()
{
    const auto result = LifeBalanceAI::Services::AiResponseParser::parseJsonObject(
        QStringLiteral("{\"summary\":\"ok\"}"));

    QVERIFY(result.ok);
    QCOMPARE(result.object.value(QStringLiteral("summary")).toString(), QStringLiteral("ok"));
}

void AiResponseParserTest::parsesMarkdownFenceJsonObject()
{
    const auto result = LifeBalanceAI::Services::AiResponseParser::parseJsonObject(
        QStringLiteral("```json\n{\"summary\":\"fenced\"}\n```"));

    QVERIFY(result.ok);
    QCOMPARE(result.object.value(QStringLiteral("summary")).toString(), QStringLiteral("fenced"));
}

void AiResponseParserTest::rejectsEmptyAndMalformedJson()
{
    const auto empty = LifeBalanceAI::Services::AiResponseParser::parseJsonObject(QString());
    QVERIFY(!empty.ok);
    QVERIFY(!empty.errorMessage.isEmpty());

    const auto malformed = LifeBalanceAI::Services::AiResponseParser::parseJsonObject(
        QStringLiteral("{\"summary\":"));
    QVERIFY(!malformed.ok);
    QVERIFY(!malformed.errorMessage.isEmpty());
}

void AiResponseParserTest::parsesDeepAnalysis()
{
    const QString raw = QStringLiteral(
        "{"
        "\"nutrition_protein\":\"protein\","
        "\"nutrition_carbs\":\"carbs\","
        "\"nutrition_fat\":\"fat\","
        "\"nutrition_advice\":\"advice\","
        "\"exercise_change\":\"change\","
        "\"exercise_confidence\":\"confidence\","
        "\"recipes\":[{\"name\":\"meal\",\"reason\":\"reason\",\"calories\":320}],"
        "\"lazy_slot\":\"dinner\","
        "\"lazy_advice\":\"prepare\""
        "}");

    const auto result = LifeBalanceAI::Services::AiResponseParser::parseDeepAnalysis(raw);

    QVERIFY(result.ok);
    QCOMPARE(result.result.nutritionAdvice, QStringLiteral("advice"));
    QCOMPARE(result.result.exerciseChange, QStringLiteral("change"));
    QCOMPARE(result.result.recipes.size(), 1);
    QCOMPARE(result.result.recipes.first().name, QStringLiteral("meal"));
    QCOMPARE(result.result.recipes.first().calories, 320);
    QCOMPARE(result.result.lazySlot, QStringLiteral("dinner"));
}

void AiResponseParserTest::parsesDeepAnalysisWithMissingFields()
{
    const auto result = LifeBalanceAI::Services::AiResponseParser::parseDeepAnalysis(
        QStringLiteral("{\"nutrition_advice\":\"only advice\"}"));

    QVERIFY(result.ok);
    QCOMPARE(result.result.nutritionAdvice, QStringLiteral("only advice"));
    QVERIFY(result.result.recipes.isEmpty());
    QVERIFY(result.result.exerciseChange.isEmpty());
}

void AiResponseParserTest::parsesReportSummary()
{
    const auto result = LifeBalanceAI::Services::AiResponseParser::parseReportSummary(
        QStringLiteral("{\"summary\":\"good\",\"completion_rate\":80,\"next_week_goal\":\"walk\"}"));

    QVERIFY(result.ok);
    QCOMPARE(result.summaryObject.value(QStringLiteral("summary")).toString(), QStringLiteral("good"));
    QCOMPARE(result.summaryObject.value(QStringLiteral("completion_rate")).toInt(), 80);
}

void AiResponseParserTest::parsesReportSummaryWithMissingFields()
{
    const auto result = LifeBalanceAI::Services::AiResponseParser::parseReportSummary(
        QStringLiteral("{\"summary\":\"partial\"}"));

    QVERIFY(result.ok);
    QCOMPARE(result.summaryObject.value(QStringLiteral("summary")).toString(), QStringLiteral("partial"));
    QVERIFY(result.summaryObject.value(QStringLiteral("next_week_goal")).toString().isEmpty());
}

QTEST_APPLESS_MAIN(AiResponseParserTest)

#include "test_airesponseparser.moc"
