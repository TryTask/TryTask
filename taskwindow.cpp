#include "taskwindow.h"
#include "ui_taskwindow.h"

#include <QMessageBox>
#include <QDebug>
#include <QTimeEdit>
#include <QDateTimeEdit>
#include <QSpinBox>
#include <QLineEdit>
#include <QTime>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <cpr/cpr.h>
// #include <QRandomGenerator> // Більше не потрібен
#include <string>
#include <QApplication> // Для QApplication::processEvents()

// --- Конструктор ---
TaskWindow::TaskWindow(QString bearerToken, QString taskID, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TaskWindow),
    m_bearerToken(bearerToken),
    m_taskID(taskID),
    m_isModified(false),
    m_isNewTask(taskID.isEmpty())
{
    ui->setupUi(this);
    // --- Налаштування віджетів ---
    ui->executionTimeEdit->setDisplayFormat(TIME_FORMAT);
    ui->dueDateTimeEdit->setDisplayFormat("dd.MM.yyyy HH:mm");
    ui->dueDateTimeEdit->setCalendarPopup(true);
    // Переконайтесь, що в .ui є QLineEdit з objectName="categoryLineEdit"

    // --- Стилі ---
    this->setStyleSheet(
        "QLineEdit, QTextEdit, QTimeEdit, QDateTimeEdit, QSpinBox {" // Перевірте наявність QLineEdit
        "   background-color: white; color: black; border: 1px solid gray;"
        "   padding: 5px; border-radius: 5px;"
        "}"
        "QTextEdit { padding: 8px; }"
        "QDateTimeEdit::up-button, QDateTimeEdit::down-button, "
        "QTimeEdit::up-button, QTimeEdit::down-button, "
        "QSpinBox::up-button, QSpinBox::down-button {"
        "   width: 15px;"
        "}"
        "QCalendarWidget QWidget { alternate-background-color: #e8f4ff; }"
        "QPushButton {"
        "   background-color: lightgray; color: black; border: 1px solid gray;"
        "   padding: 8px 15px; border-radius: 5px;"
        "}"
        "QPushButton:hover { background-color: darkgray; }"
        "QPushButton:pressed { background-color: gray; }"
    );
    // --------------------------

    // --- Підключення сигналів ---
    connect(ui->nameLineEdit, &QLineEdit::textChanged, this, [this]() { m_isModified = true; });
    connect(ui->descriptionTextEdit, &QTextEdit::textChanged, this, [this]() { m_isModified = true; });
    connect(ui->prioritySpinBox, qOverload<int>(&QSpinBox::valueChanged), this, [this]() { m_isModified = true; });
    connect(ui->categoryLineEdit, &QLineEdit::textChanged, this, [this]() { m_isModified = true; });
    connect(ui->executionTimeEdit, &QTimeEdit::timeChanged, this, [this]() { m_isModified = true; });
    connect(ui->dueDateTimeEdit, &QDateTimeEdit::dateTimeChanged, this, [this]() { m_isModified = true; });
    // --------------------------

    // --- Завантаження даних ---
    loadCategories(); // Завантажуємо категорії спочатку
    if (m_isNewTask) {
        // Встановлюємо значення за замовчуванням для нової задачі
        ui->dueDateTimeEdit->setDateTime(QDateTime::currentDateTime());
        ui->executionTimeEdit->setTime(QTime(1, 0));
        ui->prioritySpinBox->setValue(DEFAULT_PRIORITY);
        setWindowTitle("Нова задача");
    } else {
        setWindowTitle("Редагування задачі");
        loadTaskData(); // Завантажуємо дані існуючої задачі
    }
    qDebug() << "TaskWindow Initialized for" << (m_isNewTask ? "new task" : "existing task ID: " + m_taskID);
}

// --- Деструктор ---
TaskWindow::~TaskWindow() {
    delete ui;
    qDebug() << "TaskWindow Destroyed";
}

// --- Допоміжна функція для заголовка авторизації ---
cpr::Header TaskWindow::getAuthHeader() {
    if (m_bearerToken.isEmpty()) {
        qWarning() << "Bearer token is empty in TaskWindow!";
        return cpr::Header{}; // Повертаємо порожній заголовок
    }
    return cpr::Header{{"Authorization", "Bearer " + m_bearerToken.toStdString()}};
}


