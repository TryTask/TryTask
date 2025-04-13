#include "mainwindow.h"
#include "./ui_mainwindow.h" // Використовуємо ./ для відносної вказівки
#include "taskwindow.h"     // Підключаємо визначення TaskWindow

#include <QMessageBox>
#include <QPixmap>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
// #include <QRandomGenerator> // Більше не потрібен тут
#include <QDebug>
#include <cpr/cpr.h>      // Бібліотека для HTTP запитів
#include <QJsonDocument>  // Для роботи з JSON
#include <QJsonObject>
#include <QJsonArray>
#include <QListWidgetItem> // Для роботи з елементами списку
#include <QDir>          // Для роботи з директоріями

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , bearerToken("")
    , currentUserID("")
    , currentUserLogin("")
{
    ui->setupUi(this);

    // --- Стилі ---
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
        "QListWidget::item:hover { background-color: #d9d9d9; }"
        "QListWidget::item:selected { background-color: #c0c0c0; color: black; font-weight: bold; }"
        );

    this->setStyleSheet(
        "QWidget { background-color: white; }"
        "QLabel { color: black; font-size: 14px; }"
        "QLineEdit {"
        "   background-color: white; color: black; border: 1px solid gray;"
        "   padding: 8px 12px; border-radius: 15px;"
        "}"
        "QPushButton {"
        "   background-color: #e0e0e0; color: black; border: 1px solid gray;"
        "   padding: 5px; border-radius: 10px;"
        "}"
        "QPushButton:hover { background-color: #d0d0d0; }"
        );
    // --------------------------

    // --- Завантаження зображення ---
    QPixmap pix(":/resources/img/14.jpg");
    if(!pix.isNull()) {
        int w = ui->img->width(); int h = ui->img->height();
        ui->img->setPixmap(pix.scaled(w, h, Qt::KeepAspectRatio));
        int w2 = ui->img2->width(); int h2 = ui->img2->height();
        ui->img2->setPixmap(pix.scaled(w2, h2, Qt::KeepAspectRatio));
    } else {
         qWarning() << "Could not load image ':/resources/img/14.jpg'";
    }
    // ---------------------------------------

    // --- Завантаження сесії ---
    loadUserInfoFromFile();
    if (!bearerToken.isEmpty() && !currentUserID.isEmpty()) {
        qDebug() << "User info and token loaded from file.";
        updateUserInfo();
        ui->stackedWidget->setCurrentWidget(ui->pageMain);
        loadTasksForUser();
    } else {
        qDebug() << "No saved user session found. Showing Login page.";
        ui->stackedWidget->setCurrentWidget(ui->pageLogin);
    }
    // --------------------------

     // Підключаємо сигнал подвійного кліку
     connect(ui->taskListWidget, &QListWidget::itemDoubleClicked, this, &MainWindow::on_taskListWidget_itemDoubleClicked);
}

MainWindow::~MainWindow() {
    delete ui;
}

// Допоміжна функція для заголовка авторизації
cpr::Header MainWindow::getAuthHeader() {
     if (bearerToken.isEmpty()) { qWarning() << "Empty token!"; return {}; }
    return cpr::Header{{"Authorization", "Bearer " + bearerToken.toStdString()}};
}


