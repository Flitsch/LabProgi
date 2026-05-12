#ifndef COMMANDPROCESSOR_H
#define COMMANDPROCESSOR_H

#include <QString>
#include <QVector>
#include "realestate.h"
#include "propertymanager.h"

// Структура для хранения результата выполнения команды
struct CommandResult {
    bool success;
    QString message;
    int affectedRows;

    CommandResult() : success(true), message(""), affectedRows(0) {}
    CommandResult(bool s, const QString& msg, int rows = 0)
        : success(s), message(msg), affectedRows(rows) {}
};

// Класс для обработки команд
class CommandProcessor {
public:
    CommandProcessor(PropertyManager* manager);

    // Выполнение команды из строки
    CommandResult executeCommand(const QString& commandLine);

    // Выполнение всех команд из файла
    QVector<CommandResult> executeCommandsFromFile(const QString& filename);

    // Получение лога выполнения
    QString getExecutionLog() const;
    void clearLog();

private:
    PropertyManager* manager_;
    QVector<QString> log_;

    // Парсинг команды ADD: ADD field1;field2;field3;field4
    CommandResult parseAndExecuteAdd(const QString& args);

    // Парсинг команды REM: REM <поле> <оператор> <значение>
    CommandResult parseAndExecuteRemove(const QString& args);

    // Парсинг команды SAVE: SAVE filename
    CommandResult parseAndExecuteSave(const QString& args);

    // Валидация и парсинг CSV строки для ADD
    RealEstate parseCsvLine(const QString& csvLine);

    // Проверка условия для REM
    bool checkCondition(const RealEstate& property, const QString& field,
                        const QString& op, const QString& value);

    void addToLog(const QString& message);
};

#endif // COMMANDPROCESSOR_H