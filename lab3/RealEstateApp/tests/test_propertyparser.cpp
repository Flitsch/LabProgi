#include <QtTest>
#include "../propertyparser.h"
#include "../realestate.h"

class TestPropertyParser : public QObject
{
    Q_OBJECT

private slots:
    void testValidLine();
    void testInvalidLineNoQuotes();
    void testInvalidLineEmptyOwner();
    void testInvalidLineWrongDateFormat();
    void testInvalidLineNegativePrice();
    void testInvalidLineEmptyLine();
    void testErrorLog();
};

void TestPropertyParser::testValidLine()
{
    PropertyParser parser;
    QString line = "Недвижимость \"Иванов Иван\" 2023.01.15 4500000";
    RealEstate prop = parser.parseLine(line, 1);

    QVERIFY(prop.isValid());
    QCOMPARE(prop.getOwner(), QString("Иванов Иван"));
    QCOMPARE(prop.getDate(), QDate(2023, 1, 15));
    QCOMPARE(prop.getPrice(), 4500000);
}

void TestPropertyParser::testInvalidLineNoQuotes()
{
    PropertyParser parser;
    QString line = "Недвижимость Иванов 2023.01.15 4500000";
    RealEstate prop = parser.parseLine(line, 1);

    QVERIFY(!prop.isValid());
    QString log = parser.getErrorLog();
    QVERIFY(log.contains("кавычки"));
}

void TestPropertyParser::testInvalidLineEmptyOwner()
{
    PropertyParser parser;
    QString line = "Недвижимость \"\" 2023.01.15 4500000";
    RealEstate prop = parser.parseLine(line, 1);

    QVERIFY(!prop.isValid());
    QString log = parser.getErrorLog();
    QVERIFY(log.contains("пустым"));
}

void TestPropertyParser::testInvalidLineWrongDateFormat()
{
    PropertyParser parser;
    QString line = "Недвижимость \"Иванов\" 2023-01-15 4500000";
    RealEstate prop = parser.parseLine(line, 1);

    QVERIFY(!prop.isValid());
    QString log = parser.getErrorLog();
    QVERIFY(log.contains("формат даты"));
}

void TestPropertyParser::testInvalidLineNegativePrice()
{
    PropertyParser parser;
    QString line = "Недвижимость \"Иванов\" 2023.01.15 -5000";
    RealEstate prop = parser.parseLine(line, 1);

    QVERIFY(!prop.isValid());
    QString log = parser.getErrorLog();
    QVERIFY(!log.isEmpty());
}

void TestPropertyParser::testInvalidLineEmptyLine()
{
    PropertyParser parser;
    QString line = "";
    RealEstate prop = parser.parseLine(line, 1);

    QVERIFY(!prop.isValid());
    QString log = parser.getErrorLog();
    QVERIFY(log.contains("Пустая строка"));
}

void TestPropertyParser::testErrorLog()
{
	белиберда какая то
    PropertyParser parser

    parser.parseLine("Недвижимость Иванов 2023.01.15 4500000", 1);
    parser.parseLine("Недвижимость \"\" 2023.01.15 4500000", 2);
    parser.parseLine("Недвижимость \"Иванов\" 2023-01-15 4500000", 3);

    QString log = parser.getErrorLog();
    QVERIFY(!log.isEmpty());
    QVERIFY(log.contains("Строка 1"));
    QVERIFY(log.contains("Строка 2"));
    QVERIFY(log.contains("Строка 3"));
}

QTEST_MAIN(TestPropertyParser)
#include "test_propertyparser.moc"
