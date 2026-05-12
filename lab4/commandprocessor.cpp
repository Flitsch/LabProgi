#include "commandprocessor.h"
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QDebug>

CommandProcessor::CommandProcessor(PropertyManager* manager)
    : manager_(manager)
{
}

CommandResult CommandProcessor::executeCommand(const QString& commandLine)
{
    QString trimmed = commandLine.trimmed();
    if (trimmed.isEmpty()) {
        return CommandResult(false, "Пустая команда");
    }

    // Разделяем команду и аргументы
    int spacePos = trimmed.indexOf(' ');
    QString command;
    QString args;

    if (spacePos == -1) {
        command = trimmed.toUpper();
        args = "";
    } else {
        command = trimmed.left(spacePos).toUpper();
        args = trimmed.mid(spacePos + 1).trimmed();
    }

    addToLog(QString("Выполнение команды: %1 %2").arg(command).arg(args));

    if (command == "ADD") {
        return parseAndExecuteAdd(args);
    } else if (command == "REM") {
        return parseAndExecuteRemove(args);
    } else if (command == "SAVE") {
        return parseAndExecuteSave(args);
    } else {
        return CommandResult(false, QString("Неизвестная команда: %1").arg(command));
    }
}

CommandResult CommandProcessor::parseAndExecuteAdd(const QString& args)
{
    if (args.isEmpty()) {
        return CommandResult(false, "ADD: не указаны аргументы");
    }

    try {
        RealEstate property = parseCsvLine(args);
        if (!property.isValid()) {
            return CommandResult(false, "ADD: неверные данные");
        }

        manager_->addProperty(property);
        addToLog(QString("  Добавлен объект: %1, %2, %3 руб.")
                     .arg(property.getOwner())
                     .arg(property.getDate().toString("yyyy.MM.dd"))
                     .arg(property.getPrice()));

        return CommandResult(true, "Объект добавлен", 1);

    } catch (const std::exception& e) {
        return CommandResult(false, QString("ADD ошибка: %1").arg(e.what()));
    }
}

RealEstate CommandProcessor::parseCsvLine(const QString& csvLine)
{
    // Формат: "Иванов Иван;2023.01.15;4500000"
    QStringList parts = csvLine.split(';', Qt::SkipEmptyParts);

    if (parts.size() < 3) {
        throw std::runtime_error("Недостаточно полей в CSV");
    }

    QString owner = parts[0].trimmed();
    QDate date = QDate::fromString(parts[1].trimmed(), "yyyy.MM.dd");
    int price = parts[2].trimmed().toInt();

    if (owner.isEmpty()) {
        throw std::runtime_error("Имя владельца не может быть пустым");
    }
    if (!date.isValid()) {
        throw std::runtime_error("Неверный формат даты");
    }
    if (price < 0) {
        throw std::runtime_error("Стоимость не может быть отрицательной");
    }

    return RealEstate(owner, date, price);
}

CommandResult CommandProcessor::parseAndExecuteRemove(const QString& args)
{
    if (args.isEmpty()) {
        return CommandResult(false, "REM: не указано условие");
    }

    // Формат: REM <поле> <оператор> <значение>
    // Пример: REM price < 1000
    //         REM owner = Иванов
    //         REM date > 2023.01.01

    QStringList parts = args.split(' ', Qt::SkipEmptyParts);
    if (parts.size() < 3) {
        return CommandResult(false, "REM: неверный формат условия. Ожидается: поле оператор значение");
    }

    QString field = parts[0].toLower();
    QString op = parts[1];
    QString value = parts.mid(2).join(' ');

    addToLog(QString("  Условие: %1 %2 %3").arg(field).arg(op).arg(value));

    // Собираем индексы для удаления
    QVector<int> indicesToRemove;
    const auto& properties = manager_->getAllProperties();

    for (size_t i = 0; i < properties.size(); ++i) {
        if (checkCondition(properties[i], field, op, value)) {
            indicesToRemove.push_back(static_cast<int>(i));
        }
    }

    // Удаляем в обратном порядке
    int removedCount = 0;
    for (int i = indicesToRemove.size() - 1; i >= 0; --i) {
        manager_->removeProperty(indicesToRemove[i]);
        removedCount++;
    }

    addToLog(QString("  Удалено объектов: %1").arg(removedCount));

    return CommandResult(true,
                         QString("Удалено %1 объектов, соответствующих условию").arg(removedCount),
                         removedCount);
}

