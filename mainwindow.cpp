#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <iostream>
#include <fstream>
#include <string>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QRandomGenerator>
#include <QDebug>
#include <QPixmap>
#include <cpr/cpr.h>
#include <QJsonDocument>
#include <QJsonObject>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Стиль
    ui->taskListWidget->setStyleSheet(
        "QListWidget {"
        "   background-color: white;"
        "   border: 2px solid gray;"
        "   border-radius: 10px;"
        "   padding: 5px;"
        "   font-size: 14px;"
        "   color: black;"
        "}"
        "QListWidget::item {"
        "   background-color: #f0f0f0;"
        "   border: 1px solid lightgray;"
        "   border-radius: 5px;"
        "   padding: 8px;"
        "   margin: 4px;"
        "}"
        "QListWidget::item:hover {"
        "   background-color: #d9d9d9;"
        "}"
        "QListWidget::item:selected {"
        "   background-color: #c0c0c0;"
        "   color: black;"
        "   font-weight: bold;"
        "}"
        );

    //  QStackedWidget для перемикання між сторінками
    ui->stackedWidget->setCurrentWidget(ui->pageLogin);

    // Стилізація
    this->setStyleSheet(
        "QWidget { background-color: white; }"
        "QLabel { color: black; font-size: 14px; }"
        "QLineEdit {"
        "   background-color: white;"
        "   color: black;"
        "   border: 1px solid gray;"
        "   padding: 8px 12px;"
        "   border-radius: 15px;"
        "}"
        "QPushButton {"
        "   background-color: #e0e0e0;"
        "   color: black;"
        "   border: 1px solid gray;"
        "   padding: 5px;"
        "   border-radius: 10px;"
        "}"
        "QPushButton:hover { background-color: #d0d0d0; }"
        );


    // Завантаження зображення
    QPixmap pix(":/resources/img/14.jpg");

    int w = ui->img->width();
    int h = ui->img->height();
    ui->img->setPixmap(pix.scaled(w, h, Qt::KeepAspectRatio));

    int w2 = ui->img2->width();
    int h2 = ui->img2->height();
    ui->img2->setPixmap(pix.scaled(w2, h2, Qt::KeepAspectRatio));
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Вхід у систему
void MainWindow::on_ButtonLogin_clicked()
{
    QString login = ui->LogLine->text().trimmed();
    QString password = ui->PassLine->text().trimmed();

    cpr::Response r = cpr::Post(
        cpr::Url{"https://todo.mindenit.org/login?useCookies=false&useSessionCookies=false"},
        cpr::Header{
            {"accept", "application/json"},
            {"Content-Type", "application/json"}
        },
        cpr::Body{R"({"email": ")" + login.toStdString() + R"(", "password": ")" + password.toStdString() + R"("})"}
        );

    qDebug() << "Status Code (Login): " << r.status_code;
    qDebug() << "Response Body (Login): " << QString::fromStdString(r.text);

    if (r.status_code == 200) {
        QJsonParseError jsonError;
        QJsonDocument jsonResponse = QJsonDocument::fromJson(QString::fromStdString(r.text).toUtf8(), &jsonError);

        if (!jsonError.NoError) {
            if (jsonResponse.isObject()) {
                QJsonObject jsonObj = jsonResponse.object();
                if (jsonObj.contains("accessToken")) {
                    QString accessToken = jsonObj["accessToken"].toString();
                    // Збереження токена у файл
                    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
                    QFile tokenFile(desktopPath + "/token.txt");
                    if (tokenFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                        QTextStream out(&tokenFile);
                        out << accessToken;
                        tokenFile.close();
                        qDebug() << "Токен збережено у файл token.txt";
                    } else {
                        QMessageBox::warning(this, "Помилка", "Не вдалося зберегти токен у файл.");
                    }
                    QMessageBox::information(this, "Успіх", "Вхід виконано!");
                    clearLoginFields();
                    currentUserLogin = login;
                    currentUserID = ""; // ID користувача може потребувати іншого запиту
                    updateUserInfo();
                    ui->stackedWidget->setCurrentWidget(ui->pageMain);
                    loadTasksForUser();
                    return;
                } else {
                    QMessageBox::warning(this, "Помилка", "У відповіді сервера відсутній токен.");
                }
            } else {
                QMessageBox::warning(this, "Помилка", "Не вдалося обробити відповідь сервера (не є об'єктом JSON).");
            }
        } else {
            qDebug() << "Помилка парсингу JSON: " << jsonError.errorString();
            QMessageBox::warning(this, "Помилка", "Помилка парсингу JSON відповіді сервера: " + jsonError.errorString());
        }
    } else if (r.status_code == 401) {
        QMessageBox::warning(this, "Помилка", "Невірний логін або пароль!");
    } else {
        // Обробка інших помилок входу
        QString errorMessage = "Помилка входу!";
        QJsonParseError jsonError;
        QJsonDocument jsonResponse = QJsonDocument::fromJson(QString::fromStdString(r.text).toUtf8(), &jsonError);

        if (jsonError.NoError) {
            if (jsonResponse.isObject()) {
                QJsonObject jsonObj = jsonResponse.object();
                if (jsonObj.contains("detail")) {
                    errorMessage = jsonObj["detail"].toString();
                }
            }
        }
        QMessageBox::warning(this, "Помилка", errorMessage);
    }
}

// Перехід на сторінку реєстрації
void MainWindow::on_ButtonGoToRegister_clicked()
{
    clearLoginFields();  // Очищення полів входу при переході на реєстрацію
    ui->stackedWidget->setCurrentWidget(ui->pageRegister);
}