// --- Завантаження категорій з API (для отримання назв) ---
void TaskWindow::loadCategories() {
    qDebug() << "Loading categories to get names...";
    cpr::Response r = cpr::Get( cpr::Url{(API_BASE_URL + "/categories").toStdString()}, getAuthHeader() );
    qDebug() << "Load Categories Status Code:" << r.status_code;
    m_categoriesMap.clear(); // Очищуємо кеш перед завантаженням
    if(r.status_code == 200) {
        QJsonParseError error;
        QString responseText = QString::fromStdString(r.text); // Зберігаємо відповідь
        QJsonDocument doc = QJsonDocument::fromJson(responseText.toUtf8(), &error);
        if(error.error == QJsonParseError::NoError && doc.isArray()) {
            QJsonArray categoriesArray = doc.array();
            for(const QJsonValue& val : categoriesArray) {
                 if(val.isObject()) {
                     QJsonObject obj = val.toObject();
                     // Перевіряємо наявність обох полів перед додаванням
                     if(obj.contains("Id") && obj.contains("Name")) {
                         m_categoriesMap.insert(obj["Id"].toInt(), obj["Name"].toString());
                     } else {
                         qWarning() << "Category object missing Id or Name:" << obj;
                     }
                 }
            } qDebug() << "Loaded" << m_categoriesMap.size() << "category names.";
        } else {
             // Виводимо помилку парсингу ТА саму відповідь
             qWarning() << "Failed to parse categories JSON:" << error.errorString() << "Response:" << responseText;
        }
    } else {
        qWarning() << "Failed to load categories. Status code:" << r.status_code << "Reason:" << QString::fromStdString(r.reason);
        qDebug() << "Load Categories Response Body:" << QString::fromStdString(r.text); // Логуємо тіло при помилці HTTP
    }
}

// --- Відображення назви категорії за ID ---
void TaskWindow::displayCategoryName(int categoryId) {
    if (m_categoriesMap.contains(categoryId)) {
        // Категорія знайдена в кеші, встановлюємо назву
        ui->categoryLineEdit->setText(m_categoriesMap.value(categoryId));
    } else {
        // Категорія не знайдена (або кеш порожній)
        ui->categoryLineEdit->setText(""); // Залишаємо поле порожнім
        if (categoryId != INVALID_CATEGORY_ID && categoryId != 0) { // Не виводимо попередження для null/невалідних ID
             qDebug() << "Category ID:" << categoryId << "not found in loaded categories map during display.";
        }
    }
}

