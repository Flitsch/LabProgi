#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "fileloaderdialog.h"
#include "propertyparser.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDateEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Учет недвижимости");
    setupTable();
    loadData();

    // Показываем расположение лог-файла
    QString logPath = QDir::current().absolutePath() + "/error_log.txt";
    statusBar()->showMessage(QString("Лог-файл будет сохранен: %1").arg(logPath));
}

MainWindow::~MainWindow()
{
    saveData();
    delete ui;
}

void MainWindow::setupTable()
{
    ui->tableWidget->setColumnCount(3);
    QStringList headers = {"Владелец", "Дата постановки", "Стоимость (руб.)"};
    ui->tableWidget->setHorizontalHeaderLabels(headers);

    ui->tableWidget->setColumnWidth(0, 250);
    ui->tableWidget->setColumnWidth(1, 120);
    ui->tableWidget->setColumnWidth(2, 150);

    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
}

void MainWindow::refreshTable()
{
    ui->tableWidget->setRowCount(0);

    const auto& properties = manager_.getAllProperties();
    for (size_t i = 0; i < properties.size(); ++i) {
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());

        QTableWidgetItem* ownerItem = new QTableWidgetItem(properties[i].getOwner());
        QTableWidgetItem* dateItem = new QTableWidgetItem(
            properties[i].getDate().toString("yyyy.MM.dd"));
        QTableWidgetItem* priceItem = new QTableWidgetItem(
            QString::number(properties[i].getPrice()));

        priceItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

        ui->tableWidget->setItem(static_cast<int>(i), 0, ownerItem);
        ui->tableWidget->setItem(static_cast<int>(i), 1, dateItem);
        ui->tableWidget->setItem(static_cast<int>(i), 2, priceItem);
    }

    updateStatusBar();
}

void MainWindow::loadData()
{
    currentFileName_ = "realestate.txt";

    if (QFile::exists(currentFileName_)) {
        QVector<ParseError> errors;
        if (manager_.loadFromFile(currentFileName_, errors)) {
            refreshTable();
            statusBar()->showMessage("Загружено из файла: " + currentFileName_);
        }

        if (!errors.isEmpty()) {
            showErrorLog(errors);

            // Сохраняем лог в файл
            QFile logFile("error_log.txt");
            if (logFile.open(QIODevice::Append | QIODevice::Text)) {
                QTextStream out(&logFile);
                out << "\n========================================\n";
                out << QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss") << "\n";
                out << "=== ОШИБКИ ПРИ ЗАГРУЗКЕ ФАЙЛА realestate.txt ===\n";
                for (const auto& error : errors) {
                    out << QString("Строка %1: %2\n").arg(error.lineNumber).arg(error.errorMessage);
                    if (!error.line.isEmpty()) {
                        out << QString("  Содержимое: %1\n").arg(error.line);
                    }
                }
                logFile.close();
                statusBar()->showMessage("Лог ошибок сохранен в error_log.txt");
            }
        }
    } else {
        // Создаем тестовые данные
        try {
            manager_.addProperty(RealEstate("Иванов Иван", QDate(2023, 1, 15), 4500000));
            manager_.addProperty(RealEstate("Петров Петр", QDate(2023, 2, 20), 7800000));
            manager_.addProperty(RealEstate("Сидорова Анна", QDate(2023, 3, 10), 3200000));

            // Запись в лог о создании тестовых данных
            QFile logFile("error_log.txt");
            if (logFile.open(QIODevice::Append | QIODevice::Text)) {
                QTextStream out(&logFile);
                out << "\n========================================\n";
                out << QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss") << "\n";
                out << "=== СОЗДАНЫ ТЕСТОВЫЕ ДАННЫЕ ===\n";
                out << "Создано 3 тестовых объекта\n\n";
                logFile.close();
            }
        } catch (const RealEstateException& e) {
            QMessageBox::warning(this, "Ошибка", e.getMessage());
        }
        refreshTable();
        saveData();
        statusBar()->showMessage("Создан новый файл данных");
    }
}

void MainWindow::saveData()
{
    if (!currentFileName_.isEmpty()) {
        manager_.saveToFile(currentFileName_);
    }
}

void MainWindow::updateStatusBar()
{
    int count = manager_.getCount();
    long long total = manager_.getTotalValue();
    int expensive = manager_.countExpensive(5000);

    statusBar()->showMessage(
        QString("Всего: %1 | Общая стоимость: %2 руб. | Дороже 5000 руб.: %3")
            .arg(count).arg(total).arg(expensive));
}

