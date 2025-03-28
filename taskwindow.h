#ifndef TASKWINDOW_H
#define TASKWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QRandomGenerator>

QT_BEGIN_NAMESPACE
namespace Ui { class TaskWindow; }
QT_END_NAMESPACE

class TaskWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TaskWindow(QWidget *parent = nullptr, QString userID = "", QString taskID = "");
    ~TaskWindow();

    QString getUserID() const { return userID; }

private slots:
    void on_save_clicked();
    void on_BackToMain_clicked();

private:
    Ui::TaskWindow *ui;
    QString userID;
    bool isModified;

    QString generateTaskID();
    bool isTaskNameUnique(const QString &taskName);
    QString getFilePath();
    void clearFields();
    QString taskID;
    void loadTaskData();
};

#endif // TASKWINDOW_H
