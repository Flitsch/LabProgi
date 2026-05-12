#include <QtTest>
#include "../commandprocessor.h"
#include "../propertymanager.h"
#include "../realestate.h"

class TestCommandProcessor : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void testAddCommand();
    void testAddCommandInvalid();
    void testRemoveByOwner();
    void testRemoveByPrice();
    void testRemoveByDate();
    void testSaveCommand();
    void testUnknownCommand();
};

void TestCommandProcessor::initTestCase()
{
    qDebug() << "=== Запуск тестов CommandProcessor ===";
}

void TestCommandProcessor::testAddCommand()
{
    PropertyManager manager;
    CommandProcessor processor(&manager);

    CommandResult result = processor.executeCommand("ADD Иванов Иван;2023.01.15;4500000");

    QVERIFY(result.success);
    QCOMPARE(manager.getCount(), 1);
    QCOMPARE(manager.getProperty(0).getOwner(), QString("Иванов Иван"));
}

void TestCommandProcessor::testAddCommandInvalid()
{
    PropertyManager manager;
    CommandProcessor processor(&manager);

    CommandResult result = processor.executeCommand("ADD Иванов;2023.01.15");

    QVERIFY(!result.success);
    QCOMPARE(manager.getCount(), 0);
}

void TestCommandProcessor::testRemoveByOwner()
{
    PropertyManager manager;
    CommandProcessor processor(&manager);

    manager.addProperty(RealEstate("Иванов", QDate(2023, 1, 15), 1000));
    manager.addProperty(RealEstate("Петров", QDate(2023, 2, 20), 2000));
    manager.addProperty(RealEstate("Иванов", QDate(2023, 3, 10), 3000));

    CommandResult result = processor.executeCommand("REM owner = Иванов");

    QVERIFY(result.success);
    QCOMPARE(result.affectedRows, 2);
    QCOMPARE(manager.getCount(), 1);
    QCOMPARE(manager.getProperty(0).getOwner(), QString("Петров"));
}

void TestCommandProcessor::testRemoveByPrice()
{
    PropertyManager manager;
    CommandProcessor processor(&manager);

    manager.addProperty(RealEstate("Иванов", QDate(2023, 1, 15), 500));
    manager.addProperty(RealEstate("Петров", QDate(2023, 2, 20), 1500));
    manager.addProperty(RealEstate("Сидоров", QDate(2023, 3, 10), 2500));

    CommandResult result = processor.executeCommand("REM price > 1000");

    QVERIFY(result.success);
    QCOMPARE(result.affectedRows, 2);
    QCOMPARE(manager.getCount(), 1);
    QCOMPARE(manager.getProperty(0).getPrice(), 500);
}

void TestCommandProcessor::testRemoveByDate()
{
    PropertyManager manager;
    CommandProcessor processor(&manager);

    manager.addProperty(RealEstate("Иванов", QDate(2023, 1, 15), 1000));
    manager.addProperty(RealEstate("Петров", QDate(2023, 2, 20), 2000));
    manager.addProperty(RealEstate("Сидоров", QDate(2023, 3, 10), 3000));

    CommandResult result = processor.executeCommand("REM date > 2023.02.01");

    QVERIFY(result.success);
    QCOMPARE(result.affectedRows, 2);
    QCOMPARE(manager.getCount(), 1);
    QCOMPARE(manager.getProperty(0).getDate(), QDate(2023, 1, 15));
}

void TestCommandProcessor::testSaveCommand()
{
    PropertyManager manager;
    CommandProcessor processor(&manager);

    manager.addProperty(RealEstate("Тест", QDate::currentDate(), 1000));

    CommandResult result = processor.executeCommand("SAVE test_save.txt");

    QVERIFY(result.success);
    QFile::remove("test_save.txt");
}

void TestCommandProcessor::testUnknownCommand()
{
    PropertyManager manager;
    CommandProcessor processor(&manager);

    CommandResult result = processor.executeCommand("UNKNOWN command");

    QVERIFY(!result.success);
}

QTEST_MAIN(TestCommandProcessor)
#include "test_commandprocessor.moc"