bool CommandProcessor::checkCondition(const RealEstate& property,
                                      const QString& field,
                                      const QString& op,
                                      const QString& value)
{
    // owner - строковое поле
    if (field == "owner") {
        QString propValue = property.getOwner();
        QString condValue = value;

        if (op == "=" || op == "==") {
            return propValue == condValue;
        } else if (op == "!=") {
            return propValue != condValue;
        } else if (op == "contains") {
            return propValue.contains(condValue);
        }
        return false;
    }
    // date - поле даты
    else if (field == "date") {
        QDate propDate = property.getDate();
        QDate condDate = QDate::fromString(value, "yyyy.MM.dd");

        if (!condDate.isValid()) return false;

        if (op == "=" || op == "==") {
            return propDate == condDate;
        } else if (op == "!=") {
            return propDate != condDate;
        } else if (op == "<") {
            return propDate < condDate;
        } else if (op == "<=") {
            return propDate <= condDate;
        } else if (op == ">") {
            return propDate > condDate;
        } else if (op == ">=") {
            return propDate >= condDate;
        }
        return false;
    }
    // price - числовое поле
    else if (field == "price") {
        int propPrice = property.getPrice();
        bool ok;
        int condPrice = value.toInt(&ok);

        if (!ok) return false;

        if (op == "=" || op == "==") {
            return propPrice == condPrice;
        } else if (op == "!=") {
            return propPrice != condPrice;
        } else if (op == "<") {
            return propPrice < condPrice;
        } else if (op == "<=") {
            return propPrice <= condPrice;
        } else if (op == ">") {
            return propPrice > condPrice;
        } else if (op == ">=") {
            return propPrice >= condPrice;
        }
        return false;
    }

    return false;
}

CommandResult CommandProcessor::parseAndExecuteSave(const QString& args)
{
    if (args.isEmpty()) {
        return CommandResult(false, "SAVE: не указано имя файла");
    }

    QString filename = args.trimmed();

    if (manager_->saveToFile(filename)) {
        addToLog(QString("  Сохранено в файл: %1").arg(filename));
        return CommandResult(true, QString("Сохранено в файл: %1").arg(filename));
    } else {
        return CommandResult(false, QString("Ошибка сохранения в файл: %1").arg(filename));
    }
}

QVector<CommandResult> CommandProcessor::executeCommandsFromFile(const QString& filename)
{
    QVector<CommandResult> results;
    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        results.append(CommandResult(false, QString("Не удалось открыть файл: %1").arg(filename)));
        return results;
    }

    clearLog();
    addToLog(QString("Загрузка команд из файла: %1").arg(filename));

    QTextStream in(&file);
    int lineNumber = 1;

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty() || line.startsWith('#')) {
            lineNumber++;
            continue;
        }

        addToLog(QString("Строка %1: %2").arg(lineNumber).arg(line));
        CommandResult result = executeCommand(line);
        results.append(result);

        if (!result.success) {
            addToLog(QString("  ОШИБКА: %1").arg(result.message));
        }

        lineNumber++;
    }

    file.close();
    return results;
}

void CommandProcessor::addToLog(const QString& message)
{
    log_.append(message);
    qDebug() << message;
}

QString CommandProcessor::getExecutionLog() const
{
    return log_.join("\n");
}

void CommandProcessor::clearLog()
{
    log_.clear();
}