// Повернення на сторінку входу
void MainWindow::on_ButtonReturnToLogin_clicked()
{
    clearRegisterFields();  // Очищення полів реєстрації при поверненні на вхід
    ui->stackedWidget->setCurrentWidget(ui->pageLogin);
}

// Реєстрація нового користувача
void MainWindow::on_ButtonRegister_clicked()
{
    QString login = ui->inLogin->text().trimmed();
    QString password = ui->inPassword->text();
    QString passwordCheck = ui->inPasswordCheck->text();

    if (login.isEmpty() || password.isEmpty() || passwordCheck.isEmpty()) {
        QMessageBox::warning(this, "Помилка", "Заповніть всі поля!");
        return;
    }

    if (password != passwordCheck) {
        QMessageBox::warning(this, "Помилка", "Паролі не співпадають!");
        return;
    }

    // Виконуємо POST-запит на сервер для реєстрації
    cpr::Response r = cpr::Post(
        cpr::Url{"https://todo.mindenit.org/register"},
        cpr::Header{
            {"accept", "application/problem+json"},
            {"Content-Type", "application/json"}
        },
        cpr::Body{R"({"email": ")" + login.toStdString() + R"(", "password": ")" + password.toStdString() + R"("})"}
        );

    qDebug() << "Status Code (Register): " << r.status_code;
    qDebug() << "Response Body (Register): " << QString::fromStdString(r.text);

    if (r.status_code == 200) { // Перевіряємо код успішної реєстрації (тепер 200)
        QMessageBox::information(this, "Успіх", "Реєстрація успішна!");
        clearRegisterFields();  // Очищення полів реєстрації після успішної реєстрації
        ui->stackedWidget->setCurrentWidget(ui->pageLogin);
    } else {
        // Обробка помилок реєстрації
        QString errorMessage = "Помилка реєстрації!";
        QJsonParseError jsonError;
        QJsonDocument jsonResponse = QJsonDocument::fromJson(QString::fromStdString(r.text).toUtf8(), &jsonError);

        if (jsonError.NoError) {
            if (jsonResponse.isObject()) {
                QJsonObject jsonObj = jsonResponse.object();
                if (jsonObj.contains("detail")) {
                    errorMessage = jsonObj["detail"].toString();
                }
            }
        }
        QMessageBox::warning(this, "Помилка", errorMessage);
    }
}

// Очищення полів входу
void MainWindow::clearLoginFields()
{
    ui->LogLine->clear();
    ui->PassLine->clear();
}

// Перевірка введення пароля
void MainWindow::on_inPasswordCheck_textChanged(const QString &text)
{
    if (text != ui->inPassword->text()) {
        ui->inPasswordCheck->setStyleSheet("border: 2px solid red;");
    } else {
        ui->inPasswordCheck->setStyleSheet("");
    }
}

// Очищення полів реєстрації
void MainWindow::clearRegisterFields()
{
    ui->inLogin->clear();
    ui->inPassword->clear();
    ui->inPasswordCheck->clear();
}

void MainWindow::updateUserInfo()
{
    ui->labelUsername->setText("Логін: " + currentUserLogin);
    ui->labelUserID->setText("ID: " + currentUserID);
}

void MainWindow::on_ButtonLogout_clicked()
{
    clearLoginFields(); // Очищаємо поля входу
    ui->stackedWidget->setCurrentWidget(ui->pageLogin); // Повертаємось на сторінку входу
}

void MainWindow::on_ButtonNewTask_clicked()
{
    if (currentUserID.isEmpty()) {
        QMessageBox::warning(this, "Помилка", "Помилка отримання ID користувача!");
        return;
    }

    window = new TaskWindow(this, currentUserID);
    window->show();
}

void MainWindow::loadTasksForUser()
{
    ui->taskListWidget->clear();

    QFile file(getTasksFilePath());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Помилка", "Не вдалося відкрити файл задач!");
        return;
    }

    QTextStream in(&file);
    QString currentTask, taskName;
    bool isUserTask = false;
    QString taskUserID, taskID;

    qDebug() << "=== Завантаження задач для користувача ID:" << currentUserID << "===";

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        qDebug() << "Читаємо з файлу: " << line;

        if (line.startsWith("ID користувача:")) {
            taskUserID = line.section(": ", 1, 1).trimmed();
            isUserTask = (taskUserID == currentUserID);
        }

        if (isUserTask) {
            currentTask += line + "\n";

            if (line.startsWith("ID задачі:")) {
                taskID = line.section(": ", 1, 1).trimmed();
            }

            if (line.startsWith("Назва:")) {
                taskName = line.section(": ", 1, 1).trimmed();
            }

            if (line == "----------------------") {
                qDebug() << "==> Додаємо задачу: " << taskName << " (ID: " << taskID << ")";
                QListWidgetItem *taskItem = new QListWidgetItem(taskName, ui->taskListWidget);
                taskItem->setData(Qt::UserRole, taskID);
                ui->taskListWidget->addItem(taskItem);

                currentTask.clear();
            }
        }
    }

    file.close();
}

void MainWindow::on_refreshTasksButton_clicked()
{
    loadTasksForUser();
}

void MainWindow::on_taskListWidget_itemClicked(QListWidgetItem *item)
{
    QString taskID = item->data(Qt::UserRole).toString();
    TaskWindow *taskWindow = new TaskWindow(this, currentUserID, taskID);
    taskWindow->show();
}

QString MainWindow::getTasksFilePath()
{
    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    return desktopPath + "/tasks.txt";
}
