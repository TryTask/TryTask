#include "taskwindow.h"
#include "ui_taskwindow.h" // <--- Цей рядок має бути правильним для вашого проекту

#include <QMessageBox>
#include <QDebug>
#include <QTimeEdit>
#include <QDateTimeEdit>
#include <QSpinBox>
#include <QLineEdit>
// #include <QCheckBox> // Більше не потрібен для isLooped
#include <QTime>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <cpr/cpr.h>
#include <string>
#include <QApplication> // Для QApplication::processEvents()
#include <QMainWindow> // Змінено QDialog на QMainWindow
#include <QCloseEvent> // Додано для closeEvent


// --- Попереднє оголошення для Ui ---
namespace Ui {
class TaskWindow; // Попереднє оголошення класу Ui::TaskWindow
}



// --- Конструктор (змінено другий параметр на int) ---
TaskWindow::TaskWindow(QString token, int taskIdToEdit, QWidget *parent) :
    QMainWindow(parent), // Змінено QDialog на QMainWindow
    ui(new Ui::TaskWindow),
    m_bearerToken(token),
    editingTaskId(taskIdToEdit),
    m_isModified(false)
{
    ui->setupUi(this);
    // --- Налаштування віджетів ---
    ui->executionTimeEdit->setDisplayFormat(TIME_FORMAT);
    ui->dueDateTimeEdit->setDisplayFormat("dd.MM.yyyy HH:mm");
    ui->dueDateTimeEdit->setCalendarPopup(true);

    // --- Стилі ---
    this->setStyleSheet(
        "QLineEdit, QTextEdit, QTimeEdit, QDateTimeEdit, QSpinBox, QCheckBox {"
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

    // --- Підключення сигналів ---
    connect(ui->nameLineEdit, &QLineEdit::textChanged, this, [this]() { m_isModified = true; });
    connect(ui->descriptionTextEdit, &QTextEdit::textChanged, this, [this]() { m_isModified = true; });
    connect(ui->prioritySpinBox, qOverload<int>(&QSpinBox::valueChanged), this, [this]() { m_isModified = true; });
    connect(ui->categoryLineEdit, &QLineEdit::textChanged, this, [this]() { m_isModified = true; });
    connect(ui->executionTimeEdit, &QTimeEdit::timeChanged, this, [this]() { m_isModified = true; });
    connect(ui->dueDateTimeEdit, &QDateTimeEdit::dateTimeChanged, this, [this]() { m_isModified = true; });

    // --- Завантаження даних ---
    loadCategories();
    if (editingTaskId <= 0) {
        ui->dueDateTimeEdit->setDateTime(QDateTime::currentDateTime());
        ui->executionTimeEdit->setTime(QTime(1, 0));
        ui->prioritySpinBox->setValue(DEFAULT_PRIORITY);
        setWindowTitle("Нова задача");
        ui->saveButton->setText("Створити завдання");
    } else {
        setWindowTitle("Редагування задачі ID: " + QString::number(editingTaskId));
        ui->saveButton->setText("Зберегти зміни");
        loadTaskData();
    }
    qDebug() << "TaskWindow Initialized for" << (editingTaskId <= 0 ? "new task" : "existing task ID: " + QString::number(editingTaskId));
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
        return cpr::Header{};
    }
    return cpr::Header{{"Authorization", "Bearer " + m_bearerToken.toStdString()}};
}


// --- Завантаження категорій з API (для отримання назв) ---
void TaskWindow::loadCategories() {
    qDebug() << "Loading categories to get names...";
    cpr::Response r = cpr::Get( cpr::Url{(API_BASE_URL + "/categories").toStdString()}, getAuthHeader() );
    qDebug() << "Load Categories Status Code:" << r.status_code;
    m_categoriesMap.clear();
    if(r.status_code == 200) {
        QJsonParseError error;
        QString responseText = QString::fromStdString(r.text);
        QJsonDocument doc = QJsonDocument::fromJson(responseText.toUtf8(), &error);
        if(error.error == QJsonParseError::NoError && doc.isArray()) {
            QJsonArray categoriesArray = doc.array();
            for(const QJsonValue& val : categoriesArray) {
                if(val.isObject()) {
                    QJsonObject obj = val.toObject();
                    QString idKey = obj.contains("Id") ? "Id" : (obj.contains("id") ? "id" : "");
                    QString nameKey = obj.contains("Name") ? "Name" : (obj.contains("name") ? "name" : "");

                    if(!idKey.isEmpty() && !nameKey.isEmpty()) {
                        m_categoriesMap.insert(obj[idKey].toInt(), obj[nameKey].toString());
                    } else {
                        qWarning() << "Category object missing Id/id or Name/name:" << obj;
                    }
                }
            } qDebug() << "Loaded" << m_categoriesMap.size() << "category names.";
        } else {
            qWarning() << "Failed to parse categories JSON:" << error.errorString() << "Response:" << responseText;
        }
    } else {
        qWarning() << "Failed to load categories. Status code:" << r.status_code << "Reason:" << QString::fromStdString(r.reason);
        qDebug() << "Load Categories Response Body:" << QString::fromStdString(r.text);
    }
}

