#include <QtTest/QtTest>

#include "services/userservice.h"

class UserServiceValidationTest : public QObject
{
    Q_OBJECT

private slots:
    void phoneValidation_data();
    void phoneValidation();
    void passwordValidation_data();
    void passwordValidation();
    void ageValidation_data();
    void ageValidation();
    void heightValidation_data();
    void heightValidation();
    void weightValidation_data();
    void weightValidation();
};

void UserServiceValidationTest::phoneValidation_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<bool>("valid");

    QTest::newRow("empty") << QString() << false;
    QTest::newRow("short") << QStringLiteral("1380013800") << false;
    QTest::newRow("non-digit") << QStringLiteral("1380013800a") << false;
    QTest::newRow("valid") << QStringLiteral("13800138000") << true;
}

void UserServiceValidationTest::phoneValidation()
{
    QFETCH(QString, input);
    QFETCH(bool, valid);

    const QString error = LifeBalanceAI::Services::UserService::validatePhone(input);
    QCOMPARE(error.isEmpty(), valid);
}

void UserServiceValidationTest::passwordValidation_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<bool>("valid");

    QTest::newRow("empty") << QString() << false;
    QTest::newRow("short") << QStringLiteral("12345") << false;
    QTest::newRow("valid") << QStringLiteral("123456") << true;
}

void UserServiceValidationTest::passwordValidation()
{
    QFETCH(QString, input);
    QFETCH(bool, valid);

    const QString error = LifeBalanceAI::Services::UserService::validatePassword(input);
    QCOMPARE(error.isEmpty(), valid);
}

void UserServiceValidationTest::ageValidation_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<bool>("valid");

    QTest::newRow("empty") << QString() << false;
    QTest::newRow("non-number") << QStringLiteral("abc") << false;
    QTest::newRow("low") << QStringLiteral("0") << false;
    QTest::newRow("high") << QStringLiteral("151") << false;
    QTest::newRow("valid-low") << QStringLiteral("1") << true;
    QTest::newRow("valid-high") << QStringLiteral("150") << true;
}

void UserServiceValidationTest::ageValidation()
{
    QFETCH(QString, input);
    QFETCH(bool, valid);

    const QString error = LifeBalanceAI::Services::UserService::validateAge(input);
    QCOMPARE(error.isEmpty(), valid);
}

void UserServiceValidationTest::heightValidation_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<bool>("valid");

    QTest::newRow("empty") << QString() << false;
    QTest::newRow("non-number") << QStringLiteral("abc") << false;
    QTest::newRow("low") << QStringLiteral("49.9") << false;
    QTest::newRow("high") << QStringLiteral("250.1") << false;
    QTest::newRow("valid-low") << QStringLiteral("50") << true;
    QTest::newRow("valid-high") << QStringLiteral("250") << true;
}

void UserServiceValidationTest::heightValidation()
{
    QFETCH(QString, input);
    QFETCH(bool, valid);

    const QString error = LifeBalanceAI::Services::UserService::validateHeight(input);
    QCOMPARE(error.isEmpty(), valid);
}

void UserServiceValidationTest::weightValidation_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<bool>("valid");

    QTest::newRow("empty") << QString() << false;
    QTest::newRow("non-number") << QStringLiteral("abc") << false;
    QTest::newRow("low") << QStringLiteral("19.9") << false;
    QTest::newRow("high") << QStringLiteral("300.1") << false;
    QTest::newRow("valid-low") << QStringLiteral("20") << true;
    QTest::newRow("valid-high") << QStringLiteral("300") << true;
}

void UserServiceValidationTest::weightValidation()
{
    QFETCH(QString, input);
    QFETCH(bool, valid);

    const QString error = LifeBalanceAI::Services::UserService::validateWeight(input);
    QCOMPARE(error.isEmpty(), valid);
}

QTEST_APPLESS_MAIN(UserServiceValidationTest)

#include "test_userservice_validation.moc"
