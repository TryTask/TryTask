#ifndef TASKWINDOW_H
#define TASKWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QMap>
#include <cpr/cpr.h>

// Попередні оголошення
QT_BEGIN_NAMESPACE
namespace Ui { class TaskWindow; }
QT_END_NAMESPACE
class QTimeEdit;
class QDateTimeEdit;
class QSpinBox;
class QLineEdit;
class QTime;
class QDateTime;
class QJsonObject;
class QJsonArray;
// class QRandomGenerator; // Більше не потрібен

class TaskWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TaskWindow(QString bearerToken, QString taskID = "", QWidget *parent = nullptr);
    ~TaskWindow();

signals:
    void taskSavedSuccessfully();

private slots:
    void on_saveButton_clicked();
    void on_backButton_clicked();

private:
    Ui::TaskWindow *ui;
    QString m_bearerToken;
    QString m_taskID;
    bool m_isModified;
    bool m_isNewTask;
    QMap<int, QString> m_categoriesMap;

    // --- Приватні методи ---
    void loadTaskData();
    void loadCategories();
    bool saveData(); // Логіка буде значно змінена
    // collectDataForSave тепер не потрібен у такому вигляді
    cpr::Header getAuthHeader();
    void displayCategoryName(int categoryId);
    // Новий метод для отримання/створення ID категорії буде вбудований в saveData

    // --- Константи та значення за замовчуванням ---
    const QString API_BASE_URL = "https://todo.mindenit.org";
    const QString TIME_FORMAT = "HH:mm";
    const int DEFAULT_PRIORITY = 3;
    const QString DEFAULT_CATEGORY_COLOR = "black"; // Колір для нових категорій
    const int INVALID_CATEGORY_ID = -1; // Використовуємо для позначення помилки або "без категорії"
};

#endif // TASKWINDOW_H
