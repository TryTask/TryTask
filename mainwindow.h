#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qlistwidget.h>
#include "taskwindow.h" // Підключаємо оновлений taskwindow.h
#include <cpr/cpr.h>    // Додаємо для типу cpr::Header

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
    void on_taskListWidget_itemDoubleClicked(QListWidgetItem *item); // Змінено на DoubleClicked для редагування
    void loadTasksForUser(); // Слот для оновлення після збереження задачі

private:
    Ui::MainWindow *ui;
    QString bearerToken;    // Токен доступу
    QString currentUserID;  // ID користувача (GUID з API)
    QString currentUserLogin; // Email користувача

    void clearLoginFields();
    void clearRegisterFields();
    void updateUserInfo(); // Оновлює мітки з інформацією про користувача

    // Методи для збереження/завантаження сесії (токена)
    void saveUserInfoToFile();
    void loadUserInfoFromFile();

    // Допоміжна функція для створення заголовків авторизації
    cpr::Header getAuthHeader();
};

#endif // MAINWINDOW_H
