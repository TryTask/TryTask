#include "taskwindow.h"
#include "ui_taskwindow.h"
#include <QStandardPaths>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QRandomGenerator>



TaskWindow::TaskWindow(QWidget *parent, QString userID, QString taskID) :
    QMainWindow(parent),
    ui(new Ui::TaskWindow),
    userID(userID),
    taskID(taskID),
    isModified(false)
{
    ui->setupUi(this);

    if (!taskID.isEmpty()) {
        loadTaskData();
    }

    // Стиль
    this->setStyleSheet(
        "QTextEdit {"
        "   background-color: white;"
        "   color: black;"
        "   border: 1px solid gray;"
        "   padding: 8px;"
        "   border-radius: 5px;"
        "}"
        "QComboBox {"
        "   background-color: white;"
        "   color: black;"
        "   border: 1px solid gray;"
        "   padding: 5px;"
        "   border-radius: 5px;"
        "}"
        "QComboBox QAbstractItemView {"
        "   background-color: white;"
        "   selection-background-color: lightgray;"
        "   color: black;"
        "}"
        );

    // Відстеження змін у формах
    connect(ui->name, &QLineEdit::textChanged, this, [this]() { isModified = true; });
    connect(ui->textEditDescr, &QTextEdit::textChanged, this, [this]() { isModified = true; });
    connect(ui->execTime, &QLineEdit::textChanged, this, [this]() { isModified = true; });
    connect(ui->endDate, &QLineEdit::textChanged, this, [this]() { isModified = true; });
    connect(ui->Prior, &QComboBox::currentTextChanged, this, [this]() { isModified = true; });
     connect(ui->Categ, &QLineEdit::textChanged, this, [this]() { isModified = true; });
    connect(ui->Stage, &QComboBox::currentTextChanged, this, [this]() { isModified = true; });

    qDebug() << "TaskWindow отримав userID:" << userID;
}

TaskWindow::~TaskWindow()
{
    delete ui;
}

// Генерує унікальний ID задачі
QString TaskWindow::generateTaskID()
{
    return QString::number(QRandomGenerator::global()->bounded(100000, 999999));
}

// Отримує шлях до файлу збереження
QString TaskWindow::getFilePath()
{
    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    return desktopPath + "/tasks.txt";  // Файл на робочому столі
}

// Перевіряє, чи назва задачі унікальна
bool TaskWindow::isTaskNameUnique(const QString &taskName)
{
    QFile file(getFilePath());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return true;  // Якщо файл не відкривається, вважаємо, що задач ще немає

    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        if (line.startsWith("Назва: " + taskName + " | ID користувача: " + userID))
        {
            return false;  // Назва вже використана цим користувачем
        }
    }

    return true;
}

// Очищує поля
void TaskWindow::clearFields()
{
    ui->name->clear();
    ui->textEditDescr->clear();
    ui->execTime->clear();
    ui->endDate->clear();
    ui->Prior->setCurrentIndex(0);
    ui->Categ->clear();
    ui->Stage->setCurrentIndex(0);
    isModified = false;
}

// Збереження задачі у файл
void TaskWindow::on_save_clicked()
{
    QString taskName = ui->name->text().trimmed();
    QString description = ui->textEditDescr->toPlainText().trimmed();
    QString execTime = ui->execTime->text().trimmed();
    QString endDate = ui->endDate->text().trimmed();
    QString priority = ui->Prior->currentText();
    QString category = ui->Categ->text().trimmed();
    QString stage = ui->Stage->currentText();

    if (taskName.isEmpty() || description.isEmpty() || execTime.isEmpty() || endDate.isEmpty())
    {
        QMessageBox::warning(this, "Помилка", "Всі поля мають бути заповнені!");
        return;
    }

    if (!isTaskNameUnique(taskName))
    {
        QMessageBox::warning(this, "Помилка", "Задача з такою назвою вже існує!");
        return;
    }

    QString taskID = generateTaskID();
    QFile file(getFilePath());
    if (!file.open(QIODevice::Append | QIODevice::Text))
    {
        QMessageBox::warning(this, "Помилка", "Не вдалося зберегти задачу!");
        return;
    }

    QTextStream out(&file);
    out << "ID користувача: " << userID << "\n";
    out << "ID задачі: " << taskID << "\n";
    out << "Назва: " << taskName << "\n";
    out << "Опис: " << description << "\n";
    out << "Час виконання: " << execTime << "\n";
    out << "Запланована дата завершення: " << endDate << "\n";
    out << "Пріоритет: " << priority << "\n";
    out << "Категорія: " << category << "\n";
    out << "Етап: " << stage << "\n";
    out << "----------------------\n";

    file.close();
    isModified = false;

    // успішне збереження
    QMessageBox::information(this, "Збережено", "Задача успішно збережена!");
}



// Перехід на головну сторінку з перевіркою
void TaskWindow::on_BackToMain_clicked()
{
    if (isModified)
    {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, "Збереження", "Ви хочете зберегти зміни перед виходом?",
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

        if (reply == QMessageBox::Yes)
        {
            on_save_clicked();
        }
        else if (reply == QMessageBox::Cancel)
        {
            return;
        }
    }

    clearFields();
    this->close();
}

void TaskWindow::loadTaskData()
{
    QFile file(getFilePath());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Помилка", "Не вдалося відкрити файл задач!");
        return;
    }

    QTextStream in(&file);
    bool foundTask = false;

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();

        if (line.startsWith("ID задачі:") && line.section(": ", 1, 1).trimmed() == taskID) {
            foundTask = true;
            continue;
        }

        if (foundTask) {
            if (line.startsWith("Назва:")) {
                ui->name->setText(line.section(": ", 1, 1).trimmed());
            } else if (line.startsWith("Опис:")) {
                ui->textEditDescr->setPlainText(line.section(": ", 1, 1).trimmed());
            } else if (line.startsWith("Час виконання:")) {
                ui->execTime->setText(line.section(": ", 1, 1).trimmed());
            } else if (line.startsWith("Запланована дата завершення:")) {
                ui->endDate->setText(line.section(": ", 1, 1).trimmed());
            } else if (line.startsWith("Пріоритет:")) {
                ui->Prior->setCurrentText(line.section(": ", 1, 1).trimmed());
            } else if (line.startsWith("Категорія:")) {
                ui->Categ->setText(line.section(": ", 1, 1).trimmed());
            } else if (line.startsWith("Етап:")) {
                ui->Stage->setCurrentText(line.section(": ", 1, 1).trimmed());
            } else if (line == "----------------------") {
                break;
            }
        }
    }

    file.close();
}





