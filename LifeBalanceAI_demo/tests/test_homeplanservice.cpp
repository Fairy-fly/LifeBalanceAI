#include <QtTest/QtTest>

#include "services/homeplanservice.h"

class HomePlanServiceTest : public QObject
{
    Q_OBJECT

private slots:
    void slotRef_data();
    void slotRef();
    void unknownSlotRef();
    void generatedPlanPlainJson();
    void generatedPlanMarkdownFence();
    void partialUpdateArray();
    void partialUpdateDailyObject();
    void partialUpdateForcedDate();
    void malformedJson();
};

void HomePlanServiceTest::slotRef_data()
{
    QTest::addColumn<QString>("objectName");
    QTest::addColumn<int>("dayOffset");
    QTest::addColumn<QString>("slotKey");
    QTest::addColumn<QString>("slotTitle");

    QTest::newRow("today breakfast") << QStringLiteral("txtTodayMorning") << 0 << QStringLiteral("breakfast") << QStringLiteral("今天早餐");
    QTest::newRow("today lunch") << QStringLiteral("txtTodayLunch") << 0 << QStringLiteral("lunch") << QStringLiteral("今天午餐");
    QTest::newRow("today dinner") << QStringLiteral("txtTodayDinner") << 0 << QStringLiteral("dinner") << QStringLiteral("今天晚餐");
    QTest::newRow("today sports") << QStringLiteral("txtTodayExercise") << 0 << QStringLiteral("sports") << QStringLiteral("今天运动");
    QTest::newRow("tomorrow breakfast") << QStringLiteral("txtTomorrowMorning") << 1 << QStringLiteral("breakfast") << QStringLiteral("明天早餐");
    QTest::newRow("tomorrow lunch") << QStringLiteral("txtTomorrowLunch") << 1 << QStringLiteral("lunch") << QStringLiteral("明天午餐");
    QTest::newRow("tomorrow dinner") << QStringLiteral("txtTomorrowDinner") << 1 << QStringLiteral("dinner") << QStringLiteral("明天晚餐");
    QTest::newRow("tomorrow sports") << QStringLiteral("txtTomorrowExercise") << 1 << QStringLiteral("sports") << QStringLiteral("明天运动");
    QTest::newRow("dayafter breakfast") << QStringLiteral("txtDayAfterMorning") << 2 << QStringLiteral("breakfast") << QStringLiteral("后天早餐");
    QTest::newRow("dayafter lunch") << QStringLiteral("txtDayAfterLunch") << 2 << QStringLiteral("lunch") << QStringLiteral("后天午餐");
    QTest::newRow("dayafter dinner") << QStringLiteral("txtDayAfterDinner") << 2 << QStringLiteral("dinner") << QStringLiteral("后天晚餐");
    QTest::newRow("dayafter sports") << QStringLiteral("txtDayAfterExercise") << 2 << QStringLiteral("sports") << QStringLiteral("后天运动");
}

void HomePlanServiceTest::slotRef()
{
    QFETCH(QString, objectName);
    QFETCH(int, dayOffset);
    QFETCH(QString, slotKey);
    QFETCH(QString, slotTitle);

    const QDate baseDate(2026, 6, 9);
    const auto ref = LifeBalanceAI::Services::HomePlanService::resolveSlotRef(objectName, baseDate);
    QVERIFY(ref.valid);
    QCOMPARE(ref.dayOffset, dayOffset);
    QCOMPARE(ref.date, baseDate.addDays(dayOffset));
    QCOMPARE(ref.slotKey, slotKey);
    QCOMPARE(ref.slotTitle, slotTitle);
}

void HomePlanServiceTest::unknownSlotRef()
{
    const auto ref = LifeBalanceAI::Services::HomePlanService::resolveSlotRef(
        QStringLiteral("txtTodaySnack"),
        QDate(2026, 6, 9));
    QVERIFY(!ref.valid);
    QVERIFY(ref.slotKey.isEmpty());
    QVERIFY(!ref.date.isValid());
}