// Логін
void MainWindow::on_ButtonLogin_clicked() {
    QString login = ui->LogLine->text().trimmed();
    QString password = ui->PassLine->text().trimmed();
    if (login.isEmpty() || password.isEmpty()) { QMessageBox::warning(this, "Помилка", "Введіть логін та пароль."); return; }

    // Запит на логін
    cpr::Response r = cpr::Post(
        cpr::Url{"https://todo.mindenit.org/login?useCookies=false&useSessionCookies=false"},
        cpr::Header{{"accept", "application/json"},{"Content-Type", "application/json"}},
        cpr::Body{R"({"email": ")" + login.toStdString() + R"(", "password": ")" + password.toStdString() + R"("})"}
    );

    qDebug() << "Login Status Code: " << r.status_code;
    qDebug() << "Login Response Body: " << QString::fromStdString(r.text); // Логуємо відповідь

    if (r.status_code == 200) {
        QJsonParseError jsonError;
        QJsonDocument jsonResponse = QJsonDocument::fromJson(QString::fromStdString(r.text).toUtf8(), &jsonError);

        if (jsonError.error == QJsonParseError::NoError && jsonResponse.isObject()) {
            QJsonObject jsonObj = jsonResponse.object();
            if (jsonObj.contains("accessToken")) {
                bearerToken = jsonObj["accessToken"].toString();
                qDebug() << "Access Token received:" << bearerToken;

                // Запит на отримання інформації про користувача
                cpr::Response userResponse = cpr::Get(
                    cpr::Url{"https://todo.mindenit.org/user"},
                    getAuthHeader()
                );

                qDebug() << "Get User Status Code: " << userResponse.status_code;
                qDebug() << "Get User Response Body: " << QString::fromStdString(userResponse.text); // Логуємо відповідь

                if (userResponse.status_code == 200) {
                    QJsonParseError userJsonError;
                    QJsonDocument userJsonResponse = QJsonDocument::fromJson(QString::fromStdString(userResponse.text).toUtf8(), &userJsonError);

                    if (userJsonError.error == QJsonParseError::NoError && userJsonResponse.isObject()) {
                        QJsonObject userObject = userJsonResponse.object();

                        // --- ВАЖЛИВО: Перевірте тут правильний ключ! ---
                        const QString idKey = "Id"; // Замініть на "Id" якщо потрібно!

                        if (userObject.contains(idKey)) {
                            currentUserID = userObject[idKey].toString();
                            currentUserLogin = login;
                            qDebug() << "User ID received:" << currentUserID;

                            saveUserInfoToFile();
                            QMessageBox::information(this, "Успіх", "Вхід виконано!");
                            clearLoginFields();
                            updateUserInfo();
                            ui->stackedWidget->setCurrentWidget(ui->pageMain);
                            loadTasksForUser();
                            return; // Успішний вихід
                        } else {
                             qDebug() << "User info response does not contain the key '" << idKey << "'.";
                             qDebug() << "Available keys in user object:" << userObject.keys(); // Дивимось ключі
                             QMessageBox::warning(this, "Помилка", "У відповіді сервера відсутній ID користувача ('" + idKey + "').");
                        }
                    } else {
                         qDebug() << "Failed to parse Get User JSON response:" << userJsonError.errorString();
                         QMessageBox::warning(this, "Помилка", "Помилка обробки інформації про користувача.");
                    }
                } else {
                    qDebug() << "Failed to get user info. Status code:" << userResponse.status_code;
                     QMessageBox::warning(this, "Помилка", "Не вдалося отримати інформацію про користувача після входу. Код: " + QString::number(userResponse.status_code));
                }
                bearerToken = ""; // Скидаємо токен, якщо не вдалося отримати ID
                saveUserInfoToFile();

            } else {
                 qDebug() << "Login response does not contain 'accessToken'.";
                 QMessageBox::warning(this, "Помилка", "У відповіді сервера відсутній токен доступу ('accessToken').");
            }
        } else {
             qDebug() << "Failed to parse Login JSON response:" << jsonError.errorString();
             QMessageBox::warning(this, "Помилка", "Помилка обробки відповіді сервера.");
        }
    } else if (r.status_code == 400 || r.status_code == 401 ) {
        QJsonParseError jsonError;
        QJsonDocument jsonResponse = QJsonDocument::fromJson(QString::fromStdString(r.text).toUtf8(), &jsonError);
        QString errorDetail = "Невірний логін або пароль!";
         if (jsonError.error == QJsonParseError::NoError && jsonResponse.isObject()) {
             QJsonObject jsonObj = jsonResponse.object();
             if (jsonObj.contains("detail")) { errorDetail = jsonObj["detail"].toString(); }
             else if (jsonObj.contains("errors")) { errorDetail = "Помилка валідації даних."; }
         }
         QMessageBox::warning(this, "Помилка входу", errorDetail);
    } else {
        qDebug() << "Unhandled login error. Status:" << r.status_code;
        QMessageBox::warning(this, "Помилка", "Невідома помилка сервера при спробі входу. Код: " + QString::number(r.status_code));
    }
}

// Перехід на реєстрацію
void MainWindow::on_ButtonGoToRegister_clicked() { clearLoginFields(); ui->stackedWidget->setCurrentWidget(ui->pageRegister); }
// Повернення на логін
void MainWindow::on_ButtonReturnToLogin_clicked() { clearRegisterFields(); ui->stackedWidget->setCurrentWidget(ui->pageLogin); }