// --- Відображення назви категорії за ID ---
void TaskWindow::displayCategoryName(int categoryId) {
    if (m_categoriesMap.contains(categoryId)) {
        ui->categoryLineEdit->setText(m_categoriesMap.value(categoryId));
    } else {
        ui->categoryLineEdit->setText("");
        if (categoryId != INVALID_CATEGORY_ID && categoryId != 0) {
            qDebug() << "Category ID:" << categoryId << "not found in loaded categories map during display.";
        }
    }
}

// --- Завантаження даних задачі з API ---
void TaskWindow::loadTaskData() {
    qDebug() << "Loading task data for ID:" << editingTaskId;
    if (editingTaskId <= 0) return;

    cpr::Response r = cpr::Get( cpr::Url{(API_BASE_URL + "/items").toStdString()}, getAuthHeader() );
    qDebug() << "Load Task (all items) Status Code:" << r.status_code;

    if (r.status_code == 200) {
        QJsonParseError error;
        QString responseText = QString::fromStdString(r.text);
        QJsonDocument doc = QJsonDocument::fromJson(responseText.toUtf8(), &error);
        if (error.error == QJsonParseError::NoError && doc.isArray()) {
            QJsonArray itemsArray = doc.array();
            bool found = false;

            for (const QJsonValue &itemValue : itemsArray) {
                if (itemValue.isObject()) {
                    QJsonObject itemObject = itemValue.toObject();
                    QString idKey = itemObject.contains("Id") ? "Id" : (itemObject.contains("id") ? "id" : "");
                    if (!idKey.isEmpty() && itemObject[idKey].toInt() == editingTaskId) {
                        QString nameKey = itemObject.contains("Name") ? "Name" : (itemObject.contains("name") ? "name" : "");
                        QString descKey = itemObject.contains("Description") ? "Description" : (itemObject.contains("description") ? "description" : "");
                        QString priorityKey = itemObject.contains("Priority") ? "Priority" : (itemObject.contains("priority") ? "priority" : "");
                        QString hoursKey = itemObject.contains("AprxHours") ? "AprxHours" : (itemObject.contains("aprxHours") ? "aprxHours" : "");
                        QString endedAtKey = itemObject.contains("EndedAt") ? "EndedAt" : (itemObject.contains("endedAt") ? "endedAt" : "");
                        QString categoryIdKey = itemObject.contains("CategoryId") ? "CategoryId" : (itemObject.contains("categoryId") ? "categoryId" : "");

                        if (!nameKey.isEmpty()) ui->nameLineEdit->setText(itemObject[nameKey].toString());
                        if (!descKey.isEmpty()) ui->descriptionTextEdit->setPlainText(itemObject[descKey].toString());
                        if (!priorityKey.isEmpty()) ui->prioritySpinBox->setValue(itemObject[priorityKey].toInt(DEFAULT_PRIORITY));

                        int approxHours = itemObject.contains(hoursKey) ? itemObject[hoursKey].toInt(0) : 0;
                        ui->executionTimeEdit->setTime(QTime(approxHours, 0));

                        QString endedAtStr = itemObject.contains(endedAtKey) ? itemObject[endedAtKey].toString() : "";
                        QDateTime dueDate = QDateTime::fromString(endedAtStr, Qt::ISODate);
                        if (!dueDate.isValid()) {
                            dueDate = QDateTime::fromString(endedAtStr, Qt::ISODateWithMs);
                        }
                        ui->dueDateTimeEdit->setDateTime(dueDate.isValid() ? dueDate.toLocalTime() : QDateTime::currentDateTime());

                        int categoryId = itemObject.contains(categoryIdKey) && !itemObject[categoryIdKey].isNull()
                                             ? itemObject[categoryIdKey].toInt(INVALID_CATEGORY_ID)
                                             : INVALID_CATEGORY_ID;
                        displayCategoryName(categoryId);

                        found = true;
                        qDebug() << "Task data loaded for ID:" << editingTaskId;
                        break;
                    }
                }
            }
            if (!found) {
                qWarning() << "Task with ID" << editingTaskId << "not found in the fetched list!";
                QMessageBox::warning(this, "Помилка", "Не вдалося знайти дані для задачі ID: " + QString::number(editingTaskId));
            }
        } else {
            qWarning() << "Failed to parse task list JSON:" << error.errorString() << "Response:" << responseText;
            QMessageBox::warning(this, "Помилка", "Не вдалося обробити список задач від сервера.");
        }
    } else if (r.status_code == 401) {
        QMessageBox::critical(this, "Помилка автентифікації", "Не вдалося завантажити дані задачі. Можливо, ваша сесія застаріла.");
        close();
    } else {
        qWarning() << "Failed to load task list. Status code:" << r.status_code;
        qDebug() << "Load Task Response Body:" << QString::fromStdString(r.text);
        QMessageBox::warning(this, "Помилка", "Не вдалося завантажити список задач. Код: " + QString::number(r.status_code));
    }
    m_isModified = false;
}

