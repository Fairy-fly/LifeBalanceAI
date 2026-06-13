#include <QtTest/QtTest>

#include "services/accountflowservice.h"

Q_DECLARE_METATYPE(AppRoute)

class AccountFlowServiceTest : public QObject
{
    Q_OBJECT

private slots:
    void verificationCode_data();
    void verificationCode();
    void normalizeRegisterRole_data();
    void normalizeRegisterRole();
    void invalidRegistration_data();
    void invalidRegistration();
    void invalidPasswordReset_data();
    void invalidPasswordReset();
};

void AccountFlowServiceTest::verificationCode_data()
{
    QTest::addColumn<QString>("code");
    QTest::addColumn<bool>("valid");

    QTest::newRow("empty") << QString() << false;
    QTest::newRow("wrong") << QStringLiteral("4321") << false;
    QTest::newRow("correct") << QStringLiteral("1234") << true;
    QTest::newRow("trimmed") << QStringLiteral(" 1234 ") << true;
}

void AccountFlowServiceTest::verificationCode()
{
    QFETCH(QString, code);
    QFETCH(bool, valid);

    QCOMPARE(LifeBalanceAI::Services::AccountFlowService::isValidVerificationCode(code), valid);
}

void AccountFlowServiceTest::normalizeRegisterRole_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("role");

    QTest::newRow("empty") << QString() << QStringLiteral("Explorer");
    QTest::newRow("explorer") << QStringLiteral("Explorer") << QStringLiteral("Explorer");
    QTest::newRow("ascendant") << QStringLiteral("Ascendant") << QStringLiteral("Ascendant");
    QTest::newRow("unknown") << QStringLiteral("admin") << QStringLiteral("Explorer");
}

void AccountFlowServiceTest::normalizeRegisterRole()
{
    QFETCH(QString, input);
    QFETCH(QString, role);

    QCOMPARE(LifeBalanceAI::Services::AccountFlowService::normalizeRegisterRole(input), role);
}

void AccountFlowServiceTest::invalidRegistration_data()
{
    QTest::addColumn<QString>("phone");
    QTest::addColumn<QString>("password");
    QTest::addColumn<QString>("code");

    QTest::newRow("empty-phone") << QString() << QStringLiteral("123456") << QStringLiteral("1234");
    QTest::newRow("empty-password") << QStringLiteral("13800138000") << QString() << QStringLiteral("1234");
    QTest::newRow("empty-code") << QStringLiteral("13800138000") << QStringLiteral("123456") << QString();
    QTest::newRow("wrong-code") << QStringLiteral("13800138000") << QStringLiteral("123456") << QStringLiteral("0000");
    QTest::newRow("invalid-phone") << QStringLiteral("1380013800a") << QStringLiteral("123456") << QStringLiteral("1234");
    QTest::newRow("short-password") << QStringLiteral("13800138000") << QStringLiteral("12345") << QStringLiteral("1234");
}

void AccountFlowServiceTest::invalidRegistration()
{
    QFETCH(QString, phone);
    QFETCH(QString, password);
    QFETCH(QString, code);

    LifeBalanceAI::Models::RegisterInput input;
    input.phone = phone;
    input.password = password;
    input.verifyCode = code;
    input.role = QStringLiteral("Explorer");

    const auto result = LifeBalanceAI::Services::AccountFlowService::registerWithPassword(input);
    QVERIFY(!result.ok);
    QCOMPARE(result.userId, -1);
    QCOMPARE(result.nextRoute, AppRoute::Login);
    QVERIFY(!result.message.isEmpty());
}

void AccountFlowServiceTest::invalidPasswordReset_data()
{
    QTest::addColumn<QString>("phone");
    QTest::addColumn<QString>("code");
    QTest::addColumn<QString>("password");

    QTest::newRow("empty-phone") << QString() << QStringLiteral("1234") << QStringLiteral("abcdef");
    QTest::newRow("invalid-phone") << QStringLiteral("1380013800a") << QStringLiteral("1234") << QStringLiteral("abcdef");
    QTest::newRow("empty-code") << QStringLiteral("13800138000") << QString() << QStringLiteral("abcdef");
    QTest::newRow("wrong-code") << QStringLiteral("13800138000") << QStringLiteral("0000") << QStringLiteral("abcdef");
    QTest::newRow("empty-password") << QStringLiteral("13800138000") << QStringLiteral("1234") << QString();
    QTest::newRow("short-password") << QStringLiteral("13800138000") << QStringLiteral("1234") << QStringLiteral("12345");
}

void AccountFlowServiceTest::invalidPasswordReset()
{
    QFETCH(QString, phone);
    QFETCH(QString, code);
    QFETCH(QString, password);

    const auto result = LifeBalanceAI::Services::AccountFlowService::resetPasswordWithCode(
        phone, code, password);
    QVERIFY(!result.ok);
    QCOMPARE(result.userId, -1);
    QVERIFY(!result.message.isEmpty());
}

QTEST_APPLESS_MAIN(AccountFlowServiceTest)

#include "test_accountflowservice.moc"
