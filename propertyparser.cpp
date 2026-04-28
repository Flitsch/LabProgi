#include "propertyparser.h"
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QDebug>
#include <QDateTime>

PropertyParser::PropertyParser() {}

RealEstate PropertyParser::parseLine(const QString& line, int lineNumber)
{
    if (line.trimmed().isEmpty()) {
        addError(lineNumber, line, "Пустая строка");
        return RealEstate();
    }

    // Проверяем наличие кавычек
    int firstQuote = line.indexOf('"');
    int secondQuote = line.indexOf('"', firstQuote + 1);

    if (firstQuote == -1 || secondQuote == -1) {
        addError(lineNumber, line, "Отсутствуют кавычки вокруг имени владельца");
        return RealEstate();
    }

    try {
        QString owner = extractOwner(line);
        QDate date = extractDate(line);
        int price = extractPrice(line);

        return RealEstate(owner, date, price);

    } catch (const RealEstateException& e) {
        addError(lineNumber, line, e.getMessage());
        return RealEstate();
    }
}

QString PropertyParser::extractOwner(const QString& line)
{
    int firstQuote = line.indexOf('"');
    int secondQuote = line.indexOf('"', firstQuote + 1);

    QString owner = line.mid(firstQuote + 1, secondQuote - firstQuote - 1);

    if (owner.isEmpty()) {
        throw RealEstateException("Имя владельца не может быть пустым");
    }

    return owner;
}

QDate PropertyParser::extractDate(const QString& line)
{
    int secondQuote = line.indexOf('"', line.indexOf('"') + 1);
    QString remaining = line.mid(secondQuote + 1).trimmed();

    QRegularExpression dateRegex(R"(^(\d{4})\.(\d{2})\.(\d{2}))");
    QRegularExpressionMatch match = dateRegex.match(remaining);

    if (!match.hasMatch()) {
        throw RealEstateException("Неверный формат даты. Ожидается: ГГГГ.ММ.ДД");
    }

    int year = match.captured(1).toInt();
    int month = match.captured(2).toInt();
    int day = match.captured(3).toInt();

    QDate date(year, month, day);

    if (!date.isValid()) {
        throw RealEstateException("Неверная дата: " + QString::number(year) + "." +
                                  QString::number(month) + "." + QString::number(day));
    }

    return date;
}

int PropertyParser::extractPrice(const QString& line)
{
    int secondQuote = line.indexOf('"', line.indexOf('"') + 1);
    if (secondQuote == -1) {
        throw RealEstateException("Не найдена закрывающая кавычка");
    }

    QString remaining = line.mid(secondQuote + 1).trimmed();

    QRegularExpression priceRegex(R"((-?\d+)\s*$)");
    QRegularExpressionMatch match = priceRegex.match(remaining);

    if (!match.hasMatch()) {
        throw RealEstateException("Не указана стоимость или неверный формат числа");
    }

    bool ok;
    int price = match.captured(1).toInt(&ok);

    if (!ok || price < 0) {
        throw RealEstateException("Стоимость не может быть отрицательной");
    }

    return price;
}

QVector<RealEstate> PropertyParser::parseFile(const QString& filename, QVector<ParseError>& errors)
{
    errors_.clear();
    QVector<RealEstate> properties;

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        addError(-1, "", "Не удалось открыть файл: " + filename);
        errors = errors_;
        return properties;
    }

    QTextStream in(&file);
    int lineNumber = 1;

    while (!in.atEnd()) {
        QString line = in.readLine();
        RealEstate prop = parseLine(line, lineNumber);

        if (prop.isValid()) {
            properties.push_back(prop);
        }

        lineNumber++;
    }

    file.close();
    errors = errors_;

    // Сохраняем лог в файл, если есть ошибки
    if (!errors_.isEmpty()) {
        saveLogToFile();
    }

    return properties;
}

void PropertyParser::addError(int lineNumber, const QString& line, const QString& message)
{
    ParseError error;
    error.lineNumber = lineNumber;
    error.line = line;
    error.errorMessage = message;
    errors_.push_back(error);
}

QString PropertyParser::getErrorLog() const
{
    QString log;
    for (const auto& error : errors_) {
        log += QString("Строка %1: %2\n").arg(error.lineNumber).arg(error.errorMessage);
        if (!error.line.isEmpty()) {
            log += QString("  Содержимое: %1\n").arg(error.line);
        }
    }
    return log;
}

void PropertyParser::saveLogToFile(const QString& filename) const
{
    if (errors_.isEmpty()) {
        return;
    }

    QFile file(filename);
    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        qDebug() << "Не удалось открыть файл лога:" << filename;
        return;
    }

    QTextStream out(&file);
    out << "\n========================================\n";
    out << QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss") << "\n";
    out << "========================================\n";

    for (const auto& error : errors_) {
        out << QString("Строка %1: %2\n").arg(error.lineNumber).arg(error.errorMessage);
        if (!error.line.isEmpty()) {
            out << QString("  Содержимое: %1\n").arg(error.line);
        }
    }

    file.close();
    qDebug() << "Лог сохранен в файл:" << filename;
}

void PropertyParser::clearLog()
{
    errors_.clear();
}