#ifndef FILELOADERDIALOG_H
#define FILELOADERDIALOG_H

#include <QDialog>
#include <QVector>
#include <QTextBrowser>
#include <QPushButton>
#include <QVBoxLayout>
#include "realestate.h"

class FileLoaderDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FileLoaderDialog(QWidget *parent = nullptr);
    ~FileLoaderDialog();

signals:
    void dataLoaded(const QVector<RealEstate>& data);

private slots:
    void on_openButton_clicked();
    void on_processButton_clicked();

private:
    QTextBrowser* textBrowser;
    QPushButton* openButton;
    QPushButton* processButton;

    QVector<RealEstate> loadedData;
    QString currentFileName;

    void saveErrorToLog(const QString& errorMsg, const QString& line, int lineNumber);
};

#endif // FILELOADERDIALOG_H