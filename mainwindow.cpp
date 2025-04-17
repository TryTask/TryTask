#include "mainwindow.h"
#include "./ui_mainwindow.h" // Використовуємо ./ для відносної вказівки
#include "taskwindow.h"      // Підключаємо визначення TaskWindow

#include <QMessageBox>
#include <QPixmap>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QDebug>
#include <cpr/cpr.h>       // Бібліотека для HTTP запитів
#include <QJsonDocument>   // Для роботи з JSON
#include <QJsonObject>
#include <QJsonArray>
#include <QListWidgetItem> // Для роботи з елементами списку
#include <QDir>            // Для роботи з директоріями
#include <QDateTime>       // Для роботи з датою та часом

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
        loadTasksForUser(); // Завантажуємо завдання
    } else {
        qDebug() << "No saved user session found. Showing Login page.";
        ui->stackedWidget->setCurrentWidget(ui->pageLogin);
    }
    // --------------------------

    // --- Початковий стан кнопок ---
    ui->deleteTaskButton->setEnabled(false); // ❗ Вимикаємо кнопку видалення спочатку
    ui->EditButton->setEnabled(false);       // ❗ Вимикаємо кнопку редагування спочатку
    // -----------------------------

    // Підключаємо сигнал кліку на елемент списку для активації кнопок
    connect(ui->taskListWidget, &QListWidget::itemClicked, this, [=](QListWidgetItem *item) {
        bool itemSelected = (item != nullptr);
        ui->deleteTaskButton->setEnabled(itemSelected);
        ui->EditButton->setEnabled(itemSelected); // Активуємо/деактивуємо кнопку редагування
    });

    // Підключаємо сигнал зміни поточного елемента (для випадків, коли вибір зникає)
    connect(ui->taskListWidget, &QListWidget::currentItemChanged, this, [=](QListWidgetItem *current, QListWidgetItem *) {
        bool itemSelected = (current != nullptr);
        ui->deleteTaskButton->setEnabled(itemSelected);
        ui->EditButton->setEnabled(itemSelected); // Оновлюємо стан кнопки редагування
    });
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
                        // Спробуємо обидва варіанти ключа 'Id' та 'id'
                        QString idKey = "";
                        if (userObject.contains("Id")) {
                            idKey = "Id";
                        } else if (userObject.contains("id")) {
                            idKey = "id";
                        }

                        if (!idKey.isEmpty()) {
                            currentUserID = userObject[idKey].toString();
                            currentUserLogin = login; // Зберігаємо логін
                            qDebug() << "User ID received:" << currentUserID;

                            saveUserInfoToFile();
                            QMessageBox::information(this, "Успіх", "Вхід виконано!");
                            clearLoginFields();
                            updateUserInfo();
                            ui->stackedWidget->setCurrentWidget(ui->pageMain);
                            loadTasksForUser(); // Завантажуємо завдання після успішного входу
                            return; // Успішний вихід
                        } else {
                            qDebug() << "User info response does not contain the key 'Id' or 'id'.";
                            qDebug() << "Available keys in user object:" << userObject.keys(); // Дивимось ключі
                            QMessageBox::warning(this, "Помилка", "У відповіді сервера відсутній ID користувача ('Id' або 'id').");
                        }
                    } else {
                        qDebug() << "Failed to parse Get User JSON response:" << userJsonError.errorString();
                        QMessageBox::warning(this, "Помилка", "Помилка обробки інформації про користувача.");
                    }
                } else {
                    qDebug() << "Failed to get user info. Status code:" << userResponse.status_code;
                    QMessageBox::warning(this, "Помилка", "Не вдалося отримати інформацію про користувача після входу. Код: " + QString::number(userResponse.status_code));
                }
                // Якщо не вдалося отримати ID, скидаємо токен
                if (currentUserID.isEmpty()) {
                    bearerToken = "";
                    saveUserInfoToFile();
                }

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
void MainWindow::on_ButtonLogout_clicked() {
    bearerToken = ""; currentUserID = ""; currentUserLogin = "";
    saveUserInfoToFile();
    ui->taskListWidget->clear();
    ui->deleteTaskButton->setEnabled(false); // ❗ Вимикаємо кнопку видалення при виході
    ui->EditButton->setEnabled(false);       // ❗ Вимикаємо кнопку редагування при виході
    clearLoginFields();
    ui->stackedWidget->setCurrentWidget(ui->pageLogin);
    qDebug() << "User logged out.";
}

