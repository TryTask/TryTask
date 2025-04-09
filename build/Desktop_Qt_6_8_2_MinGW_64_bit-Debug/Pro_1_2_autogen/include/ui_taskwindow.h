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
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QLabel *label_6;
    QLineEdit *execTime;
    QLabel *label_7;
    QLineEdit *endDate;
    QWidget *layoutWidget1;
    QVBoxLayout *verticalLayout_2;
    QLabel *label;
    QLineEdit *name;
    QLabel *label_2;
    QTextEdit *textEditDescr;
    QWidget *layoutWidget2;
    QHBoxLayout *horizontalLayout;
    QPushButton *save;
    QPushButton *BackToMain;
    QWidget *widget;
    QVBoxLayout *verticalLayout_3;
    QLabel *label_4;
    QLineEdit *Categ;
    QLabel *label_3;
    QComboBox *Prior;
    QLabel *label_5;
    QComboBox *Stage;

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
        layoutWidget = new QWidget(groupBox);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(460, 260, 271, 119));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        label_6 = new QLabel(layoutWidget);
        label_6->setObjectName("label_6");

        verticalLayout->addWidget(label_6);

        execTime = new QLineEdit(layoutWidget);
        execTime->setObjectName("execTime");

        verticalLayout->addWidget(execTime);

        label_7 = new QLabel(layoutWidget);
        label_7->setObjectName("label_7");

        verticalLayout->addWidget(label_7);

        endDate = new QLineEdit(layoutWidget);
        endDate->setObjectName("endDate");

        verticalLayout->addWidget(endDate);

        layoutWidget1 = new QWidget(groupBox);
        layoutWidget1->setObjectName("layoutWidget1");
        layoutWidget1->setGeometry(QRect(30, 30, 701, 176));
        verticalLayout_2 = new QVBoxLayout(layoutWidget1);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(layoutWidget1);
        label->setObjectName("label");

        verticalLayout_2->addWidget(label);

        name = new QLineEdit(layoutWidget1);
        name->setObjectName("name");

        verticalLayout_2->addWidget(name);

        label_2 = new QLabel(layoutWidget1);
        label_2->setObjectName("label_2");

        verticalLayout_2->addWidget(label_2);

        textEditDescr = new QTextEdit(layoutWidget1);
        textEditDescr->setObjectName("textEditDescr");

        verticalLayout_2->addWidget(textEditDescr);

        layoutWidget2 = new QWidget(groupBox);
        layoutWidget2->setObjectName("layoutWidget2");
        layoutWidget2->setGeometry(QRect(510, 410, 222, 31));
        horizontalLayout = new QHBoxLayout(layoutWidget2);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        save = new QPushButton(layoutWidget2);
        save->setObjectName("save");

        horizontalLayout->addWidget(save);

        BackToMain = new QPushButton(layoutWidget2);
        BackToMain->setObjectName("BackToMain");

        horizontalLayout->addWidget(BackToMain);

        widget = new QWidget(groupBox);
        widget->setObjectName("widget");
        widget->setGeometry(QRect(30, 251, 181, 181));
        verticalLayout_3 = new QVBoxLayout(widget);
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        label_4 = new QLabel(widget);
        label_4->setObjectName("label_4");

        verticalLayout_3->addWidget(label_4);

        Categ = new QLineEdit(widget);
        Categ->setObjectName("Categ");

        verticalLayout_3->addWidget(Categ);

        label_3 = new QLabel(widget);
        label_3->setObjectName("label_3");

        verticalLayout_3->addWidget(label_3);

        Prior = new QComboBox(widget);
        Prior->addItem(QString());
        Prior->addItem(QString());
        Prior->addItem(QString());
        Prior->addItem(QString());
        Prior->setObjectName("Prior");

        verticalLayout_3->addWidget(Prior);

        label_5 = new QLabel(widget);
        label_5->setObjectName("label_5");

        verticalLayout_3->addWidget(label_5);

        Stage = new QComboBox(widget);
        Stage->addItem(QString());
        Stage->addItem(QString());
        Stage->addItem(QString());
        Stage->addItem(QString());
        Stage->setObjectName("Stage");

        verticalLayout_3->addWidget(Stage);

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
        save->setText(QCoreApplication::translate("TaskWindow", "Save", nullptr));
        BackToMain->setText(QCoreApplication::translate("TaskWindow", "Back to main page", nullptr));
        label_4->setText(QCoreApplication::translate("TaskWindow", "\320\232\320\260\321\202\320\265\320\263\320\276\321\200\321\226\321\217", nullptr));
        label_3->setText(QCoreApplication::translate("TaskWindow", "\320\237\321\200\321\226\320\276\321\200\321\226\321\202\320\265\321\202", nullptr));
        Prior->setItemText(0, QCoreApplication::translate("TaskWindow", "---", nullptr));
        Prior->setItemText(1, QCoreApplication::translate("TaskWindow", "\320\222\320\270\321\201\320\276\320\272\320\270\320\271", nullptr));
        Prior->setItemText(2, QCoreApplication::translate("TaskWindow", "\320\241\320\265\321\200\320\265\320\264\320\275\321\226\320\271", nullptr));
        Prior->setItemText(3, QCoreApplication::translate("TaskWindow", "\320\235\320\270\320\267\321\214\320\272\320\270\320\271", nullptr));

        label_5->setText(QCoreApplication::translate("TaskWindow", "\320\225\321\202\320\260\320\277", nullptr));
        Stage->setItemText(0, QCoreApplication::translate("TaskWindow", "---", nullptr));
        Stage->setItemText(1, QCoreApplication::translate("TaskWindow", "\320\243 \321\200\320\276\320\267\320\276\321\200\320\261\321\206\321\226", nullptr));
        Stage->setItemText(2, QCoreApplication::translate("TaskWindow", "\320\227\320\260\320\262\320\265\321\200\321\210\320\265\320\275\320\270\320\271", nullptr));
        Stage->setItemText(3, QCoreApplication::translate("TaskWindow", "\320\222\321\226\320\264\320\274\321\226\320\275\320\265\320\275\320\270\320\271", nullptr));

    } // retranslateUi

};

namespace Ui {
    class TaskWindow: public Ui_TaskWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TASKWINDOW_H
