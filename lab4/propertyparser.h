#ifndef PROPERTYPARSER_H
#define PROPERTYPARSER_H

#include <QString>
#include <QVector>
#include <QDate>
#include "realestate.h"

struct ParseError {
    int lineNumber;
    QString line;
    QString errorMessage;
};

class PropertyParser {
public:
    PropertyParser();

    // Парсинг одной строки
    RealEstate parseLine(const QString& line, int lineNumber = -1);

    // Парсинг всего файла
    QVector<RealEstate> parseFile(const QString& filename, QVector<ParseError>& errors);

    // Получение лога ошибок
    QString getErrorLog() const;

    // Сохранение лога в файл
    void saveLogToFile(const QString& filename = "error_log.txt") const;

    // Очистка лога
    void clearLog();

private:
    QVector<ParseError> errors_;

    QString extractOwner(const QString& line);
    QDate extractDate(const QString& line);
    int extractPrice(const QString& line);
    void addError(int lineNumber, const QString& line, const QString& message);
};

#endif // PROPERTYPARSER_H