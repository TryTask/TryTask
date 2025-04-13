/********************************************************************************
** Form generated from reading UI file 'categorydialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CATEGORYDIALOG_H
#define UI_CATEGORYDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_CategoryDialog
{
public:
    QGroupBox *groupBox;
    QFormLayout *formLayout;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QLineEdit *lineEdit;
    QPushButton *saveButton;

    void setupUi(QDialog *CategoryDialog)
    {
        if (CategoryDialog->objectName().isEmpty())
            CategoryDialog->setObjectName("CategoryDialog");
        CategoryDialog->resize(400, 300);
        groupBox = new QGroupBox(CategoryDialog);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(90, 70, 212, 124));
        formLayout = new QFormLayout(groupBox);
        formLayout->setObjectName("formLayout");
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        label = new QLabel(groupBox);
        label->setObjectName("label");

        verticalLayout->addWidget(label);

        lineEdit = new QLineEdit(groupBox);
        lineEdit->setObjectName("lineEdit");

        verticalLayout->addWidget(lineEdit);

        saveButton = new QPushButton(groupBox);
        saveButton->setObjectName("saveButton");

        verticalLayout->addWidget(saveButton);


        formLayout->setLayout(0, QFormLayout::LabelRole, verticalLayout);


        retranslateUi(CategoryDialog);

        QMetaObject::connectSlotsByName(CategoryDialog);
    } // setupUi

    void retranslateUi(QDialog *CategoryDialog)
    {
        CategoryDialog->setWindowTitle(QCoreApplication::translate("CategoryDialog", "Dialog", nullptr));
        groupBox->setTitle(QString());
        label->setText(QCoreApplication::translate("CategoryDialog", "\320\222\320\262\320\265\320\264\321\226\321\202\321\214 \320\262\320\273\320\260\321\201\320\275\321\203 \320\272\320\260\321\202\320\265\320\263\320\276\321\200\321\226\321\216", nullptr));
        saveButton->setText(QCoreApplication::translate("CategoryDialog", "Create", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CategoryDialog: public Ui_CategoryDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CATEGORYDIALOG_H