// --- МЕТОД: Встановлення даних задачі з MainWindow (оновлено сигнатуру) ---
void TaskWindow::setTaskDetails(const QString &name, const QString &description, int categoryId, int priority, int aprxHours, const QString &endedAtIso) {
    qDebug() << "Setting task details from MainWindow - ID:" << editingTaskId << "Name:" << name;
    ui->nameLineEdit->setText(name);
    ui->descriptionTextEdit->setPlainText(description);
    ui->prioritySpinBox->setValue(priority);
    ui->executionTimeEdit->setTime(QTime(aprxHours, 0));

    if (!endedAtIso.isNull() && !endedAtIso.isEmpty() && endedAtIso != "null") {
        QDateTime dateTime = QDateTime::fromString(endedAtIso, Qt::ISODate);
        if (!dateTime.isValid()) {
            dateTime = QDateTime::fromString(endedAtIso, Qt::ISODateWithMs);
        }
        if (dateTime.isValid()) {
            ui->dueDateTimeEdit->setDateTime(dateTime.toLocalTime());
        } else {
            qDebug() << "Could not parse date from MainWindow:" << endedAtIso;
            ui->dueDateTimeEdit->setDateTime(QDateTime::currentDateTime());
        }
    } else {
        ui->dueDateTimeEdit->setDateTime(QDateTime::currentDateTime());
    }

    displayCategoryName(categoryId);

    m_isModified = false;
}