// --- Завантаження даних задачі з API ---
void TaskWindow::loadTaskData() {
     qDebug() << "Loading task data for ID:" << m_taskID;
     if (m_taskID.isEmpty()) return;

     // !!! РЕКОМЕНДОВАНО: Замінити на запит GET /items/{id} коли він буде в API !!!
     // Тимчасове рішення: завантажуємо всі задачі
     cpr::Response r = cpr::Get( cpr::Url{(API_BASE_URL + "/items").toStdString()}, getAuthHeader() );
     qDebug() << "Load Task (all items) Status Code:" << r.status_code;

     if (r.status_code == 200) {
         QJsonParseError error;
         QString responseText = QString::fromStdString(r.text); // Зберігаємо відповідь
         QJsonDocument doc = QJsonDocument::fromJson(responseText.toUtf8(), &error);
         if (error.error == QJsonParseError::NoError && doc.isArray()) {
             QJsonArray itemsArray = doc.array();
             bool found = false;
             int taskIdInt = m_taskID.toInt(); // Конвертуємо ID для порівняння

             for (const QJsonValue &itemValue : itemsArray) {
                 if (itemValue.isObject()) {
                     QJsonObject itemObject = itemValue.toObject();
                     // Перевіряємо наявність ID та співпадіння
                     if (itemObject.contains("Id") && itemObject["Id"].toInt() == taskIdInt) {
                         // Заповнюємо поля форми даними з JSON
                         ui->nameLineEdit->setText(itemObject["Name"].toString());
                         ui->descriptionTextEdit->setPlainText(itemObject["Description"].toString());
                         ui->prioritySpinBox->setValue(itemObject["Priority"].toInt(DEFAULT_PRIORITY));
                         int approxHours = itemObject["AprxHours"].toInt(0);
                         ui->executionTimeEdit->setTime(QTime(approxHours, 0));
                         // Парсимо дату з ISO формату
                         QDateTime dueDate = QDateTime::fromString(itemObject["EndedAt"].toString(), Qt::ISODate);
                         ui->dueDateTimeEdit->setDateTime(dueDate.isValid() ? dueDate : QDateTime::currentDateTime());

                         // Отримуємо ID категорії та відображаємо її назву
                         int categoryId = itemObject.contains("CategoryId") && !itemObject["CategoryId"].isNull()
                                          ? itemObject["CategoryId"].toInt(INVALID_CATEGORY_ID)
                                          : INVALID_CATEGORY_ID;
                         displayCategoryName(categoryId);

                         found = true;
                         qDebug() << "Task data loaded for ID:" << m_taskID;
                         break; // Знайшли, виходимо з циклу
                     }
                 }
             }
             if (!found) {
                 qWarning() << "Task with ID" << m_taskID << "not found in the fetched list!";
                 QMessageBox::warning(this, "Помилка", "Не вдалося знайти дані для задачі ID: " + m_taskID);
             }
         } else {
             qWarning() << "Failed to parse task list JSON:" << error.errorString() << "Response:" << responseText; // Логуємо відповідь при помилці парсингу
             QMessageBox::warning(this, "Помилка", "Не вдалося обробити список задач від сервера.");
         }
     } else if (r.status_code == 401) {
         QMessageBox::critical(this, "Помилка автентифікації", "Не вдалося завантажити дані задачі. Можливо, ваша сесія застаріла.");
         close(); // Закриваємо вікно при помилці авторизації
     } else {
         qWarning() << "Failed to load task list. Status code:" << r.status_code;
         qDebug() << "Load Task Response Body:" << QString::fromStdString(r.text); // Логуємо відповідь при помилці HTTP
         QMessageBox::warning(this, "Помилка", "Не вдалося завантажити список задач. Код: " + QString::number(r.status_code));
     }
     m_isModified = false; // Скидаємо прапорець після завантаження
}