void HomePlanServiceTest::generatedPlanPlainJson()
{
    const QString json = QStringLiteral(R"({
        "long_term": {"week": "w", "month": "m", "year": "y"},
        "daily": [
            {"day": 1, "breakfast": "b1", "lunch": "l1", "dinner": "d1", "sports": "s1"},
            {"day": 2, "breakfast": "b2", "lunch": "l2", "dinner": "d2", "sports": "s2"},
            {"day": 3, "breakfast": "b3", "lunch": "l3", "dinner": "d3", "sports": "s3"}
        ]
    })");

    const QDate baseDate(2026, 6, 9);
    const auto result = LifeBalanceAI::Services::HomePlanService::parseGeneratedPlan(json, baseDate);
    QVERIFY(result.ok);
    QCOMPARE(result.items.size(), 12);
    QCOMPARE(result.longTermText, QStringLiteral("本周: w\n本月: m\n本年: y"));
    QCOMPARE(result.items.first().date, baseDate);
    QCOMPARE(result.items.first().timeSlot, QStringLiteral("breakfast"));
    QCOMPARE(result.items.first().content, QStringLiteral("b1"));
    QCOMPARE(result.items.last().date, baseDate.addDays(2));
    QCOMPARE(result.items.last().timeSlot, QStringLiteral("sports"));
    QCOMPARE(result.items.last().content, QStringLiteral("s3"));
}

void HomePlanServiceTest::generatedPlanMarkdownFence()
{
    const QString json = QStringLiteral("```json\n{\"daily\":[{\"day\":1,\"breakfast\":\"b\"}]}\n```");
    const auto result = LifeBalanceAI::Services::HomePlanService::parseGeneratedPlan(json, QDate(2026, 6, 9));
    QVERIFY(result.ok);
    QCOMPARE(result.items.size(), 1);
    QCOMPARE(result.longTermText, QStringLiteral("（未提供长期目标）"));
}

void HomePlanServiceTest::partialUpdateArray()
{
    const QString json = QStringLiteral(R"([
        {"day": 1, "time_slot": "breakfast", "content": "new b"},
        {"day": 2, "time_slot": "sports", "content": "new s"}
    ])");
    const QDate baseDate(2026, 6, 9);
    const auto result = LifeBalanceAI::Services::HomePlanService::parsePartialUpdate(json, baseDate);
    QVERIFY(result.ok);
    QCOMPARE(result.items.size(), 2);
    QCOMPARE(result.items[0].date, baseDate);
    QCOMPARE(result.items[1].date, baseDate.addDays(1));
}

void HomePlanServiceTest::partialUpdateDailyObject()
{
    const QString json = QStringLiteral(R"({
        "daily": [
            {"date": "2026-06-11", "time_slot": "lunch", "content": "new lunch"}
        ]
    })");
    const auto result = LifeBalanceAI::Services::HomePlanService::parsePartialUpdate(json, QDate(2026, 6, 9));
    QVERIFY(result.ok);
    QCOMPARE(result.items.size(), 1);
    QCOMPARE(result.items[0].date, QDate(2026, 6, 11));
    QCOMPARE(result.items[0].timeSlot, QStringLiteral("lunch"));
}

void HomePlanServiceTest::partialUpdateForcedDate()
{
    const QString json = QStringLiteral(R"([
        {"day": 1, "time_slot": "dinner", "content": "forced dinner"}
    ])");
    const auto result = LifeBalanceAI::Services::HomePlanService::parsePartialUpdate(
        json,
        QDate(2026, 6, 9),
        QStringLiteral("2026-06-12"));
    QVERIFY(result.ok);
    QCOMPARE(result.items.size(), 1);
    QCOMPARE(result.items[0].date, QDate(2026, 6, 12));
}

void HomePlanServiceTest::malformedJson()
{
    const auto result = LifeBalanceAI::Services::HomePlanService::parseGeneratedPlan(
        QStringLiteral("{not-json"),
        QDate(2026, 6, 9));
    QVERIFY(!result.ok);
    QVERIFY(!result.errorMessage.isEmpty());
}

QTEST_APPLESS_MAIN(HomePlanServiceTest)

#include "test_homeplanservice.moc"