// --- Збереження даних через API (з логікою перевірки/створення категорії) ---
bool TaskWindow::saveData() {
    QString categoryName = ui->categoryLineEdit->text().trimmed();
    int categoryIdInt = INVALID_CATEGORY_ID;

    ui->saveButton->setEnabled(false);
    QApplication::processEvents();

    if (!categoryName.isEmpty()) {
        qDebug() << "Resolving category:" << categoryName;
        int foundId = INVALID_CATEGORY_ID;

        for (auto it = m_categoriesMap.constBegin(); it != m_categoriesMap.constEnd(); ++it) {
            if (it.value().compare(categoryName, Qt::CaseInsensitive) == 0) {
                foundId = it.key(); break;
            }
        }

        if (foundId == INVALID_CATEGORY_ID) {
            qDebug() << "Category not in cache, re-fetching from API...";
            cpr::Response r_get_cats = cpr::Get(cpr::Url{(API_BASE_URL + "/categories").toStdString()}, getAuthHeader());
            qDebug() << "Re-Get Categories Status:" << r_get_cats.status_code;
            if (r_get_cats.status_code == 200) {
                m_categoriesMap.clear();
                QJsonParseError error; QString responseText = QString::fromStdString(r_get_cats.text);
                QJsonDocument doc = QJsonDocument::fromJson(responseText.toUtf8(), &error);
                if (error.error == QJsonParseError::NoError && doc.isArray()) {
                    QJsonArray cats = doc.array();
                    for (const auto& val : cats) {
                        QJsonObject obj = val.toObject();
                        QString idKey = obj.contains("Id") ? "Id" : (obj.contains("id") ? "id" : "");
                        QString nameKey = obj.contains("Name") ? "Name" : (obj.contains("name") ? "name" : "");
                        if (!idKey.isEmpty() && !nameKey.isEmpty()) {
                            int currentId = obj[idKey].toInt(); QString currentName = obj[nameKey].toString();
                            m_categoriesMap.insert(currentId, currentName);
                            if (currentName.compare(categoryName, Qt::CaseInsensitive) == 0) { foundId = currentId; }
                        }
                    } qDebug() << "Re-fetched and parsed" << m_categoriesMap.size() << "categories.";
                } else { qWarning() << "Failed to parse re-fetched categories JSON:" << error.errorString() << "Response:" << responseText; }
            } else { qWarning() << "Failed to re-fetch categories! Status:" << r_get_cats.status_code; }
        }

        if (foundId != INVALID_CATEGORY_ID) {
            categoryIdInt = foundId;
            qDebug() << "Using existing category ID:" << categoryIdInt;
        } else {
            qDebug() << "Category still not found, creating new one...";
            QJsonObject newCategoryData; newCategoryData["Name"] = categoryName; newCategoryData["Color"] = DEFAULT_CATEGORY_COLOR;
            cpr::Response r_add_cat = cpr::Post( cpr::Url{(API_BASE_URL + "/categories/add").toStdString()}, getAuthHeader(), cpr::Header{{"Content-Type", "application/json"}}, cpr::Body{QJsonDocument(newCategoryData).toJson().constData()} );
            qDebug() << "Add Category Status:" << r_add_cat.status_code; qDebug() << "Add Category Response:" << QString::fromStdString(r_add_cat.text);
            if (r_add_cat.status_code >= 200 && r_add_cat.status_code < 300) {
                qDebug() << "Category created. Re-fetching categories AGAIN to get new ID...";
                cpr::Response r_get_cats_final = cpr::Get(cpr::Url{(API_BASE_URL + "/categories").toStdString()}, getAuthHeader());
                qDebug() << "Final Get Categories Status:" << r_get_cats_final.status_code;
                int newFoundId = INVALID_CATEGORY_ID;
                if (r_get_cats_final.status_code == 200) {
                    m_categoriesMap.clear(); QJsonParseError error; QString responseTextFinal = QString::fromStdString(r_get_cats_final.text);
                    QJsonDocument doc = QJsonDocument::fromJson(responseTextFinal.toUtf8(), &error);
                    if (error.error == QJsonParseError::NoError && doc.isArray()) {
                        QJsonArray cats = doc.array();
                        for (const auto& val : cats) {
                            if (val.isObject()) {
                                QJsonObject obj = val.toObject();
                                QString idKey = obj.contains("Id") ? "Id" : (obj.contains("id") ? "id" : "");
                                QString nameKey = obj.contains("Name") ? "Name" : (obj.contains("name") ? "name" : "");
                                if (!idKey.isEmpty() && !nameKey.isEmpty()) {
                                    int currentId = obj[idKey].toInt(); QString currentName = obj[nameKey].toString();
                                    m_categoriesMap.insert(currentId, currentName);
                                    if (currentName.compare(categoryName, Qt::CaseInsensitive) == 0) { newFoundId = currentId; }
                                }
                            }
                        } qDebug() << "Parsed final categories list. Found ID:" << newFoundId;
                    } else { qWarning() << "Failed to parse final categories JSON:" << error.errorString() << "Response:" << responseTextFinal; }
                } else { qWarning() << "Final Get Categories request failed. Status:" << r_get_cats_final.status_code; }
                if (newFoundId != INVALID_CATEGORY_ID) { categoryIdInt = newFoundId; qDebug() << "Found newly created category ID:" << categoryIdInt; }
                else { qWarning() << "Failed to find the newly created category ID!"; QMessageBox::warning(this, "Помилка категорії", "Категорію '" + categoryName + "' було створено, але не вдалося отримати її ID. Задача буде збережена без категорії."); categoryIdInt = INVALID_CATEGORY_ID; }
            } else { qWarning() << "Failed to create category. Status:" << r_add_cat.status_code; QMessageBox::warning(this, "Помилка категорії", "Не вдалося створити категорію '" + categoryName + "'. Задача буде збережена без категорії.\nКод: " + QString::number(r_add_cat.status_code)); categoryIdInt = INVALID_CATEGORY_ID; }
        }
    }

    QJsonObject taskData;
    if (editingTaskId > 0) {
        taskData["Id"] = editingTaskId;
    }
    taskData["Name"] = ui->nameLineEdit->text().trimmed();
    taskData["Description"] = ui->descriptionTextEdit->toPlainText().trimmed();
    taskData["Priority"] = ui->prioritySpinBox->value();
    taskData["AprxHours"] = ui->executionTimeEdit->time().hour();
    taskData["EndedAt"] = ui->dueDateTimeEdit->dateTime().toUTC().toString(Qt::ISODate);
    taskData["CategoryId"] = (categoryIdInt != INVALID_CATEGORY_ID) ? categoryIdInt : QJsonValue::Null;

    qDebug() << "Final data for task save:" << taskData;

    if (taskData["Name"].toString().isEmpty() || taskData["Description"].toString().isEmpty()) {
        QMessageBox::warning(this, "Помилка валідації", "Поля 'Назва' та 'Опис' мають бути заповнені!");
        ui->saveButton->setEnabled(true); return false;
    }

    cpr::Response r_save_task; QByteArray jsonData = QJsonDocument(taskData).toJson(); QString urlString;
    cpr::Header headers = getAuthHeader(); headers["Content-Type"] = "application/json";

    if (editingTaskId <= 0) {
        urlString = API_BASE_URL + "/items/add"; qDebug() << "POST task to" << urlString;
        r_save_task = cpr::Post( cpr::Url{urlString.toStdString()}, headers, cpr::Body{jsonData.constData()} );
    } else {
        urlString = API_BASE_URL + "/items/update"; qDebug() << "PUT task to" << urlString;
        r_save_task = cpr::Put( cpr::Url{urlString.toStdString()}, headers, cpr::Body{jsonData.constData()} );
    }
    qDebug() << "Save Task Status Code:" << r_save_task.status_code;
    qDebug() << "Save Task Response Body:" << QString::fromStdString(r_save_task.text);

    bool success = false;
    if (r_save_task.status_code >= 200 && r_save_task.status_code < 300) {
        qDebug() << "Task data save request successful."; m_isModified = false;
        emit taskSavedSuccessfully();
        success = true;
    } else {
        QString errorMessage = "Помилка збереження задачі!";
        QJsonParseError jsonError;
        QJsonDocument errorDoc = QJsonDocument::fromJson(QString::fromStdString(r_save_task.text).toUtf8(), &jsonError);
        if (jsonError.error == QJsonParseError::NoError && errorDoc.isObject()) {
            QJsonObject errorObj = errorDoc.object();
            if (errorObj.contains("detail")) errorMessage += "\nДеталі: " + errorObj["detail"].toString();
            else if (errorObj.contains("title")) errorMessage += "\n" + errorObj["title"].toString();
            else if (errorObj.contains("errors")) errorMessage = "Помилка валідації даних від сервера.";
        } else if (r_save_task.text.find("foreign key constraint fails") != std::string::npos || r_save_task.text.find("Cannot add or update a child row") != std::string::npos) {
            errorMessage = "Помилка збереження: Неприпустимий ID категорії.";
            qWarning() << "Foreign key error, likely due to issue resolving CategoryId:" << categoryIdInt;
        }
        QMessageBox::warning(this, "Помилка збереження", errorMessage + "\nКод: " + QString::number(r_save_task.status_code));
        success = false;
    }

    ui->saveButton->setEnabled(true); return success;
}


