#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include "propertymanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_addButton_clicked();
    void on_deleteButton_clicked();
    void on_refreshButton_clicked();
    void on_tableWidget_itemSelectionChanged();

private:
    Ui::MainWindow *ui;
    PropertyManager manager_;
    QString currentFileName_;

    void setupTable();
    void refreshTable();
    void loadData();
    void saveData();
    void updateStatusBar();
    void showAddDialog();
};

#endif // MAINWINDOW_H