void MainWindow::showAddDialog()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Добавить объект недвижимости");

    QVBoxLayout* layout = new QVBoxLayout(&dialog);

    QHBoxLayout* ownerLayout = new QHBoxLayout();
    QLabel* ownerLabel = new QLabel("Владелец:");
    QLineEdit* ownerEdit = new QLineEdit();
    ownerLayout->addWidget(ownerLabel);
    ownerLayout->addWidget(ownerEdit);

    QHBoxLayout* dateLayout = new QHBoxLayout();
    QLabel* dateLabel = new QLabel("Дата (ГГГГ.ММ.ДД):");
    QDateEdit* dateEdit = new QDateEdit();
    dateEdit->setCalendarPopup(true);
    dateEdit->setDisplayFormat("yyyy.MM.dd");
    dateEdit->setDate(QDate::currentDate());
    dateLayout->addWidget(dateLabel);
    dateLayout->addWidget(dateEdit);

    QHBoxLayout* priceLayout = new QHBoxLayout();
    QLabel* priceLabel = new QLabel("Стоимость (руб.):");
    QLineEdit* priceEdit = new QLineEdit();
    priceLayout->addWidget(priceLabel);
    priceLayout->addWidget(priceEdit);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    layout->addLayout(ownerLayout);
    layout->addLayout(dateLayout);
    layout->addLayout(priceLayout);
    layout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        QString owner = ownerEdit->text().trimmed();
        QDate date = dateEdit->date();
        int price = priceEdit->text().toInt();

        if (owner.isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Введите владельца");
            // Запись в лог
            QFile logFile("error_log.txt");
            if (logFile.open(QIODevice::Append | QIODevice::Text)) {
                QTextStream out(&logFile);
                out << "\n========================================\n";
                out << QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss") << "\n";
                out << "ОШИБКА ПРИ ДОБАВЛЕНИИ: Владелец не может быть пустым\n\n";
                logFile.close();
            }
            return;
        }

        if (price <= 0) {
            QMessageBox::warning(this, "Ошибка", "Введите корректную стоимость");
            // Запись в лог
            QFile logFile("error_log.txt");
            if (logFile.open(QIODevice::Append | QIODevice::Text)) {
                QTextStream out(&logFile);
                out << "\n========================================\n";
                out << QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss") << "\n";
                out << QString("ОШИБКА ПРИ ДОБАВЛЕНИИ: Некорректная стоимость (%1)\n\n").arg(price);
                logFile.close();
            }
            return;
        }

        try {
            manager_.addProperty(RealEstate(owner, date, price));
            refreshTable();
            saveData();
            statusBar()->showMessage("Объект добавлен");

            // Запись в лог об успешном добавлении
            QFile logFile("error_log.txt");
            if (logFile.open(QIODevice::Append | QIODevice::Text)) {
                QTextStream out(&logFile);
                out << "\n========================================\n";
                out << QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss") << "\n";
                out << "ДОБАВЛЕН ОБЪЕКТ:\n";
                out << QString("  Владелец: %1\n").arg(owner);
                out << QString("  Дата: %1\n").arg(date.toString("yyyy.MM.dd"));
                out << QString("  Стоимость: %1 руб.\n\n").arg(price);
                logFile.close();
            }
        } catch (const RealEstateException& e) {
            QMessageBox::warning(this, "Ошибка", e.getMessage());
            // Запись в лог
            QFile logFile("error_log.txt");
            if (logFile.open(QIODevice::Append | QIODevice::Text)) {
                QTextStream out(&logFile);
                out << "\n========================================\n";
                out << QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss") << "\n";
                out << "ОШИБКА ПРИ ДОБАВЛЕНИИ: " << e.getMessage() << "\n";
                out << "  Владелец: " << owner << "\n";
                out << "  Дата: " << date.toString("yyyy.MM.dd") << "\n";
                out << "  Цена: " << price << "\n\n";
                logFile.close();
            }
        }
    }
}

void MainWindow::showErrorLog(const QVector<ParseError>& errors)
{
    QString logText = "Обнаружены ошибки при загрузке файла:\n\n";
    for (const auto& error : errors) {
        logText += QString("• Строка %1: %2\n").arg(error.lineNumber).arg(error.errorMessage);
        if (!error.line.isEmpty()) {
            logText += QString("  \"%1\"\n").arg(error.line);
        }
    }

    QMessageBox::warning(this, "Ошибки в файле данных", logText);
}

