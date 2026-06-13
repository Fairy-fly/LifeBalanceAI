#include <QtTest/QtTest>

#include "services/authflowservice.h"

Q_DECLARE_METATYPE(AppRoute)

class AuthFlowServiceTest : public QObject
{
    Q_OBJECT

private slots:
    void invalidInput_data();
    void invalidInput();
    void routeDecision_data();
    void routeDecision();
};

void AuthFlowServiceTest::invalidInput_data()
{
    QTest::addColumn<QString>("phone");
    QTest::addColumn<QString>("password");

    QTest::newRow("empty-phone") << QString() << QStringLiteral("123456");
    QTest::newRow("empty-password") << QStringLiteral("13800138000") << QString();
    QTest::newRow("invalid-phone") << QStringLiteral("1380013800a") << QStringLiteral("123456");
}

void AuthFlowServiceTest::invalidInput()
{
    QFETCH(QString, phone);
    QFETCH(QString, password);

    const auto result = LifeBalanceAI::Services::AuthFlowService::loginWithPassword(phone, password);
    QVERIFY(!result.ok);
    QVERIFY(!result.message.isEmpty());
    QCOMPARE(result.userId, -1);
    QCOMPARE(result.nextRoute, AppRoute::Login);
    QVERIFY(!result.fromAutoLogin);
}

void AuthFlowServiceTest::routeDecision_data()
{
    QTest::addColumn<QString>("role");
    QTest::addColumn<bool>("hasProfile");
    QTest::addColumn<AppRoute>("route");

    QTest::newRow("admin") << QStringLiteral("admin") << true << AppRoute::Admin;
    QTest::newRow("user-with-profile") << QStringLiteral("Ascendant") << true << AppRoute::Home;
    QTest::newRow("user-without-profile") << QStringLiteral("Explorer") << false << AppRoute::ProfileSetup;
}

void AuthFlowServiceTest::routeDecision()
{
    QFETCH(QString, role);
    QFETCH(bool, hasProfile);
    QFETCH(AppRoute, route);

    QCOMPARE(LifeBalanceAI::Services::AuthFlowService::resolveNextRoute(role, hasProfile), route);
}

QTEST_APPLESS_MAIN(AuthFlowServiceTest)

#include "test_authflowservice.moc"
