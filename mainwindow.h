#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QRandomGenerator>
#include <qlistwidget.h>
#include "taskwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_ButtonLogin_clicked();
    void on_ButtonGoToRegister_clicked();
    void on_ButtonReturnToLogin_clicked();
    void on_inPasswordCheck_textChanged(const QString &text);
    void on_ButtonRegister_clicked();
    void on_ButtonLogout_clicked();
    void on_ButtonNewTask_clicked();
    void on_refreshTasksButton_clicked();
    void on_taskListWidget_itemClicked(QListWidgetItem *item);


private:
    Ui::MainWindow *ui;
    QString generateUniqueID();
    bool isUserDuplicate(const QString &login, const QString &password);
    void saveUserToFile(const QString &id, const QString &login, const QString &password);
    QString getFilePath();
    void clearLoginFields();
    void clearRegisterFields();
    QString currentUserLogin;
    QString currentUserID;
    void updateUserInfo();
    TaskWindow *window;
    void loadTasksForUser();
    QString getTasksFilePath();


};

#endif // MAINWINDOW_H
