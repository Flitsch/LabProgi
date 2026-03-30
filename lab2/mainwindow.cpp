#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDateEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <QFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Учет недвижимости");
    setupTable();
    loadData();
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
        if (manager_.loadFromFile(currentFileName_)) {
            refreshTable();
            statusBar()->showMessage("Загружено из файла: " + currentFileName_);
        }
    } else {
        // Создаем тестовые данные
        manager_.addProperty(RealEstate("Иванов Иван", QDate(2023, 1, 15), 4500000));
        manager_.addProperty(RealEstate("Петров Петр", QDate(2023, 2, 20), 7800000));
        manager_.addProperty(RealEstate("Сидорова Анна", QDate(2023, 3, 10), 3200000));
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
            return;
        }

        if (price <= 0) {
            QMessageBox::warning(this, "Ошибка", "Введите корректную стоимость");
            return;
        }

        manager_.addProperty(RealEstate(owner, date, price));
        refreshTable();
        saveData();
        statusBar()->showMessage("Объект добавлен");
    }
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
        }
    } else {
        QMessageBox::information(this, "Удаление", "Выберите объект для удаления");
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