// Нова задача
void MainWindow::on_ButtonNewTask_clicked() {
    if (bearerToken.isEmpty()) { QMessageBox::warning(this, "Помилка", "Потрібно увійти..."); return; }
    // Створюємо вікно TaskWindow для НОВОГО завдання (taskId = 0 або інше значення за замовчуванням)
    TaskWindow *newTaskWindow = new TaskWindow(bearerToken, 0, this); // Передаємо токен і ID=0
    newTaskWindow->setAttribute(Qt::WA_DeleteOnClose);
    // Підключаємо сигнал для оновлення списку після збереження нової задачі
    connect(newTaskWindow, &TaskWindow::taskSavedSuccessfully, this, &MainWindow::loadTasksForUser);
    newTaskWindow->setWindowTitle("Нове завдання"); // Встановлюємо заголовок
    newTaskWindow->show();
}


// Завантаження задач
void MainWindow::loadTasksForUser() {
    ui->taskListWidget->clear(); // Очищаємо список перед завантаженням
    ui->deleteTaskButton->setEnabled(false); // ❗ Вимикаємо кнопки перед завантаженням
    ui->EditButton->setEnabled(false);       // ❗

    if (bearerToken.isEmpty()) {
        qDebug() << "Token is empty, cannot load tasks.";
        return;
    }

    qDebug() << "Loading tasks for user...";

    cpr::Response r = cpr::Get(cpr::Url{"https://todo.mindenit.org/items"}, getAuthHeader());

    qDebug() << "Load Items Status Code: " << r.status_code;
    // qDebug() << "Load Items Response Body: " << QString::fromStdString(r.text); // Розкоментуйте для детального логування

    if (r.status_code == 200) {
        QJsonParseError jsonError;
        QJsonDocument jsonResponse = QJsonDocument::fromJson(QString::fromStdString(r.text).toUtf8(), &jsonError);

        if (jsonError.error == QJsonParseError::NoError && jsonResponse.isArray()) {
            QJsonArray itemsArray = jsonResponse.array();

            if (itemsArray.isEmpty()) {
                qDebug() << "No tasks found for the user.";
            } else {
                qDebug() << "Received" << itemsArray.size() << "tasks.";

                for (const QJsonValue &itemValue : itemsArray) {
                    if (itemValue.isObject()) {
                        QJsonObject itemObject = itemValue.toObject();

                        // --- ВАЖЛИВО: Перевірте тут правильний ключ ID! ---
                        QString idKey = "";
                        if (itemObject.contains("Id")) { idKey = "Id"; }
                        else if (itemObject.contains("id")) { idKey = "id"; }

                        // --- ВАЖЛИВО: Перевірте тут правильний ключ Назви! ---
                        QString nameKey = "";
                        if (itemObject.contains("Name")) { nameKey = "Name"; }
                        else if (itemObject.contains("name")) { nameKey = "name"; }


                        if (!idKey.isEmpty() && !nameKey.isEmpty()) {
                            int itemId = itemObject[idKey].toInt(); // Використовуємо знайдений ключ ID
                            QString taskName = itemObject[nameKey].toString(); // Використовуємо знайдений ключ Назви
                            QString taskDescription = itemObject.contains("Description") ? itemObject["Description"].toString() : "";
                            int categoryId = itemObject.contains("CategoryId") ? itemObject["CategoryId"].toInt() : 0;
                            int priority = itemObject.contains("Priority") ? itemObject["Priority"].toInt() : 0;
                            int aprxHours = itemObject.contains("AprxHours") ? itemObject["AprxHours"].toInt() : 0;
                            QString endedAt = itemObject.contains("EndedAt") ? itemObject["EndedAt"].toString() : "";
                                // Додайте отримання IsLooped, якщо воно є в JSON відповіді
                            bool isLooped = itemObject.contains("IsLooped") ? itemObject["IsLooped"].toBool() : false;

                            QListWidgetItem *taskItem = new QListWidgetItem(taskName, ui->taskListWidget);
                            taskItem->setData(Qt::UserRole, itemId);             // 0: ID задачі
                            taskItem->setData(Qt::UserRole + 1, taskDescription); // 1: Опис
                            taskItem->setData(Qt::UserRole + 2, categoryId);      // 2: Категорія ID
                            taskItem->setData(Qt::UserRole + 3, priority);        // 3: Пріоритет
                            taskItem->setData(Qt::UserRole + 4, aprxHours);       // 4: Орієнтовний час
                            taskItem->setData(Qt::UserRole + 5, endedAt);         // 5: Дата завершення (рядок ISO)
                            taskItem->setData(Qt::UserRole + 6, isLooped);        // 6: Чи зациклена

                            taskItem->setToolTip(taskDescription); // Тултип для зручності

                            ui->taskListWidget->addItem(taskItem);
                        } else {
                            qWarning() << "Task object is missing 'Id'/'id' or 'Name'/'name' key.";
                            qWarning() << "Task object keys:" << itemObject.keys();
                        }
                    } else {
                        qWarning() << "Item in tasks array is not a JSON object.";
                    }
                }
            }
        } else {
            qDebug() << "Failed to parse Load Items JSON response:" << jsonError.errorString();
            QMessageBox::warning(this, "Помилка", "Не вдалося обробити список задач. Формат відповіді сервера невірний.");
        }
    } else if (r.status_code == 401) {
        QMessageBox::warning(this, "Помилка сесії", "Ваша сесія більше недійсна. Будь ласка, увійдіть знову.");
        on_ButtonLogout_clicked(); // Виконуємо вихід користувача
    } else if (r.status_code == 404) {
        qDebug() << "No tasks found (API returned 404).";
        // Це не помилка, просто немає завдань
    } else {
        qDebug() << "Failed to load tasks. Status code:" << r.status_code;
        QMessageBox::warning(this, "Помилка", "Не вдалося завантажити задачі. Код помилки сервера: " + QString::number(r.status_code));
    }
}


