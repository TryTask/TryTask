/********************************************************************************
** Form generated from reading UI file 'taskwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TASKWINDOW_H
#define UI_TASKWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TaskWindow
{
public:
    QWidget *centralwidget;
    QGroupBox *groupBox;
    QWidget *widget;
    QVBoxLayout *verticalLayout;
    QLabel *label_6;
    QLineEdit *execTime;
    QLabel *label_7;
    QLineEdit *endDate;
    QWidget *widget1;
    QVBoxLayout *verticalLayout_2;
    QLabel *label;
    QLineEdit *name;
    QLabel *label_2;
    QTextEdit *textEditDescr;
    QWidget *widget2;
    QVBoxLayout *verticalLayout_3;
    QLabel *label_3;
    QComboBox *Prior;
    QLabel *label_4;
    QComboBox *Categ;
    QLabel *label_5;
    QComboBox *Stage;
    QWidget *widget3;
    QHBoxLayout *horizontalLayout;
    QPushButton *save;
    QPushButton *BackToMain;

    void setupUi(QMainWindow *TaskWindow)
    {
        if (TaskWindow->objectName().isEmpty())
            TaskWindow->setObjectName("TaskWindow");
        TaskWindow->resize(1044, 682);
        centralwidget = new QWidget(TaskWindow);
        centralwidget->setObjectName("centralwidget");
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(140, 70, 751, 521));
        widget = new QWidget(groupBox);
        widget->setObjectName("widget");
        widget->setGeometry(QRect(460, 260, 271, 119));
        verticalLayout = new QVBoxLayout(widget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        label_6 = new QLabel(widget);
        label_6->setObjectName("label_6");

        verticalLayout->addWidget(label_6);

        execTime = new QLineEdit(widget);
        execTime->setObjectName("execTime");

        verticalLayout->addWidget(execTime);

        label_7 = new QLabel(widget);
        label_7->setObjectName("label_7");

        verticalLayout->addWidget(label_7);

        endDate = new QLineEdit(widget);
        endDate->setObjectName("endDate");

        verticalLayout->addWidget(endDate);

        widget1 = new QWidget(groupBox);
        widget1->setObjectName("widget1");
        widget1->setGeometry(QRect(30, 30, 701, 171));
        verticalLayout_2 = new QVBoxLayout(widget1);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(widget1);
        label->setObjectName("label");

        verticalLayout_2->addWidget(label);

        name = new QLineEdit(widget1);
        name->setObjectName("name");

        verticalLayout_2->addWidget(name);

        label_2 = new QLabel(widget1);
        label_2->setObjectName("label_2");

        verticalLayout_2->addWidget(label_2);

        textEditDescr = new QTextEdit(widget1);
        textEditDescr->setObjectName("textEditDescr");

        verticalLayout_2->addWidget(textEditDescr);

        widget2 = new QWidget(groupBox);
        widget2->setObjectName("widget2");
        widget2->setGeometry(QRect(30, 260, 151, 181));
        verticalLayout_3 = new QVBoxLayout(widget2);
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        label_3 = new QLabel(widget2);
        label_3->setObjectName("label_3");

        verticalLayout_3->addWidget(label_3);

        Prior = new QComboBox(widget2);
        Prior->addItem(QString());
        Prior->addItem(QString());
        Prior->addItem(QString());
        Prior->addItem(QString());
        Prior->setObjectName("Prior");

        verticalLayout_3->addWidget(Prior);

        label_4 = new QLabel(widget2);
        label_4->setObjectName("label_4");

        verticalLayout_3->addWidget(label_4);

        Categ = new QComboBox(widget2);
        Categ->addItem(QString());
        Categ->addItem(QString());
        Categ->addItem(QString());
        Categ->addItem(QString());
        Categ->addItem(QString());
        Categ->setObjectName("Categ");

        verticalLayout_3->addWidget(Categ);

        label_5 = new QLabel(widget2);
        label_5->setObjectName("label_5");

        verticalLayout_3->addWidget(label_5);

        Stage = new QComboBox(widget2);
        Stage->addItem(QString());
        Stage->addItem(QString());
        Stage->addItem(QString());
        Stage->addItem(QString());
        Stage->setObjectName("Stage");

        verticalLayout_3->addWidget(Stage);

        widget3 = new QWidget(groupBox);
        widget3->setObjectName("widget3");
        widget3->setGeometry(QRect(510, 410, 222, 31));
        horizontalLayout = new QHBoxLayout(widget3);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        save = new QPushButton(widget3);
        save->setObjectName("save");

        horizontalLayout->addWidget(save);

        BackToMain = new QPushButton(widget3);
        BackToMain->setObjectName("BackToMain");

        horizontalLayout->addWidget(BackToMain);

        TaskWindow->setCentralWidget(centralwidget);

        retranslateUi(TaskWindow);

        QMetaObject::connectSlotsByName(TaskWindow);
    } // setupUi

    void retranslateUi(QMainWindow *TaskWindow)
    {
        TaskWindow->setWindowTitle(QCoreApplication::translate("TaskWindow", "MainWindow", nullptr));
        groupBox->setTitle(QString());
        label_6->setText(QCoreApplication::translate("TaskWindow", "\320\237\321\200\320\270\320\261\320\273\320\270\320\267\320\275\320\270\320\271 \321\207\320\260\321\201 \320\262\320\270\320\272\320\276\320\275\320\260\320\275\320\275\321\217", nullptr));
        label_7->setText(QCoreApplication::translate("TaskWindow", "\320\232\321\226\320\275\321\206\320\265\320\262\320\260 \321\202\320\265\321\200\320\274\321\226\320\275 \320\262\320\270\320\272\320\276\320\275\320\260\320\275\320\275\321\217", nullptr));
        label->setText(QCoreApplication::translate("TaskWindow", "\320\235\320\260\320\267\320\262\320\260", nullptr));
        label_2->setText(QCoreApplication::translate("TaskWindow", "\320\236\320\277\320\270\321\201", nullptr));
        label_3->setText(QCoreApplication::translate("TaskWindow", "\320\237\321\200\321\226\320\276\321\200\321\226\321\202\320\265\321\202", nullptr));
        Prior->setItemText(0, QCoreApplication::translate("TaskWindow", "---", nullptr));
        Prior->setItemText(1, QCoreApplication::translate("TaskWindow", "\320\222\320\270\321\201\320\276\320\272\320\270\320\271", nullptr));
        Prior->setItemText(2, QCoreApplication::translate("TaskWindow", "\320\241\320\265\321\200\320\265\320\264\320\275\321\226\320\271", nullptr));
        Prior->setItemText(3, QCoreApplication::translate("TaskWindow", "\320\235\320\270\320\267\321\214\320\272\320\270\320\271", nullptr));

        label_4->setText(QCoreApplication::translate("TaskWindow", "\320\232\320\260\321\202\320\265\320\263\320\276\321\200\321\226\321\217", nullptr));
        Categ->setItemText(0, QCoreApplication::translate("TaskWindow", "---", nullptr));
        Categ->setItemText(1, QCoreApplication::translate("TaskWindow", "\320\240\320\276\320\261\320\276\321\202\320\260", nullptr));
        Categ->setItemText(2, QCoreApplication::translate("TaskWindow", "\320\224\321\226\320\274", nullptr));
        Categ->setItemText(3, QCoreApplication::translate("TaskWindow", "\320\235\320\260\320\262\321\207\320\260\320\275\320\275\321\217", nullptr));
        Categ->setItemText(4, QCoreApplication::translate("TaskWindow", "\320\237\321\200\320\276\320\274\320\270\321\201\320\273\320\276\320\262\321\226\321\201\321\202\321\214", nullptr));

        label_5->setText(QCoreApplication::translate("TaskWindow", "\320\225\321\202\320\260\320\277", nullptr));
        Stage->setItemText(0, QCoreApplication::translate("TaskWindow", "---", nullptr));
        Stage->setItemText(1, QCoreApplication::translate("TaskWindow", "\320\243 \321\200\320\276\320\267\320\276\321\200\320\261\321\206\321\226", nullptr));
        Stage->setItemText(2, QCoreApplication::translate("TaskWindow", "\320\227\320\260\320\262\320\265\321\200\321\210\320\265\320\275\320\270\320\271", nullptr));
        Stage->setItemText(3, QCoreApplication::translate("TaskWindow", "\320\222\321\226\320\264\320\274\321\226\320\275\320\265\320\275\320\270\320\271", nullptr));

        save->setText(QCoreApplication::translate("TaskWindow", "Save", nullptr));
        BackToMain->setText(QCoreApplication::translate("TaskWindow", "Back to main page", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TaskWindow: public Ui_TaskWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TASKWINDOW_H
