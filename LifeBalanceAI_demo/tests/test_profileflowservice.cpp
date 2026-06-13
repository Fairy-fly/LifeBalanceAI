#include <QtTest/QtTest>

#include "services/profileflowservice.h"

Q_DECLARE_METATYPE(AppRoute)
Q_DECLARE_METATYPE(LifeBalanceAI::Models::ProfileSaveMode)

class ProfileFlowServiceTest : public QObject
{
    Q_OBJECT

private slots:
    void invalidAge_data();
    void invalidAge();
    void invalidHeight_data();
    void invalidHeight();
    void invalidWeight_data();
    void invalidWeight();
    void validProfile();
    void buildPreferences_data();
    void buildPreferences();
    void parsePreferences_data();
    void parsePreferences();
    void routeDecision_data();
    void routeDecision();
};

static LifeBalanceAI::Models::ProfileInput validInput()
{
    LifeBalanceAI::Models::ProfileInput input;
    input.nickname = QStringLiteral("tester");
    input.ageText = QStringLiteral("25");
    input.heightText = QStringLiteral("175.5");
    input.weightText = QStringLiteral("70.2");
    input.gender = QString::fromUtf8("\u7537");
    input.goal = QString::fromUtf8("\u589e\u808c");
    input.allergy = QString::fromUtf8("\u65e0");
    input.dietPref = QString::fromUtf8("\u4f4e\u7cd6");
    input.sportPref = QString::fromUtf8("\u8dd1\u6b65");
    return input;
}

void ProfileFlowServiceTest::invalidAge_data()
{
    QTest::addColumn<QString>("age");
    QTest::newRow("empty") << QString();
    QTest::newRow("non-numeric") << QStringLiteral("abc");
    QTest::newRow("zero") << QStringLiteral("0");
    QTest::newRow("too-high") << QStringLiteral("151");
}

void ProfileFlowServiceTest::invalidAge()
{
    QFETCH(QString, age);
    auto input = validInput();
    input.ageText = age;
    const auto result = LifeBalanceAI::Services::ProfileFlowService::validateProfile(input);
    QVERIFY(!result.ok);
    QVERIFY(!result.message.isEmpty());
}

void ProfileFlowServiceTest::invalidHeight_data()
{
    QTest::addColumn<QString>("height");
    QTest::newRow("empty") << QString();
    QTest::newRow("non-numeric") << QStringLiteral("abc");
    QTest::newRow("too-low") << QStringLiteral("49.9");
    QTest::newRow("too-high") << QStringLiteral("250.1");
}

void ProfileFlowServiceTest::invalidHeight()
{
    QFETCH(QString, height);
    auto input = validInput();
    input.heightText = height;
    const auto result = LifeBalanceAI::Services::ProfileFlowService::validateProfile(input);
    QVERIFY(!result.ok);
    QVERIFY(!result.message.isEmpty());
}

void ProfileFlowServiceTest::invalidWeight_data()
{
    QTest::addColumn<QString>("weight");
    QTest::newRow("empty") << QString();
    QTest::newRow("non-numeric") << QStringLiteral("abc");
    QTest::newRow("too-low") << QStringLiteral("19.9");
    QTest::newRow("too-high") << QStringLiteral("300.1");
}

void ProfileFlowServiceTest::invalidWeight()
{
    QFETCH(QString, weight);
    auto input = validInput();
    input.weightText = weight;
    const auto result = LifeBalanceAI::Services::ProfileFlowService::validateProfile(input);
    QVERIFY(!result.ok);
    QVERIFY(!result.message.isEmpty());
}

void ProfileFlowServiceTest::validProfile()
{
    const auto result = LifeBalanceAI::Services::ProfileFlowService::validateProfile(validInput());
    QVERIFY(result.ok);
    QCOMPARE(result.profile.age, 25);
    QCOMPARE(result.profile.height, 175.5);
    QCOMPARE(result.profile.weight, 70.2);
    QCOMPARE(result.profile.goal, QString::fromUtf8("\u589e\u808c"));
}