// --- Збереження даних через API (з логікою перевірки/створення категорії) ---
bool TaskWindow::saveData() {
    // 1. Отримуємо назву категорії з поля вводу
    QString categoryName = ui->categoryLineEdit->text().trimmed();
    int categoryIdInt = INVALID_CATEGORY_ID; // ID для збереження в задачі

    // Блокуємо кнопку збереження
    ui->saveButton->setEnabled(false);
    QApplication::processEvents(); // Даємо UI можливість оновитись

    // --- Розділ, що може блокувати UI ---
    // 2. Визначаємо ID категорії
    if (!categoryName.isEmpty()) {
        qDebug() << "Resolving category:" << categoryName;
        int foundId = INVALID_CATEGORY_ID;

        // 2а. Шукаємо в локальному кеші
        for (auto it = m_categoriesMap.constBegin(); it != m_categoriesMap.constEnd(); ++it) {
            if (it.value().compare(categoryName, Qt::CaseInsensitive) == 0) {
                foundId = it.key(); break;
            }
        }

        // 2б. Якщо не знайшли в кеші, спробуємо завантажити знову
        if (foundId == INVALID_CATEGORY_ID) {
             qDebug() << "Category not in cache, re-fetching from API...";
             cpr::Response r_get_cats = cpr::Get(cpr::Url{(API_BASE_URL + "/categories").toStdString()}, getAuthHeader());
             qDebug() << "Re-Get Categories Status:" << r_get_cats.status_code;
             if (r_get_cats.status_code == 200) {
                 m_categoriesMap.clear(); // Оновлюємо кеш
                 QJsonParseError error; QString responseText = QString::fromStdString(r_get_cats.text);
                 QJsonDocument doc = QJsonDocument::fromJson(responseText.toUtf8(), &error);
                 if (error.error == QJsonParseError::NoError && doc.isArray()) {
                     QJsonArray cats = doc.array();
                     for (const auto& val : cats) {
                         QJsonObject obj = val.toObject();
                         if (obj.contains("Id") && obj.contains("Name")) {
                             int currentId = obj["Id"].toInt(); QString currentName = obj["Name"].toString();
                             m_categoriesMap.insert(currentId, currentName);
                             if (currentName.compare(categoryName, Qt::CaseInsensitive) == 0) { foundId = currentId; } // Знайшли
                         }
                     } qDebug() << "Re-fetched and parsed" << m_categoriesMap.size() << "categories.";
                 } else { qWarning() << "Failed to parse re-fetched categories JSON:" << error.errorString() << "Response:" << responseText; }
             } else { qWarning() << "Failed to re-fetch categories! Status:" << r_get_cats.status_code; }
        }

        // 2в. Якщо категорію знайдено
        if (foundId != INVALID_CATEGORY_ID) {
            categoryIdInt = foundId;
            qDebug() << "Using existing category ID:" << categoryIdInt;
        } else {
            // 2г. Якщо категорію НЕ знайдено, створюємо нову
            qDebug() << "Category still not found, creating new one...";
            QJsonObject newCategoryData; newCategoryData["Name"] = categoryName; newCategoryData["Color"] = DEFAULT_CATEGORY_COLOR;
            cpr::Response r_add_cat = cpr::Post( cpr::Url{(API_BASE_URL + "/categories/add").toStdString()}, getAuthHeader(), cpr::Header{{"Content-Type", "application/json"}}, cpr::Body{QJsonDocument(newCategoryData).toJson().constData()} );
            qDebug() << "Add Category Status:" << r_add_cat.status_code; qDebug() << "Add Category Response:" << QString::fromStdString(r_add_cat.text);
            if (r_add_cat.status_code >= 200 && r_add_cat.status_code < 300) {
                 qDebug() << "Category created. Re-fetching categories AGAIN to get new ID...";
                 // 2д. Знову завантажуємо категорії
                 cpr::Response r_get_cats_final = cpr::Get(cpr::Url{(API_BASE_URL + "/categories").toStdString()}, getAuthHeader());
                 qDebug() << "Final Get Categories Status:" << r_get_cats_final.status_code;
                 qDebug() << "Final Get Categories Response Body:" << QString::fromStdString(r_get_cats_final.text); // Логуємо відповідь
                 int newFoundId = INVALID_CATEGORY_ID;
                 if (r_get_cats_final.status_code == 200) {
                     m_categoriesMap.clear(); QJsonParseError error; QString responseTextFinal = QString::fromStdString(r_get_cats_final.text);
                     QJsonDocument doc = QJsonDocument::fromJson(responseTextFinal.toUtf8(), &error);
                     if (error.error == QJsonParseError::NoError && doc.isArray()) {
                         QJsonArray cats = doc.array();
                         for (const auto& val : cats) {
                             if (val.isObject()) { QJsonObject obj = val.toObject(); if(obj.contains("Id") && obj.contains("Name")) { m_categoriesMap.insert(obj["Id"].toInt(), obj["Name"].toString()); if (obj["Name"].toString().compare(categoryName, Qt::CaseInsensitive) == 0) { newFoundId = obj["Id"].toInt(); } } }
                         } qDebug() << "Parsed final categories list. Found ID:" << newFoundId;
                     } else { qWarning() << "Failed to parse final categories JSON:" << error.errorString() << "Response:" << responseTextFinal; } // Логуємо помилку і відповідь
                 } else { qWarning() << "Final Get Categories request failed. Status:" << r_get_cats_final.status_code; } // Логуємо помилку HTTP
                 // Встановлюємо знайдений ID (або залишаємо INVALID_CATEGORY_ID)
                 if (newFoundId != INVALID_CATEGORY_ID) { categoryIdInt = newFoundId; qDebug() << "Found newly created category ID:" << categoryIdInt; }
                 else { qWarning() << "Failed to find the newly created category ID!"; QMessageBox::warning(this, "Помилка категорії", "Категорію '" + categoryName + "' було створено, але не вдалося отримати її ID. Задача буде збережена без категорії."); categoryIdInt = INVALID_CATEGORY_ID; }
            } else { qWarning() << "Failed to create category. Status:" << r_add_cat.status_code; QMessageBox::warning(this, "Помилка категорії", "Не вдалося створити категорію '" + categoryName + "'. Задача буде збережена без категорії.\nКод: " + QString::number(r_add_cat.status_code)); categoryIdInt = INVALID_CATEGORY_ID; }
        }
    } // кінець if (!categoryName.isEmpty())
    // --- Кінець секції, що блокує UI ---

    // 3. Збираємо решту даних задачі
    QJsonObject taskData;
    if (!m_isNewTask) { taskData["Id"] = m_taskID.toInt(); }
    taskData["Name"] = ui->nameLineEdit->text().trimmed();
    taskData["Description"] = ui->descriptionTextEdit->toPlainText().trimmed();
    taskData["Priority"] = ui->prioritySpinBox->value();
    taskData["AprxHours"] = ui->executionTimeEdit->time().hour();
    taskData["EndedAt"] = ui->dueDateTimeEdit->dateTime().toString(Qt::ISODate);
    taskData["CategoryId"] = (categoryIdInt != INVALID_CATEGORY_ID) ? categoryIdInt : QJsonValue::Null; // Встановлюємо ID або null
    taskData["IsLooped"] = false;
    qDebug() << "Final data for task save:" << taskData;

    // 4. Валідація
    if (taskData["Name"].toString().isEmpty() || taskData["Description"].toString().isEmpty()) {
         QMessageBox::warning(this, "Помилка валідації", "Поля 'Назва' та 'Опис' мають бути заповнені!");
         ui->saveButton->setEnabled(true); return false;
    }

    // 5. Надсилання запиту POST або PUT для задачі
    cpr::Response r_save_task; QByteArray jsonData = QJsonDocument(taskData).toJson(); QString urlString;
    cpr::Header headers = getAuthHeader(); headers["Content-Type"] = "application/json";
    if (m_isNewTask) {
        urlString = API_BASE_URL + "/items/add"; qDebug() << "POST task to" << urlString;
        r_save_task = cpr::Post( cpr::Url{urlString.toStdString()}, headers, cpr::Body{jsonData.constData()} );
    } else {
        urlString = API_BASE_URL + "/items/update"; qDebug() << "PUT task to" << urlString;
        r_save_task = cpr::Put( cpr::Url{urlString.toStdString()}, headers, cpr::Body{jsonData.constData()} );
    }
    qDebug() << "Save Task Status Code:" << r_save_task.status_code; qDebug() << "Save Task Response Body:" << QString::fromStdString(r_save_task.text);

    // 6. Обробка результату
    bool success = false;
    if (r_save_task.status_code >= 200 && r_save_task.status_code < 300) {
        qDebug() << "Task data save request successful."; m_isModified = false;
        emit taskSavedSuccessfully(); success = true;
    } else {
        QString errorMessage = "Помилка збереження задачі!";
        if (r_save_task.text.find("foreign key constraint fails") != std::string::npos || r_save_task.text.find("Cannot add or update a child row") != std::string::npos) { errorMessage = "Помилка збереження: Неприпустимий ID категорії."; qWarning() << "Foreign key error, likely due to issue resolving CategoryId:" << categoryIdInt; }
        else { /* ... парсинг інших помилок JSON або HTTP ... */ }
        QMessageBox::warning(this, "Помилка збереження", errorMessage + "\nКод: " + QString::number(r_save_task.status_code));
        success = false;
    }

    ui->saveButton->setEnabled(true); return success; // Розблоковуємо кнопку
}


// --- Слот для кнопки "Зберегти" ---
void TaskWindow::on_saveButton_clicked() {
    if (saveData()) {
        close();
    }
}
// --- Слот для кнопки "Назад" ---
void TaskWindow::on_backButton_clicked() {
    qDebug() << "Back button clicked. isModified =" << m_isModified;
    if (m_isModified) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Незбережені зміни",
                                      "Ви маєте незбережені зміни. Зберегти їх перед виходом?",
                                      QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                                      QMessageBox::Cancel);
        if (reply == QMessageBox::Save) { if (!saveData()) { return; } } // Зберігаємо і закриваємо (якщо успішно)
        else if (reply == QMessageBox::Cancel) { return; } // Не закриваємо
        // Якщо Discard, просто закриваємо нижче
    }
    close(); // Закриваємо вікно
}