// Оновити список
void MainWindow::on_refreshTasksButton_clicked() {
    loadTasksForUser();
}

// Збереження сесії
void MainWindow::saveUserInfoToFile() {
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir configDir(configPath);
    if (!configDir.exists()) { if (!configDir.mkpath(".")) { qWarning() << "Could not create config dir:" << configPath; QMessageBox::warning(this, "Помилка", "Не вдалося створити директорію конфігурації."); return; } }
    QString filePath = configPath + "/userinfo.dat";
    QFile userInfoFile(filePath);
    if (userInfoFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&userInfoFile); out << "Token=" << bearerToken << "\n"; out << "UserID=" << currentUserID << "\n"; out << "UserLogin=" << currentUserLogin << "\n"; userInfoFile.close(); qDebug() << "User info saved to:" << filePath;
    } else { qWarning() << "Failed to save user info to:" << filePath << "Error:" << userInfoFile.errorString(); QMessageBox::warning(this, "Помилка", "Не вдалося зберегти дані сесії."); }
}

// Завантаження сесії
void MainWindow::loadUserInfoFromFile() {
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QString filePath = configPath + "/userinfo.dat";
    QFile userInfoFile(filePath);
    if (userInfoFile.exists() && userInfoFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&userInfoFile); bearerToken = ""; currentUserID = ""; currentUserLogin = "";
        while (!in.atEnd()) { QString line = in.readLine().trimmed(); if (line.startsWith("Token=")) { bearerToken = line.mid(6); } else if (line.startsWith("UserID=")) { currentUserID = line.mid(7); } else if (line.startsWith("UserLogin=")) { currentUserLogin = line.mid(10); } }
        userInfoFile.close(); if (!bearerToken.isEmpty() && !currentUserID.isEmpty()) { qDebug() << "User info loaded from file."; } else { qDebug() << "Token or UserID missing in the saved file. Clearing session."; bearerToken = ""; currentUserID = ""; currentUserLogin = ""; }
    } else { qDebug() << "User info file not found or could not be opened:" << filePath; }
}

