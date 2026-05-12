#include <QtTest>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include "../propertymanager.h"
#include "../propertyparser.h"
#include "../commandprocessor.h"

class TestFileOperations : public QObject
{
    Q_OBJECT

private:
    QString testDataDir;
    QString testInputFile;
    QString testOutputFile;
    QString testCommandsFile;

    void createTestDataFile(const QString& filename, const QStringList& lines);
    QString readFileContent(const QString& filename);

private slots:
    void initTestCase();      // Выполняется один раз перед всеми тестами
    void cleanupTestCase();   // Выполняется один раз после всех тестов
    void init();              // Выполняется перед каждым тестом
    void cleanup();           // Выполняется после каждого теста

    // Тесты для PropertyParser (загрузка/сохранение файлов)
    void testLoadValidFile();
    void testLoadFileWithErrors();
    void testLoadEmptyFile();
    void testLoadNonexistentFile();
    void testSaveToFile();
    void testSaveEmptyList();
    void testOverwriteFile();

    // Тесты для CommandProcessor с файлами
    void testCommandsFromFile();
    void testAddCommandFromFile();
    void testMultipleCommandsFromFile();
    void testCommandsFileWithComments();
    void testCommandsFileWithErrors();
    void testSaveCommandFromFile();
    void testChainCommandsFromFile();

    // Интеграционные тесты
    void testLoadModifySave();
    void testFullWorkflow();
};

// ========== Вспомогательные методы ==========

void TestFileOperations::initTestCase()
{
    // Создаем временную папку для тестов
    testDataDir = QDir::current().absolutePath() + "/test_data";
    QDir().mkdir(testDataDir);

    testInputFile = testDataDir + "/input.txt";
    testOutputFile = testDataDir + "/output.txt";
    testCommandsFile = testDataDir + "/commands.txt";

    qDebug() << "========================================";
    qDebug() << "Тесты файловых операций";
    qDebug() << "  Временная папка:" << testDataDir;
    qDebug() << "========================================";
}

void TestFileOperations::cleanupTestCase()
{
    // Удаляем временную папку после всех тестов
    QDir dir(testDataDir);
    if (dir.exists()) {
        for (const QString& file : dir.entryList(QDir::Files)) {
            dir.remove(file);
        }
        dir.rmdir(testDataDir);
    }
}

void TestFileOperations::init()
{
    // Очищаем перед каждым тестом
    QDir dir(testDataDir);
    for (const QString& file : dir.entryList(QDir::Files)) {
        dir.remove(file);
    }
}

void TestFileOperations::cleanup()
{
    // Очищаем после каждого теста
    QDir dir(testDataDir);
    for (const QString& file : dir.entryList(QDir::Files)) {
        dir.remove(file);
    }
}

void TestFileOperations::createTestDataFile(const QString& filename, const QStringList& lines)
{
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (const QString& line : lines) {
            out << line << "\n";
        }
        file.close();
    }
}

QString TestFileOperations::readFileContent(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return "";
    }

    QTextStream in(&file);
    QString content = in.readAll();
    file.close();
    return content;
}

// ========== Тесты для PropertyParser ==========

void TestFileOperations::testLoadValidFile()
{
    // Создаем корректный файл
    QStringList lines = {
        "Недвижимость \"Иванов Иван\" 2023.01.15 4500000",
        "Недвижимость \"Петров Петр\" 2023.02.20 7800000",
        "Недвижимость \"Сидорова Анна\" 2023.03.10 3200000"
    };
    createTestDataFile(testInputFile, lines);

    // Загружаем файл
    PropertyManager manager;
    QVector<ParseError> errors;
    bool result = manager.loadFromFile(testInputFile, errors);

    QVERIFY(result);
    QCOMPARE(manager.getCount(), 3);
    QCOMPARE(errors.size(), 0);
    QCOMPARE(manager.getProperty(0).getOwner(), QString("Иванов Иван"));
    QCOMPARE(manager.getProperty(1).getOwner(), QString("Петров Петр"));
    QCOMPARE(manager.getProperty(2).getOwner(), QString("Сидорова Анна"));
}

