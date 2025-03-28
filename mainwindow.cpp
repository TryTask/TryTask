#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QPixmap>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QRandomGenerator>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Стиль
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

    QFile file(getFilePath());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Помилка", "Файл з користувачами не знайдено!");
        return;
    }

    QTextStream in(&file);
    QString storedLogin, storedPassword, storedID;
    bool loginFound = false;

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();

        if (line.startsWith("ID: ")) {
            storedID = line.section(": ", 1, 1).trimmed();
        }
        else if (line.startsWith("Логін: ")) {
            storedLogin = line.section(": ", 1, 1).trimmed();
        }
        else if (line.startsWith("Пароль: ")) {
            storedPassword = line.section(": ", 1, 1).trimmed();

            // Перевірка логіна і пароля
            if (storedLogin == login && storedPassword == password) {
                loginFound = true;
                currentUserLogin = storedLogin;
                currentUserID = storedID;
                break;
            }
        }
    }

    file.close();

    if (loginFound) {
        QMessageBox::information(this, "Успіх", "Вхід виконано!");
        clearLoginFields();
        updateUserInfo(); // Оновлюємо інформацію на сторінці
        ui->stackedWidget->setCurrentWidget(ui->pageMain);
    } else {
        QMessageBox::warning(this, "Помилка", "Невірний логін або пароль!");
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

    if (isUserDuplicate(login, password)) {
        QMessageBox::warning(this, "Помилка", "Такий логін або пароль вже існує!");
        return;
    }

    QString id = generateUniqueID();
    saveUserToFile(id, login, password);

    QMessageBox::information(this, "Успіх", "Реєстрація успішна!");
    clearRegisterFields();  // Очищення полів реєстрації після успішної реєстрації
    ui->stackedWidget->setCurrentWidget(ui->pageLogin);
}

// Генерація унікального ID
QString MainWindow::generateUniqueID()
{
    return QString::number(QRandomGenerator::global()->bounded(100000, 999999));
}

// Очищення полів входу
void MainWindow::clearLoginFields()
{
    ui->LogLine->clear();
    ui->PassLine->clear();
}

// Отримання шляху до файлу користувачів
QString MainWindow::getFilePath()
{
    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    return desktopPath + "/users.txt";
}

// Перевірка, чи користувач уже існує
bool MainWindow::isUserDuplicate(const QString &login, const QString &password)
{
    QFile file(getFilePath());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.startsWith("Логін: ")) {
            QString storedLogin = line.section(": ", 1, 1).trimmed();
            QString storedPassword = in.readLine().section(": ", 1, 1).trimmed();
            if (storedLogin == login || storedPassword == password) {
                file.close();
                return true;
            }
        }
    }

    file.close();
    return false;
}

// Збереження нового користувача у файл
void MainWindow::saveUserToFile(const QString &id, const QString &login, const QString &password)
{
    QFile file(getFilePath());
    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        return;
    }

    QTextStream out(&file);
    out << "ID: " << id << "\n";
    out << "Логін: " << login << "\n";
    out << "Пароль: " << password << "\n";
    out << "--------------------\n";
    file.close();
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