// Реєстрація
void MainWindow::on_ButtonRegister_clicked() {
    QString login = ui->inLogin->text().trimmed(); QString password = ui->inPassword->text(); QString passwordCheck = ui->inPasswordCheck->text();
    if (login.isEmpty() || password.isEmpty() || passwordCheck.isEmpty()) { QMessageBox::warning(this, "Помилка", "Заповніть всі поля!"); return; }
    if (password != passwordCheck) { QMessageBox::warning(this, "Помилка", "Паролі не співпадають!"); return; }
    if (password.length() < 6) { QMessageBox::warning(this, "Помилка", "Пароль мін 6 символів."); return; }

    cpr::Response r = cpr::Post(
        cpr::Url{"https://todo.mindenit.org/register"},
        cpr::Header{{"accept", "*/*"}, {"Content-Type", "application/json"}},
        cpr::Body{R"({"email": ")" + login.toStdString() + R"(", "password": ")" + password.toStdString() + R"("})"}
    );
    qDebug() << "Register Status Code: " << r.status_code;
    qDebug() << "Register Response Body: " << QString::fromStdString(r.text); // Логуємо відповідь

    if (r.status_code == 200) {
        QMessageBox::information(this, "Успіх", "Реєстрація успішна! Тепер ви можете увійти.");
        clearRegisterFields();
        ui->stackedWidget->setCurrentWidget(ui->pageLogin);
    } else {
        QString errorMessage = "Помилка реєстрації!";
        QJsonParseError jsonError; QJsonDocument jsonResponse = QJsonDocument::fromJson(QString::fromStdString(r.text).toUtf8(), &jsonError);
        if (jsonError.error == QJsonParseError::NoError && jsonResponse.isObject()) {
            QJsonObject jsonObj = jsonResponse.object();
            if (jsonObj.contains("errors") && jsonObj["errors"].isObject()) {
                 QJsonObject errorsObj = jsonObj["errors"].toObject(); QStringList errorMessages;
                 for(auto it = errorsObj.begin(); it != errorsObj.end(); ++it) { if (it.value().isArray()) { for(const auto& errVal : it.value().toArray()) { errorMessages.append(errVal.toString()); } } }
                 if (!errorMessages.isEmpty()) { errorMessage = errorMessages.join("\n"); }
            } else if (jsonObj.contains("title")) { errorMessage = jsonObj["title"].toString(); if (jsonObj.contains("detail")) { errorMessage += "\n" + jsonObj["detail"].toString(); } }
        } else { errorMessage = "Помилка сервера. Код: " + QString::number(r.status_code) + ". " + QString::fromStdString(r.reason); }
        QMessageBox::warning(this, "Помилка реєстрації", errorMessage);
    }
}

// Очищення полів
void MainWindow::clearLoginFields() { ui->LogLine->clear(); ui->PassLine->clear(); }
void MainWindow::clearRegisterFields() { ui->inLogin->clear(); ui->inPassword->clear(); ui->inPasswordCheck->clear(); }
// Перевірка паролів
void MainWindow::on_inPasswordCheck_textChanged(const QString &text) { ui->inPasswordCheck->setStyleSheet(text != ui->inPassword->text() ? "border: 1px solid red;" : ""); }
// Оновлення UI
void MainWindow::updateUserInfo() { ui->labelUsername->setText("Користувач: " + (currentUserLogin.isEmpty() ? "N/A" : currentUserLogin)); ui->labelUserID->setVisible(false); }
// Вихід
void MainWindow::on_ButtonLogout_clicked() { bearerToken = ""; currentUserID = ""; currentUserLogin = ""; saveUserInfoToFile(); ui->taskListWidget->clear(); clearLoginFields(); ui->stackedWidget->setCurrentWidget(ui->pageLogin); qDebug() << "User logged out."; }

// Нова задача
void MainWindow::on_ButtonNewTask_clicked() {
    if (bearerToken.isEmpty()) { QMessageBox::warning(this, "Помилка", "Потрібно увійти..."); return; }
    TaskWindow *newTaskWindow = new TaskWindow(bearerToken, "", this);
    newTaskWindow->setAttribute(Qt::WA_DeleteOnClose);
    connect(newTaskWindow, &TaskWindow::taskSavedSuccessfully, this, &MainWindow::loadTasksForUser); // Підключаємо сигнал
    newTaskWindow->show();
}

