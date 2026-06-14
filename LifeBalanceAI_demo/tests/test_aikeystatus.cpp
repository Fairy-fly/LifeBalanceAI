#include <QtTest/QtTest>

#include "aimanager.h"

#include <QFile>
#include <QTemporaryDir>
#include <QTextStream>

class AiKeyStatusTest : public QObject
{
    Q_OBJECT

private slots:
    void envFileParsing_data();
    void envFileParsing();
    void statusDoesNotExposeSecret();

private:
    QString writeEnvFile(QTemporaryDir &dir, const QString &content) const;
};

QString AiKeyStatusTest::writeEnvFile(QTemporaryDir &dir, const QString &content) const
{
    const QString path = dir.filePath(QStringLiteral(".env"));
    QFile file(path);
    if (!file.open(QFile::WriteOnly | QFile::Text))
        return QString();
    QTextStream stream(&file);
    stream << content;
    file.close();
    return path;
}

void AiKeyStatusTest::envFileParsing_data()
{
    QTest::addColumn<QString>("content");
    QTest::addColumn<bool>("configured");

    QTest::newRow("empty-file") << QString() << false;
    QTest::newRow("missing-key") << QStringLiteral("OPENAI_API_KEY=abc\n") << false;
    QTest::newRow("empty-key") << QStringLiteral("DEEPSEEK_API_KEY=\n") << false;
    QTest::newRow("plain-key") << QStringLiteral("DEEPSEEK_API_KEY=sk-test-value\n") << true;
    QTest::newRow("double-quoted-key") << QStringLiteral("DEEPSEEK_API_KEY=\"sk-test-value\"\n") << true;
    QTest::newRow("single-quoted-key") << QStringLiteral("DEEPSEEK_API_KEY='sk-test-value'\n") << true;
}

void AiKeyStatusTest::envFileParsing()
{
    QFETCH(QString, content);
    QFETCH(bool, configured);

    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString envPath = writeEnvFile(dir, content);
    QVERIFY(!envPath.isEmpty());

    const AiKeyStatus status = AIManager::chatApiKeyStatus({envPath}, false, false);
    QCOMPARE(status.configured, configured);
    QCOMPARE(status.keyName, QStringLiteral("DEEPSEEK_API_KEY"));
    QVERIFY(status.checkedPaths.contains(envPath));
    QVERIFY(!status.message.isEmpty());
}

void AiKeyStatusTest::statusDoesNotExposeSecret()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString secret = QStringLiteral("sk-do-not-print-this");
    const QString envPath = writeEnvFile(
        dir, QStringLiteral("DEEPSEEK_API_KEY=%1\n").arg(secret));
    QVERIFY(!envPath.isEmpty());

    const AiKeyStatus status = AIManager::chatApiKeyStatus({envPath}, false, false);
    QVERIFY(status.configured);
    QVERIFY(!status.source.contains(secret));
    QVERIFY(!status.message.contains(secret));
    QVERIFY(!status.checkedPaths.join(QString()).contains(secret));
}

QTEST_APPLESS_MAIN(AiKeyStatusTest)

#include "test_aikeystatus.moc"