void MainWindow::processLoadedFileData()
{
    if (loadedFromFileData.size() == 0) {
        QMessageBox::information(this, "Информация",
                                 "Нет данных для обработки.\n\n"
                                 "Порядок действий:\n"
                                 "1. Нажмите 'Открыть файл'\n"
                                 "2. Выберите файл во втором окне\n"
                                 "3. Нажмите 'Обработать' во втором окне\n"
                                 "4. Нажмите 'Обработать данные из файла' в этом окне");
        return;
    }

    try {
        manager_.clearAll();

        for (const auto& prop : loadedFromFileData) {
            manager_.addProperty(prop);
        }

        refreshTable();
        saveData();

        statusBar()->showMessage(QString("Загружено %1 объектов из файла (старые данные заменены)")
                                     .arg(loadedFromFileData.size()));

        QMessageBox::information(this, "Успех",
                                 QString("Данные успешно загружены!\n\n"
                                         "Загружено объектов: %1\n"
                                         "Старые данные полностью заменены.")
                                     .arg(loadedFromFileData.size()));

        // Запись в лог о загрузке данных
        QFile logFile("error_log.txt");
        if (logFile.open(QIODevice::Append | QIODevice::Text)) {
            QTextStream out(&logFile);
            out << "\n========================================\n";
            out << QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss") << "\n";
            out << "ЗАГРУЖЕНЫ ДАННЫЕ ИЗ ФАЙЛА:\n";
            out << QString("  Загружено объектов: %1\n").arg(loadedFromFileData.size());
            out << "  Старые данные заменены\n\n";
            logFile.close();
        }

        loadedFromFileData.clear();

    } catch (const RealEstateException& e) {
        QString errorMsg = QString("Ошибка при обработке данных: %1").arg(e.getMessage());
        statusBar()->showMessage(errorMsg);

        QFile logFile("error_log.txt");
        if (logFile.open(QIODevice::Append | QIODevice::Text)) {
            QTextStream out(&logFile);
            out << "\n========================================\n";
            out << QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss") << "\n";
            out << "ОШИБКА ПРИ ЗАГРУЗКЕ ДАННЫХ: " << errorMsg << "\n\n";
            logFile.close();
        }

        QMessageBox::critical(this, "Ошибка", errorMsg);
    }
}

void MainWindow::receiveLoadedData(const QVector<RealEstate>& data)
{
    loadedFromFileData = data;
    statusBar()->showMessage(QString("Получено %1 объектов из дополнительного окна. Нажмите 'Обработать данные из файла' для замены текущих данных.")
                                 .arg(data.size()));
}

void MainWindow::on_addButton_clicked()
{
    showAddDialog();
}

void MainWindow::on_deleteButton_clicked()
{
    int currentRow = ui->tableWidget->currentRow();
    if (currentRow >= 0) {
        QString owner = manager_.getProperty(currentRow).getOwner();

        QMessageBox::StandardButton reply = QMessageBox::question(
            this, "Подтверждение удаления",
            QString("Удалить объект владельца \"%1\"?").arg(owner),
            QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            manager_.removeProperty(currentRow);
            refreshTable();
            saveData();
            statusBar()->showMessage("Объект удален");

            // Запись в лог об удалении
            QFile logFile("error_log.txt");
            if (logFile.open(QIODevice::Append | QIODevice::Text)) {
                QTextStream out(&logFile);
                out << "\n========================================\n";
                out << QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss") << "\n";
                out << "УДАЛЕН ОБЪЕКТ:\n";
                out << QString("  Владелец: %1\n\n").arg(owner);
                logFile.close();
            }
        }
    } else {
        QMessageBox::information(this, "Удаление", "Выберите объект для удаления");

        // Запись в лог попытки удаления без выбора
        QFile logFile("error_log.txt");
        if (logFile.open(QIODevice::Append | QIODevice::Text)) {
            QTextStream out(&logFile);
            out << "\n========================================\n";
            out << QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss") << "\n";
            out << "ПРЕДУПРЕЖДЕНИЕ: Попытка удаления без выбора объекта\n\n";
            logFile.close();
        }
    }
}

void MainWindow::on_refreshButton_clicked()
{
    refreshTable();
}

void MainWindow::on_tableWidget_itemSelectionChanged()
{
    ui->deleteButton->setEnabled(ui->tableWidget->currentRow() >= 0);
}

void MainWindow::on_openFileDialogButton_clicked()
{
    FileLoaderDialog *dialog = new FileLoaderDialog(this);
    connect(dialog, &FileLoaderDialog::dataLoaded,
            this, &MainWindow::receiveLoadedData);
    dialog->show();
}

void MainWindow::on_processLoadedButton_clicked()
{
    processLoadedFileData();
}