// Завантаження задач
void MainWindow::loadTasksForUser() {
    ui->taskListWidget->clear();
    if (bearerToken.isEmpty()) { qDebug() << "Token empty"; return; }
    qDebug() << "Loading tasks...";
    cpr::Response r = cpr::Get( cpr::Url{"https://todo.mindenit.org/items"}, getAuthHeader() );
    qDebug() << "Load Items Status Code: " << r.status_code;
    qDebug() << "Load Items Response Body: " << QString::fromStdString(r.text); // Логуємо відповідь

    if (r.status_code == 200) {
        QJsonParseError jsonError; QJsonDocument jsonResponse = QJsonDocument::fromJson(QString::fromStdString(r.text).toUtf8(), &jsonError);
        if (jsonError.error == QJsonParseError::NoError && jsonResponse.isArray()) {
            QJsonArray itemsArray = jsonResponse.array();
            if (itemsArray.isEmpty()) { qDebug() << "No tasks found"; }
            else { qDebug() << "Received" << itemsArray.size() << "tasks.";
                for (const QJsonValue &itemValue : itemsArray) {
                     if (itemValue.isObject()) { QJsonObject itemObject = itemValue.toObject();
                         if (itemObject.contains("Id") && itemObject.contains("Name")) { // Перевіряємо Id та Name
                             int itemId = itemObject["Id"].toInt(); QString taskName = itemObject["Name"].toString(); QString taskDescription = itemObject["Description"].toString();
                             QListWidgetItem *taskItem = new QListWidgetItem(taskName, ui->taskListWidget); taskItem->setData(Qt::UserRole, itemId); taskItem->setToolTip(taskDescription); ui->taskListWidget->addItem(taskItem);
                         } else { qWarning() << "Task obj missing Id/Name"; }
                     } else { qWarning() << "Item not object"; }
                }
            }
        } else { QMessageBox::warning(this, "Помилка", "Не вдалося обробити список задач."); }
    } else if (r.status_code == 401) { QMessageBox::warning(this, "Помилка сесії", "Сесія недійсна."); on_ButtonLogout_clicked(); }
    else if (r.status_code == 404) { qDebug() << "No tasks (404)"; }
    else { QMessageBox::warning(this, "Помилка", "Не вдалося завантажити задачі. Код: " + QString::number(r.status_code)); }
}

// Оновити список
void MainWindow::on_refreshTasksButton_clicked() { loadTasksForUser(); }

// Редагувати задачу (подвійний клік)
void MainWindow::on_taskListWidget_itemDoubleClicked(QListWidgetItem *item) {
     if (!item) return;
     if (bearerToken.isEmpty()) { QMessageBox::warning(this, "Помилка", "Потрібно увійти..."); return; }
     int taskIDInt = item->data(Qt::UserRole).toInt(); QString taskIDStr = QString::number(taskIDInt);
     qDebug() << "Editing task with ID:" << taskIDStr;
     TaskWindow *editTaskWindow = new TaskWindow(bearerToken, taskIDStr, this);
     editTaskWindow->setAttribute(Qt::WA_DeleteOnClose);
     connect(editTaskWindow, &TaskWindow::taskSavedSuccessfully, this, &MainWindow::loadTasksForUser); // Підключаємо сигнал
     editTaskWindow->show();
}

// Збереження сесії
void MainWindow::saveUserInfoToFile() {
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir configDir(configPath);
    if (!configDir.exists()) { if (!configDir.mkpath(".")) { qWarning() << "Could not create config dir:" << configPath; QMessageBox::warning(this, "Помилка", "Не вдалося створити директорію."); return; } }
    QString filePath = configPath + "/userinfo.dat";
    QFile userInfoFile(filePath);
    if (userInfoFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&userInfoFile); out << "Token=" << bearerToken << "\n"; out << "UserID=" << currentUserID << "\n"; out << "UserLogin=" << currentUserLogin << "\n"; userInfoFile.close(); qDebug() << "User info saved to:" << filePath;
    } else { qWarning() << "Failed to save user info to:" << filePath << "Error:" << userInfoFile.errorString(); QMessageBox::warning(this, "Помилка", "Не вдалося зберегти сесію."); }
}

// Завантаження сесії
void MainWindow::loadUserInfoFromFile() {
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QString filePath = configPath + "/userinfo.dat";
    QFile userInfoFile(filePath);
    if (userInfoFile.exists() && userInfoFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&userInfoFile); bearerToken = ""; currentUserID = ""; currentUserLogin = "";
        while (!in.atEnd()) { QString line = in.readLine().trimmed(); if (line.startsWith("Token=")) { bearerToken = line.mid(6); } else if (line.startsWith("UserID=")) { currentUserID = line.mid(7); } else if (line.startsWith("UserLogin=")) { currentUserLogin = line.mid(10); } }
        userInfoFile.close(); if (!bearerToken.isEmpty() && !currentUserID.isEmpty()) { qDebug() << "User info loaded"; } else { qDebug() << "Token/UserID missing"; bearerToken = ""; currentUserID = ""; currentUserLogin = ""; }
    } else { qDebug() << "User info file not found:" << filePath; }
}
