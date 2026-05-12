#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include "propertymanager.h"
#include "realestate.h"
#include "commandprocessor.h"

class FileLoaderDialog;
struct ParseError;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void receiveLoadedData(const QVector<RealEstate>& data);

private slots:
    void on_addButton_clicked();
    void on_deleteButton_clicked();
    void on_refreshButton_clicked();
    void on_tableWidget_itemSelectionChanged();
    void on_openFileDialogButton_clicked();
    void on_processLoadedButton_clicked();
    void on_executeCommandsButton_clicked();
    void on_loadCommandsFileButton_clicked();


private:
    Ui::MainWindow *ui;
    PropertyManager manager_;
    QString currentFileName_;
    QVector<RealEstate> loadedFromFileData;

    void setupTable();
    void refreshTable();
    void loadData();
    void saveData();
    void updateStatusBar();
    void showAddDialog();
    void processLoadedFileData();
    void showErrorLog(const QVector<ParseError>& errors);
    QString currentCommandsFile_;
    class CommandProcessor* commandProcessor_;  // forward declaration

    void showCommandResults(const QVector<CommandResult>& results);

};

#endif // MAINWINDOW_H