// Видалення задачі
void MainWindow::on_deleteTaskButton_clicked()
{
    QListWidgetItem *item = ui->taskListWidget->currentItem();
    if (!item) {
        qDebug() << "Delete button clicked, but no item selected.";
        QMessageBox::information(this, "Видалення", "Будь ласка, виберіть задачу для видалення.");
        return; // Немає вибраного елемента
    }

    // Отримуємо ID задачі з даних елемента
    bool ok;
    int taskId = item->data(Qt::UserRole).toInt(&ok);
    if (!ok || taskId <= 0) { // Перевіряємо, чи вдалося отримати валідний ID
        qWarning() << "Could not get a valid task ID from the selected item.";
        QMessageBox::critical(this, "Помилка", "Не вдалося отримати ID вибраної задачі.");
        return;
    }

    qDebug() << "Attempting to delete task with ID:" << taskId;

    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Підтвердження видалення",
        "Ви точно хочете видалити задачу:\n'" + item->text() + "'?", // Показуємо назву задачі
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No // Кнопка "Ні" за замовчуванням
        );

    if (reply == QMessageBox::Yes) {
        qDebug() << "User confirmed deletion for task ID:" << taskId;

        // --- ВИПРАВЛЕНО: Формуємо URL з ID як частина шляху ---
        std::string url = "https://todo.mindenit.org/items/delete/" + std::to_string(taskId);
        // -----------------------------------------------------

        qDebug() << "DELETE request URL:" << QString::fromStdString(url);

        cpr::Response r = cpr::Delete(
            cpr::Url{url},
            getAuthHeader() // Додаємо заголовок авторизації
            );

        qDebug() << "DELETE request status code: " << r.status_code;
        qDebug() << "DELETE request response body: " << QString::fromStdString(r.text);

        if (r.status_code == 200) {
            QMessageBox::information(this, "Успіх", "Задачу успішно видалено!");
            // Видаляємо елемент зі списку локально (швидше, ніж повне перезавантаження)
            // delete ui->taskListWidget->takeItem(ui->taskListWidget->row(item));
            // Або перезавантажуємо весь список з сервера
            loadTasksForUser(); // Це також вимкне кнопки видалення/редагування
        } else if (r.status_code == 401) {
            QMessageBox::warning(this, "Помилка сесії", "Ваша сесія більше недійсна. Будь ласка, увійдіть знову.");
            on_ButtonLogout_clicked();
        } else if (r.status_code == 404) {
            QMessageBox::warning(this, "Помилка", "Задачу не знайдено на сервері. Можливо, її вже видалено.");
            loadTasksForUser(); // Оновлюємо список, щоб прибрати неіснуючу задачу
        } else {
            QString errorMsg = "Не вдалося видалити задачу.";
            // Спробуємо отримати деталі помилки з JSON, якщо є
            QJsonParseError jsonError;
            QJsonDocument jsonResponse = QJsonDocument::fromJson(QString::fromStdString(r.text).toUtf8(), &jsonError);
            if (jsonError.error == QJsonParseError::NoError && jsonResponse.isObject()) {
                QJsonObject jsonObj = jsonResponse.object();
                if (jsonObj.contains("detail")) { errorMsg += "\nДеталі: " + jsonObj["detail"].toString(); }
                else if (jsonObj.contains("title")) { errorMsg += "\n" + jsonObj["title"].toString(); }
            } else {
                errorMsg += " Код помилки сервера: " + QString::number(r.status_code);
            }
            QMessageBox::warning(this, "Помилка видалення", errorMsg);
        }
    } else {
        qDebug() << "User cancelled deletion for task ID:" << taskId;
    }
}


