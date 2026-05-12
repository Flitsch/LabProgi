#include "fileloaderdialog.h"
#include <QTextBrowser>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QFileDialog>
#include <QDateTime>
#include <QDebug>

FileLoaderDialog::FileLoaderDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Загрузка данных из файла");
    setModal(false);
    resize(800, 600);

    textBrowser = new QTextBrowser(this);
    textBrowser->setFontFamily("Courier New");
    textBrowser->setFontPointSize(10);

    openButton = new QPushButton("Открыть файл", this);
    processButton = new QPushButton("Обработать и передать данные", this);
    processButton->setEnabled(false);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(textBrowser);
    layout->addWidget(openButton);
    layout->addWidget(processButton);

    connect(openButton, &QPushButton::clicked, this, &FileLoaderDialog::on_openButton_clicked);
    connect(processButton, &QPushButton::clicked, this, &FileLoaderDialog::on_processButton_clicked);

    textBrowser->append("╔═══════════════════════════════════════════════════════════╗");
    textBrowser->append("║     ОКНО ЗАГРУЗКИ ДАННЫХ                                  ║");
    textBrowser->append("║                                                          ║");
    textBrowser->append("║     Нажмите 'Открыть файл' для выбора файла              ║");
    textBrowser->append("║     Затем нажмите 'Обработать' для передачи данных       ║");
    textBrowser->append("╚═══════════════════════════════════════════════════════════╝");
}

FileLoaderDialog::~FileLoaderDialog() {}

void FileLoaderDialog::saveErrorToLog(const QString& errorMsg, const QString& line, int lineNumber)
{
    QFile logFile("error_log.txt");
    if (logFile.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&logFile);
        out << "\n========================================\n";
        out << QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss") << "\n";
        out << "=== ОШИБКА ПРИ ЗАГРУЗКЕ ФАЙЛА ===\n";
        out << QString("Строка %1: %2\n").arg(lineNumber).arg(errorMsg);
        if (!line.isEmpty()) {
            out << QString("  Содержимое: %1\n").arg(line);
        }
        logFile.close();
        qDebug() << "Ошибка записана в лог-файл";
    }
}

void FileLoaderDialog::on_openButton_clicked()
{
    currentFileName = QFileDialog::getOpenFileName(this,
                                                   "Выберите файл с данными о недвижимости",
                                                   "",
                                                   "Текстовые файлы (*.txt);;Все файлы (*)");

    if (currentFileName.isEmpty()) {
        return;
    }

    textBrowser->clear();
    textBrowser->append("╔═══════════════════════════════════════════════════════════╗");
    textBrowser->append("║     СОДЕРЖИМОЕ ФАЙЛА                                      ║");
    textBrowser->append("╚═══════════════════════════════════════════════════════════╝\n");

    QFile file(currentFileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString errorMsg = "Не удалось открыть файл: " + currentFileName;
        QMessageBox::warning(this, "Ошибка", errorMsg);
        saveErrorToLog(errorMsg, "", -1);
        return;
    }

    loadedData.clear();
    QTextStream in(&file);
    int lineNum = 1;
    int errorCount = 0;

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) {
            lineNum++;
            continue;
        }

        textBrowser->append(QString("%1. %2").arg(lineNum).arg(line));

        try {
            int firstQuote = line.indexOf('"');
            int secondQuote = line.indexOf('"', firstQuote + 1);

            if (firstQuote != -1 && secondQuote != -1) {
                QString owner = line.mid(firstQuote + 1, secondQuote - firstQuote - 1);
                QString remaining = line.mid(secondQuote + 1).trimmed();
                QStringList parts = remaining.split(' ', Qt::SkipEmptyParts);

                if (parts.size() >= 2) {
                    QDate date = QDate::fromString(parts[0], "yyyy.MM.dd");
                    int price = parts[1].toInt();
                    if (date.isValid() && price >= 0) {
                        loadedData.push_back(RealEstate(owner, date, price));
                    } else {
                        errorCount++;
                        QString errorMsg = "Неверный формат даты или отрицательная цена";
                        textBrowser->append(QString("  ⚠ %1").arg(errorMsg));
                        saveErrorToLog(errorMsg, line, lineNum);
                    }
                } else {
                    errorCount++;
                    QString errorMsg = "Недостаточно данных в строке";
                    textBrowser->append(QString("  ⚠ %1").arg(errorMsg));
                    saveErrorToLog(errorMsg, line, lineNum);
                }
            } else {
                errorCount++;
                QString errorMsg = "Отсутствуют кавычки вокруг имени владельца";
                textBrowser->append(QString("  ⚠ %1").arg(errorMsg));
                saveErrorToLog(errorMsg, line, lineNum);
            }
        } catch (const std::exception& e) {
            errorCount++;
            QString errorMsg = QString("Ошибка парсинга: %1").arg(e.what());
            textBrowser->append(QString("  ⚠ %1").arg(errorMsg));
            saveErrorToLog(errorMsg, line, lineNum);
        }

        lineNum++;
    }

    file.close();

    textBrowser->append("");
    textBrowser->append("───────────────────────────────────────────────────────────────");
    textBrowser->append(QString("Всего объектов в файле: %1").arg(loadedData.size()));
    textBrowser->append(QString("Ошибок: %1").arg(errorCount));
    textBrowser->append("───────────────────────────────────────────────────────────────");

    if (loadedData.size() > 0) {
        processButton->setEnabled(true);
        QMessageBox::information(this, "Успех",
                                 QString("Загружено %1 объектов из файла (пропущено %2 ошибок)\n\n"
                                         "Ошибки записаны в файл error_log.txt")
                                     .arg(loadedData.size()).arg(errorCount));
    } else {
        processButton->setEnabled(false);
        QMessageBox::warning(this, "Предупреждение",
                             QString("Не удалось загрузить данные из файла. Обнаружено %1 ошибок.\n\n"
                                     "Проверьте формат строк и файл error_log.txt")
                                 .arg(errorCount));
    }
}

void FileLoaderDialog::on_processButton_clicked()
{
    if (loadedData.size() > 0) {
        emit dataLoaded(loadedData);
        QMessageBox::information(this, "Успех",
                                 QString("Передано %1 объектов в главное окно").arg(loadedData.size()));
        processButton->setEnabled(false);
    }
}