void ProfileFlowServiceTest::buildPreferences_data()
{
    QTest::addColumn<QString>("diet");
    QTest::addColumn<QString>("sport");
    QTest::addColumn<QString>("goal");
    QTest::addColumn<QString>("expected");

    QTest::newRow("all") << QString::fromUtf8("\u4f4e\u7cd6")
                         << QString::fromUtf8("\u8dd1\u6b65")
                         << QString::fromUtf8("\u589e\u808c")
                         << QString::fromUtf8("\u996e\u98df:\u4f4e\u7cd6 | \u8fd0\u52a8:\u8dd1\u6b65 | \u76ee\u6807:\u589e\u808c");
    QTest::newRow("partial") << QString()
                             << QString::fromUtf8("\u6e38\u6cf3")
                             << QString()
                             << QString::fromUtf8("\u8fd0\u52a8:\u6e38\u6cf3");
    QTest::newRow("empty") << QString() << QString() << QString() << QString();
}

void ProfileFlowServiceTest::buildPreferences()
{
    QFETCH(QString, diet);
    QFETCH(QString, sport);
    QFETCH(QString, goal);
    QFETCH(QString, expected);

    QCOMPARE(LifeBalanceAI::Services::ProfileFlowService::buildPreferences(diet, sport, goal), expected);
}

void ProfileFlowServiceTest::parsePreferences_data()
{
    QTest::addColumn<QString>("preferences");
    QTest::addColumn<QString>("diet");
    QTest::addColumn<QString>("sport");
    QTest::addColumn<QString>("goal");

    QTest::newRow("normal") << QString::fromUtf8("\u996e\u98df:\u4f4e\u7cd6 | \u8fd0\u52a8:\u8dd1\u6b65 | \u76ee\u6807:\u589e\u808c")
                            << QString::fromUtf8("\u4f4e\u7cd6")
                            << QString::fromUtf8("\u8dd1\u6b65")
                            << QString::fromUtf8("\u589e\u808c");
    QTest::newRow("legacy") << QStringLiteral("楗:legacy diet | 杩愬姩:legacy sport | 鐩爣:legacy goal")
                            << QStringLiteral("legacy diet")
                            << QStringLiteral("legacy sport")
                            << QStringLiteral("legacy goal");
    QTest::newRow("missing") << QString::fromUtf8("\u996e\u98df:\u6e05\u6de1")
                             << QString::fromUtf8("\u6e05\u6de1")
                             << QString()
                             << QString();
    QTest::newRow("empty") << QString() << QString() << QString() << QString();
}

void ProfileFlowServiceTest::parsePreferences()
{
    QFETCH(QString, preferences);
    QFETCH(QString, diet);
    QFETCH(QString, sport);
    QFETCH(QString, goal);

    const auto parts = LifeBalanceAI::Services::ProfileFlowService::parsePreferences(preferences);
    QCOMPARE(parts.dietPref, diet);
    QCOMPARE(parts.sportPref, sport);
    QCOMPARE(parts.goal, goal);
}

void ProfileFlowServiceTest::routeDecision_data()
{
    QTest::addColumn<LifeBalanceAI::Models::ProfileSaveMode>("mode");
    QTest::addColumn<AppRoute>("route");

    QTest::newRow("initial") << LifeBalanceAI::Models::ProfileSaveMode::InitialSetup << AppRoute::Home;
    QTest::newRow("edit-profile") << LifeBalanceAI::Models::ProfileSaveMode::EditFromProfile << AppRoute::Home;
    QTest::newRow("admin") << LifeBalanceAI::Models::ProfileSaveMode::AdminSetup << AppRoute::Admin;
}

void ProfileFlowServiceTest::routeDecision()
{
    QFETCH(LifeBalanceAI::Models::ProfileSaveMode, mode);
    QFETCH(AppRoute, route);

    QCOMPARE(LifeBalanceAI::Services::ProfileFlowService::routeForSaveMode(mode), route);
}

QTEST_APPLESS_MAIN(ProfileFlowServiceTest)

#include "test_profileflowservice.moc"
