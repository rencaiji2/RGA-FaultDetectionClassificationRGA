#include "sIPAddressBox.h"

#include <QLabel>

sIPAddressBox::sIPAddressBox(QWidget *parent) : QWidget(parent) {
    for (int i = 0; i < 4; ++i) {
        QSpinBox *spinBox = new QSpinBox(this);
        spinBox->setRange(0, 255);
        spinBox->setAlignment(Qt::AlignRight);
        spinBox->setValue(0);
        connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value){
            emit ipAddressChanged(getIPAddress());
        });
        lineEdits.append(spinBox);
        if (i < 3) {
            //QLineEdit *lineEdit = new QLineEdit(".", this);
            //lineEdit->setReadOnly(true);
            QLabel *lineEdit = new QLabel(".", this);
            lineLayout->addWidget(spinBox);
            lineLayout->addWidget(lineEdit);
        } else {
            lineLayout->addWidget(spinBox);
        }
    }
    setLayout(lineLayout);
}

bool sIPAddressBox::isReadOnly() const
{
    return lineEdits[0]->isReadOnly();
}

void sIPAddressBox::setReadOnly(bool ReadOnly)
{
    for(int i= 0; i<lineEdits.size(); ++i){
        lineEdits[i]->setReadOnly(ReadOnly);
    }
}