void TestFileOperations::testLoadFileWithErrors()
{
    // Создаем файл с ошибками
    QStringList lines = {
        "Недвижимость \"Иванов Иван\" 2023.01.15 4500000",           // Корректная
        "Недвижимость Иванов 2023.01.15 4500000",                    // Ошибка: нет кавычек
        "Недвижимость \"Петров\" 2023-02-20 7800000",                // Ошибка: неверный формат даты
        "Недвижимость \"Сидоров\" 2023.03.10 3200000",               // Корректная
        "Недвижимость \"\" 2023.04.01 1000000"                       // Ошибка: пустой владелец
    };
    createTestDataFile(testInputFile, lines);

    // Загружаем файл
    PropertyManager manager;
    QVector<ParseError> errors;
    bool result = manager.loadFromFile(testInputFile, errors);

    QVERIFY(result);
    QCOMPARE(manager.getCount(), 2);      // Только корректные строки
    QCOMPARE(errors.size(), 3);            // 3 ошибки
}

void TestFileOperations::testLoadEmptyFile()
{
    // Создаем пустой файл
    createTestDataFile(testInputFile, QStringList());

    PropertyManager manager;
    QVector<ParseError> errors;
    bool result = manager.loadFromFile(testInputFile, errors);

    QVERIFY(!result);  // Пустой файл не содержит данных
    QCOMPARE(manager.getCount(), 0);
}

void TestFileOperations::testLoadNonexistentFile()
{
    PropertyManager manager;
    QVector<ParseError> errors;
    bool result = manager.loadFromFile("nonexistent_file_12345.txt", errors);

    QVERIFY(!result);
    QCOMPARE(manager.getCount(), 0);
}

void TestFileOperations::testSaveToFile()
{
    // Создаем данные
    PropertyManager manager;
    manager.addProperty(RealEstate("Иванов Иван", QDate(2023, 1, 15), 4500000));
    manager.addProperty(RealEstate("Петров Петр", QDate(2023, 2, 20), 7800000));

    // Сохраняем в файл
    bool result = manager.saveToFile(testOutputFile);

    QVERIFY(result);
    QVERIFY(QFile::exists(testOutputFile));

    // Проверяем содержимое
    QString content = readFileContent(testOutputFile);
    QVERIFY(content.contains("Иванов Иван"));
    QVERIFY(content.contains("Петров Петр"));
    QVERIFY(content.contains("2023.01.15"));
    QVERIFY(content.contains("2023.02.20"));
}

void TestFileOperations::testSaveEmptyList()
{
    PropertyManager manager;  // Пустой менеджер

    bool result = manager.saveToFile(testOutputFile);

    QVERIFY(result);
    QVERIFY(QFile::exists(testOutputFile));
}

void TestFileOperations::testOverwriteFile()
{
    // Сначала сохраняем одни данные
    PropertyManager manager1;
    manager1.addProperty(RealEstate("Первый", QDate(2023, 1, 1), 1000));
    manager1.saveToFile(testOutputFile);

    // Затем сохраняем другие данные в тот же файл
    PropertyManager manager2;
    manager2.addProperty(RealEstate("Второй", QDate(2023, 2, 2), 2000));
    manager2.addProperty(RealEstate("Третий", QDate(2023, 3, 3), 3000));
    manager2.saveToFile(testOutputFile);

    // Проверяем, что файл перезаписан
    QString content = readFileContent(testOutputFile);
    QVERIFY(content.contains("Второй"));
    QVERIFY(content.contains("Третий"));
    QVERIFY(!content.contains("Первый"));
}

// ========== Тесты для CommandProcessor с файлами ==========

void TestFileOperations::testCommandsFromFile()
{
    PropertyManager manager;
    CommandProcessor processor(&manager);

    // Создаем файл с командами
    QStringList commands = {
        "ADD Иванов Иван;2023.01.15;4500000",
        "ADD Петров Петр;2023.02.20;7800000",
        "ADD Сидоров Сидор;2023.03.10;3200000",
        "REM price < 5000000",
        "SAVE " + testOutputFile
    };
    createTestDataFile(testCommandsFile, commands);

    // Выполняем команды
    QVector<CommandResult> results = processor.executeCommandsFromFile(testCommandsFile);

    QCOMPARE(results.size(), 5);
    QVERIFY(results[0].success);
    QVERIFY(results[1].success);
    QVERIFY(results[2].success);
    QVERIFY(results[3].success);
    QVERIFY(results[4].success);

    // Проверяем, что остались только объекты дороже 5 млн
    QCOMPARE(manager.getCount(), 1);
    QCOMPARE(manager.getProperty(0).getOwner(), QString("Петров Петр"));
}