// --- Слот для кнопки "Зберегти" ---
void TaskWindow::on_saveButton_clicked() {
    if (saveData()) {
        close(); // Змінено accept() на close()
    }
}
// --- Слот для кнопки "Назад" (або "Скасувати") ---
void TaskWindow::on_backButton_clicked() {
    qDebug() << "Back button clicked. isModified =" << m_isModified;
    if (m_isModified) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Незбережені зміни",
                                                                  "Ви маєте незбережені зміни. Зберегти їх перед виходом?",
                                                                  QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                                                                  QMessageBox::Cancel);
        if (reply == QMessageBox::Save) { if (!saveData()) { return; } else { close(); return;} } // Змінено accept() на close()
        else if (reply == QMessageBox::Cancel) { return; }
    }
    close(); // Змінено reject() на close()
}

// --- Обробник події закриття вікна ---
void TaskWindow::closeEvent(QCloseEvent *event)
{
    qDebug() << "Close event triggered. isModified =" << m_isModified;
    if (m_isModified) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Незбережені зміни",
                                                                  "Ви маєте незбережені зміни. Зберегти їх перед закриттям?",
                                                                  QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
                                                                  QMessageBox::Cancel);
        if (reply == QMessageBox::Save) {
            if (!saveData()) {
                event->ignore();
                return;
            } else {
                event->accept();
                return;
            }
        } else if (reply == QMessageBox::Cancel) {
            event->ignore();
            return;
        }
    }
    event->accept();
    QMainWindow::closeEvent(event); // Змінено QDialog на QMainWindow
}
