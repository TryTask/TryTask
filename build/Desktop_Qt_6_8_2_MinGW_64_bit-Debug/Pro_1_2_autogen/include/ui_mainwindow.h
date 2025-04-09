/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QStackedWidget *stackedWidget;
    QWidget *pageLogin;
    QLabel *img;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout_4;
    QLabel *label_4;
    QLineEdit *LogLine;
    QLabel *label_5;
    QLineEdit *PassLine;
    QWidget *layoutWidget_2;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *ButtonLogin;
    QPushButton *ButtonGoToRegister;
    QWidget *pageMain;
    QWidget *widget;
    QFormLayout *formLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *labelUsername;
    QLabel *labelUserID;
    QPushButton *ButtonLogout;
    QPushButton *ButtonNewTask;
    QListWidget *taskListWidget;
    QPushButton *refreshTasksButton;
    QWidget *pageRegister;
    QLabel *img2;
    QWidget *layoutWidget1;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QLineEdit *inLogin;
    QLabel *label_2;
    QLineEdit *inPassword;
    QLabel *label_3;
    QLineEdit *inPasswordCheck;
    QSplitter *splitter;
    QPushButton *ButtonRegister;
    QPushButton *ButtonReturnToLogin;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1118, 641);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        stackedWidget = new QStackedWidget(centralwidget);
        stackedWidget->setObjectName("stackedWidget");
        stackedWidget->setGeometry(QRect(0, 0, 1121, 641));
        pageLogin = new QWidget();
        pageLogin->setObjectName("pageLogin");
        img = new QLabel(pageLogin);
        img->setObjectName("img");
        img->setGeometry(QRect(440, 70, 281, 221));
        img->setPixmap(QPixmap(QString::fromUtf8("../Pro_1_1/img/14.jpg")));
        layoutWidget = new QWidget(pageLogin);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(410, 368, 301, 131));
        verticalLayout_4 = new QVBoxLayout(layoutWidget);
        verticalLayout_4->setObjectName("verticalLayout_4");
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        label_4 = new QLabel(layoutWidget);
        label_4->setObjectName("label_4");

        verticalLayout_4->addWidget(label_4);

        LogLine = new QLineEdit(layoutWidget);
        LogLine->setObjectName("LogLine");

        verticalLayout_4->addWidget(LogLine);

        label_5 = new QLabel(layoutWidget);
        label_5->setObjectName("label_5");

        verticalLayout_4->addWidget(label_5);

        PassLine = new QLineEdit(layoutWidget);
        PassLine->setObjectName("PassLine");

        verticalLayout_4->addWidget(PassLine);

        layoutWidget_2 = new QWidget(pageLogin);
        layoutWidget_2->setObjectName("layoutWidget_2");
        layoutWidget_2->setGeometry(QRect(470, 510, 175, 31));
        horizontalLayout_2 = new QHBoxLayout(layoutWidget_2);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        ButtonLogin = new QPushButton(layoutWidget_2);
        ButtonLogin->setObjectName("ButtonLogin");

        horizontalLayout_2->addWidget(ButtonLogin);

        ButtonGoToRegister = new QPushButton(layoutWidget_2);
        ButtonGoToRegister->setObjectName("ButtonGoToRegister");

        horizontalLayout_2->addWidget(ButtonGoToRegister);

        stackedWidget->addWidget(pageLogin);
        pageMain = new QWidget();
        pageMain->setObjectName("pageMain");
        widget = new QWidget(pageMain);
        widget->setObjectName("widget");
        widget->setGeometry(QRect(0, 0, 1121, 61));
        widget->setStyleSheet(QString::fromUtf8("background-color: lightgray;\n"
"border-radius: 10px;\n"
"padding: 10px;"));
        formLayout = new QFormLayout(widget);
        formLayout->setObjectName("formLayout");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        labelUsername = new QLabel(widget);
        labelUsername->setObjectName("labelUsername");

        horizontalLayout->addWidget(labelUsername);

        labelUserID = new QLabel(widget);
        labelUserID->setObjectName("labelUserID");

        horizontalLayout->addWidget(labelUserID);

        ButtonLogout = new QPushButton(widget);
        ButtonLogout->setObjectName("ButtonLogout");

        horizontalLayout->addWidget(ButtonLogout);


        formLayout->setLayout(0, QFormLayout::LabelRole, horizontalLayout);

        ButtonNewTask = new QPushButton(pageMain);
        ButtonNewTask->setObjectName("ButtonNewTask");
        ButtonNewTask->setGeometry(QRect(60, 90, 221, 191));
        taskListWidget = new QListWidget(pageMain);
        taskListWidget->setObjectName("taskListWidget");
        taskListWidget->setGeometry(QRect(350, 90, 256, 192));
        refreshTasksButton = new QPushButton(pageMain);
        refreshTasksButton->setObjectName("refreshTasksButton");
        refreshTasksButton->setGeometry(QRect(520, 310, 83, 29));
        stackedWidget->addWidget(pageMain);
        pageRegister = new QWidget();
        pageRegister->setObjectName("pageRegister");
        img2 = new QLabel(pageRegister);
        img2->setObjectName("img2");
        img2->setGeometry(QRect(230, 210, 281, 221));
        img2->setPixmap(QPixmap(QString::fromUtf8("../Pro_1_1/img/14.jpg")));
        layoutWidget1 = new QWidget(pageRegister);
        layoutWidget1->setObjectName("layoutWidget1");
        layoutWidget1->setGeometry(QRect(630, 200, 331, 211));
        verticalLayout = new QVBoxLayout(layoutWidget1);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(layoutWidget1);
        label->setObjectName("label");

        verticalLayout->addWidget(label);

        inLogin = new QLineEdit(layoutWidget1);
        inLogin->setObjectName("inLogin");

        verticalLayout->addWidget(inLogin);

        label_2 = new QLabel(layoutWidget1);
        label_2->setObjectName("label_2");

        verticalLayout->addWidget(label_2);

        inPassword = new QLineEdit(layoutWidget1);
        inPassword->setObjectName("inPassword");

        verticalLayout->addWidget(inPassword);

        label_3 = new QLabel(layoutWidget1);
        label_3->setObjectName("label_3");

        verticalLayout->addWidget(label_3);

        inPasswordCheck = new QLineEdit(layoutWidget1);
        inPasswordCheck->setObjectName("inPasswordCheck");

        verticalLayout->addWidget(inPasswordCheck);

        splitter = new QSplitter(pageRegister);
        splitter->setObjectName("splitter");
        splitter->setGeometry(QRect(630, 420, 198, 29));
        splitter->setOrientation(Qt::Orientation::Horizontal);
        ButtonRegister = new QPushButton(splitter);
        ButtonRegister->setObjectName("ButtonRegister");
        splitter->addWidget(ButtonRegister);
        ButtonReturnToLogin = new QPushButton(splitter);
        ButtonReturnToLogin->setObjectName("ButtonReturnToLogin");
        splitter->addWidget(ButtonReturnToLogin);
        stackedWidget->addWidget(pageRegister);
        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        img->setText(QString());
        label_4->setText(QCoreApplication::translate("MainWindow", "\320\233\320\276\320\263\321\226\320\275", nullptr));
        label_5->setText(QCoreApplication::translate("MainWindow", "\320\237\320\260\321\200\320\276\320\273\321\214", nullptr));
        ButtonLogin->setText(QCoreApplication::translate("MainWindow", "Log In", nullptr));
        ButtonGoToRegister->setText(QCoreApplication::translate("MainWindow", "Register", nullptr));
        labelUsername->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        labelUserID->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        ButtonLogout->setText(QCoreApplication::translate("MainWindow", "Log Out", nullptr));
        ButtonNewTask->setText(QCoreApplication::translate("MainWindow", "New Task", nullptr));
        refreshTasksButton->setText(QCoreApplication::translate("MainWindow", "Refresh", nullptr));
        img2->setText(QString());
        label->setText(QCoreApplication::translate("MainWindow", "\320\233\320\276\320\263\321\226\320\275", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "\320\237\320\260\321\200\320\276\320\273\321\214", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "\320\237\321\226\320\264\321\202\320\262\320\265\321\200\320\264\320\266\320\265\320\275\320\275\321\217 \320\277\320\260\321\200\320\276\320\273\321\216", nullptr));
        ButtonRegister->setText(QCoreApplication::translate("MainWindow", "Register", nullptr));
        ButtonReturnToLogin->setText(QCoreApplication::translate("MainWindow", "Return to Log in", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