void TestFileOperations::testAddCommandFromFile()
{
    PropertyManager manager;
    CommandProcessor processor(&manager);

    QStringList commands = {
        "ADD Иванов Иван;2023.01.15;4500000",
        "ADD Петров Петр;2023.02.20;7800000"
    };
    createTestDataFile(testCommandsFile, commands);

    processor.executeCommandsFromFile(testCommandsFile);

    QCOMPARE(manager.getCount(), 2);
    QCOMPARE(manager.getProperty(0).getOwner(), QString("Иванов Иван"));
    QCOMPARE(manager.getProperty(1).getOwner(), QString("Петров Петр"));
}

void TestFileOperations::testMultipleCommandsFromFile()
{
    PropertyManager manager;
    CommandProcessor processor(&manager);

    QStringList commands = {
        "# Это комментарий - будет пропущен",
        "ADD Иванов Иван;2023.01.15;4500000",
        "ADD Петров Петр;2023.02.20;7800000",
        "",
        "REM owner = Иванов Иван",
        "ADD Сидоров Сидор;2023.03.10;3200000",
        "SAVE " + testOutputFile
    };
    createTestDataFile(testCommandsFile, commands);

    QVector<CommandResult> results = processor.executeCommandsFromFile(testCommandsFile);

    QCOMPARE(results.size(), 5);  // 5 команд (комментарий и пустая строка пропущены)
    QCOMPARE(manager.getCount(), 2);  // Петров и Сидоров
}

void TestFileOperations::testCommandsFileWithComments()
{
    PropertyManager manager;
    CommandProcessor processor(&manager);

    QStringList commands = {
        "# Первая команда - добавляем Иванова",
        "ADD Иванов Иван;2023.01.15;4500000",
        "# Вторая команда - добавляем Петрова",
        "ADD Петров Петр;2023.02.20;7800000",
        "  # Комментарий с пробелами в начале",
        "REM price < 5000000",
        "# Сохраняем результат",
        "SAVE " + testOutputFile
    };
    createTestDataFile(testCommandsFile, commands);

    QVector<CommandResult> results = processor.executeCommandsFromFile(testCommandsFile);

    QCOMPARE(results.size(), 4);  // 4 полезные команды
    QVERIFY(results[0].success);
    QVERIFY(results[1].success);
    QVERIFY(results[2].success);
    QVERIFY(results[3].success);
}

void TestFileOperations::testCommandsFileWithErrors()
{
    PropertyManager manager;
    CommandProcessor processor(&manager);

    QStringList commands = {
        "ADD Иванов Иван;2023.01.15;4500000",
        "INVALID COMMAND",                    // Неизвестная команда
        "REM price < 1000",                   // Условие без объектов
        "ADD Петров Петр;2023.02.20",         // Недостаточно полей (нет цены)
        "SAVE " + testOutputFile
    };
    createTestDataFile(testCommandsFile, commands);

    QVector<CommandResult> results = processor.executeCommandsFromFile(testCommandsFile);

    QCOMPARE(results.size(), 5);
    QVERIFY(results[0].success);   // ADD успешен
    QVERIFY(!results[1].success);  // INVALID - ошибка
    QVERIFY(results[2].success);   // REM успешен (0 объектов удалено)
    QVERIFY(!results[3].success);  // ADD с ошибкой
    QVERIFY(results[4].success);   // SAVE успешен

    // Проверяем, что только первая ADD сработала
    QCOMPARE(manager.getCount(), 1);
}

void TestFileOperations::testSaveCommandFromFile()
{
    PropertyManager manager;
    CommandProcessor processor(&manager);

    // Создаем данные
    manager.addProperty(RealEstate("Тестовый Объект", QDate(2024, 1, 15), 1000000));
    manager.addProperty(RealEstate("Еще Один", QDate(2024, 2, 20), 2000000));

    QStringList commands = {
        "SAVE " + testOutputFile
    };
    createTestDataFile(testCommandsFile, commands);

    processor.executeCommandsFromFile(testCommandsFile);

    QVERIFY(QFile::exists(testOutputFile));
    QString content = readFileContent(testOutputFile);
    QVERIFY(content.contains("Тестовый Объект"));
    QVERIFY(content.contains("Еще Один"));
}