// Подвійний клік на задачі (перегляд деталей)
void MainWindow::on_taskListWidget_itemDoubleClicked(QListWidgetItem *item)
{
    if (!item) return;

    // Отримуємо дані з елемента списку
    QString name = item->text();
    QString description = item->data(Qt::UserRole + 1).toString();
    int category = item->data(Qt::UserRole + 2).toInt();
    int priority = item->data(Qt::UserRole + 3).toInt();
    int aprxHours = item->data(Qt::UserRole + 4).toInt();
    QString endedAtRaw = item->data(Qt::UserRole + 5).toString();
    bool isLooped = item->data(Qt::UserRole + 6).toBool();

    // Форматуємо дату, якщо вона є
    QString endedAtFormatted = "Не вказано";
    if (!endedAtRaw.isNull() && !endedAtRaw.isEmpty() && endedAtRaw != "null") {
        // Спробуємо розпарсити дату/час в різних форматах (ISO з 'T' та 'Z', або без)
        QDateTime dateTime = QDateTime::fromString(endedAtRaw, Qt::ISODate);
        if (!dateTime.isValid()) {
            dateTime = QDateTime::fromString(endedAtRaw, Qt::ISODateWithMs); // Спробуємо з мілісекундами
        }
        if (dateTime.isValid()) {
            endedAtFormatted = dateTime.toLocalTime().toString("dd.MM.yyyy HH:mm"); // Форматуємо локальну дату і час
        } else {
            qDebug() << "Could not parse date:" << endedAtRaw;
            endedAtFormatted = endedAtRaw; // Якщо не вдалося розпарсити, показуємо як є
        }
    }


    // Формуємо текст для вікна повідомлення з HTML-розміткою
    QString taskInfo = QString(
                           "<b>Назва:</b> %1<br>"
                           "<b>Опис:</b> %2<br>"
                           "<b>Категорія ID:</b> %3<br>" // Поки що показуємо ID
                           "<b>Пріоритет:</b> %4<br>"
                           "<b>Орієнтовний час (год):</b> %5<br>"
                           "<b>Кінцевий термін:</b> %6<br>"
                           "<b>Зациклена:</b> %7"
                           ).arg(name.toHtmlEscaped(), // Екрануємо HTML символи в даних
                                description.toHtmlEscaped(),
                                QString::number(category),
                                QString::number(priority),
                                QString::number(aprxHours),
                                endedAtFormatted.toHtmlEscaped(),
                                isLooped ? "Так" : "Ні" ); // Додаємо інформацію про зацикленість


    QMessageBox msgBox(this); // Вказуємо батьківське вікно
    msgBox.setWindowTitle("Деталі задачі");
    msgBox.setTextFormat(Qt::RichText); // Дозволяє HTML-форматування
    msgBox.setText(taskInfo);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setIcon(QMessageBox::Information); // Використовуємо іконку інформації

    msgBox.exec();
}

// --- СЛОТ: Обробник кнопки редагування ---
void MainWindow::on_EditButton_clicked()
{
    QListWidgetItem *item = ui->taskListWidget->currentItem();
    if (!item) {
        qDebug() << "Edit button clicked, but no item selected.";
        QMessageBox::information(this, "Редагування", "Будь ласка, виберіть задачу для редагування.");
        return;
    }

    // Отримуємо всі дані задачі з елемента списку
    bool idOk;
    int taskId = item->data(Qt::UserRole).toInt(&idOk);
    if (!idOk || taskId <= 0) {
        qWarning() << "Could not get a valid task ID from the selected item for editing.";
        QMessageBox::critical(this, "Помилка", "Не вдалося отримати ID вибраної задачі для редагування.");
        return;
    }

    QString taskName = item->text();
    QString taskDescription = item->data(Qt::UserRole + 1).toString();
    int categoryId = item->data(Qt::UserRole + 2).toInt();
    int priority = item->data(Qt::UserRole + 3).toInt();
    int aprxHours = item->data(Qt::UserRole + 4).toInt();
    QString endedAt = item->data(Qt::UserRole + 5).toString(); // Зберігаємо як рядок ISO
    bool isLooped = item->data(Qt::UserRole + 6).toBool();

    qDebug() << "Attempting to open TaskWindow for editing task ID:" << taskId << "Name:" << taskName;

    // Створюємо вікно TaskWindow, передаючи ID задачі (для режиму редагування)
    TaskWindow *editTaskWindow = new TaskWindow(bearerToken, taskId, this);
    editTaskWindow->setAttribute(Qt::WA_DeleteOnClose);

    // --- ВАЖЛИВО: Передача даних до TaskWindow ---
    // Припускаємо, що у TaskWindow є метод setTaskDetails для заповнення полів
    // Вам потрібно буде реалізувати цей метод у taskwindow.cpp
    editTaskWindow->setTaskDetails(taskName, taskDescription, categoryId, priority, aprxHours, endedAt);
    // -------------------------------------------------

    // Підключаємо сигнал для оновлення списку після успішного збереження змін
    connect(editTaskWindow, &TaskWindow::taskSavedSuccessfully, this, &MainWindow::loadTasksForUser);

    editTaskWindow->setWindowTitle("Редагування задачі: " + taskName); // Встановлюємо заголовок вікна
    editTaskWindow->show();
}