void TestFileOperations::testChainCommandsFromFile()
{
    PropertyManager manager;
    CommandProcessor processor(&manager);

    QStringList commands = {
        "ADD Объект1;2023.01.01;1000",
        "ADD Объект2;2023.02.02;2000",
        "ADD Объект3;2023.03.03;3000",
        "ADD Объект4;2023.04.04;4000",
        "REM price < 2500",
        "ADD Объект5;2023.05.05;5000",
        "REM owner = Объект3",
        "SAVE " + testOutputFile
    };
    createTestDataFile(testCommandsFile, commands);

    QVector<CommandResult> results = processor.executeCommandsFromFile(testCommandsFile);

    // Проверяем, что все команды выполнились
    QCOMPARE(results.size(), 8);
    for (const auto& result : results) {
        QVERIFY(result.success);
    }

    // Должны остаться: Объект4 (4000) и Объект5 (5000)
    QCOMPARE(manager.getCount(), 2);
}

// ========== Интеграционные тесты ==========

void TestFileOperations::testLoadModifySave()
{
    // 1. Создаем исходный файл
    QStringList initialData = {
        "Недвижимость \"Иванов Иван\" 2023.01.15 4500000",
        "Недвижимость \"Петров Петр\" 2023.02.20 7800000",
        "Недвижимость \"Сидоров Сидор\" 2023.03.10 3200000"
    };
    createTestDataFile(testInputFile, initialData);

    // 2. Загружаем данные
    PropertyManager manager;
    QVector<ParseError> errors;
    manager.loadFromFile(testInputFile, errors);
    QCOMPARE(manager.getCount(), 3);

    // 3. Модифицируем через команды
    CommandProcessor processor(&manager);
    processor.executeCommand("REM owner = Иванов Иван");
    QCOMPARE(manager.getCount(), 2);

    // 4. Сохраняем в новый файл
    manager.saveToFile(testOutputFile);

    // 5. Проверяем результат
    QString content = readFileContent(testOutputFile);
    QVERIFY(content.contains("Петров Петр"));
    QVERIFY(content.contains("Сидоров Сидор"));
    QVERIFY(!content.contains("Иванов Иван"));
}

void TestFileOperations::testFullWorkflow()
{
    // Полный цикл работы с программой

    // Шаг 1: Загружаем начальные данные
    QStringList initialData = {
        "Недвижимость \"Иванов Иван\" 2023.01.15 1500000",
        "Недвижимость \"Петров Петр\" 2023.02.20 2500000",
        "Недвижимость \"Сидоров Сидор\" 2023.03.10 3500000",
        "Недвижимость \"Козлова Анна\" 2023.04.05 4500000",
        "Недвижимость \"Смирнов Павел\" 2023.05.12 5500000"
    };
    createTestDataFile(testInputFile, initialData);

    PropertyManager manager;
    QVector<ParseError> errors;
    manager.loadFromFile(testInputFile, errors);
    QCOMPARE(manager.getCount(), 5);

    // Шаг 2: Выполняем команды из файла
    QStringList commands = {
        "# Удаляем дешевые объекты (до 3 млн)",
        "REM price < 3000000",
        "",
        "# Добавляем новые объекты",
        "ADD Орлова Мария;2023.06.01;6500000",
        "ADD Васильев Игорь;2023.07.01;7500000",
        "",
        "# Удаляем по владельцу",
        "REM owner = Сидоров Сидор",
        "",
        "# Сохраняем результат",
        "SAVE " + testOutputFile
    };
    createTestDataFile(testCommandsFile, commands);

    CommandProcessor processor(&manager);
    QVector<CommandResult> results = processor.executeCommandsFromFile(testCommandsFile);

    // Проверяем успешность всех команд
    for (const auto& result : results) {
        QVERIFY(result.success);
    }

    // Должны остаться: Козлова (4.5M), Смирнов (5.5M), Орлова (6.5M), Васильев (7.5M)
    QCOMPARE(manager.getCount(), 4);

    // Шаг 3: Проверяем сохраненный файл
    QVERIFY(QFile::exists(testOutputFile));
    QString content = readFileContent(testOutputFile);
    QVERIFY(content.contains("Козлова Анна"));
    QVERIFY(content.contains("Смирнов Павел"));
    QVERIFY(content.contains("Орлова Мария"));
    QVERIFY(content.contains("Васильев Игорь"));
    QVERIFY(!content.contains("Иванов Иван"));
    QVERIFY(!content.contains("Петров Петр"));
    QVERIFY(!content.contains("Сидоров Сидор"));
}

QTEST_MAIN(TestFileOperations)
#include "test_fileoperations.moc"