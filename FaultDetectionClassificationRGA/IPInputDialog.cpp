#include "IPInputDialog.h"

IPInputDialog::IPInputDialog(QWidget *parent):QDialog(parent)
{
        setWindowTitle(tr("输入IP地址"));
        QVBoxLayout *mainLayout = new QVBoxLayout(this);

        QHBoxLayout *ipLayout = new QHBoxLayout;
        for (int i = 0; i < 4; ++i) {
            m_ipEdits[i] = new QLineEdit(this);
            m_ipEdits[i]->setValidator(new QIntValidator(0, 255, this));
            m_ipEdits[i]->setMaxLength(3);
            ipLayout->addWidget(m_ipEdits[i]);

            if (i < 3) {
                connect(m_ipEdits[i], &QLineEdit::textChanged, [this, i](const QString &text) {
                    if (text.length() == 3) {
                        m_ipEdits[i+1]->setFocus();
                    }
                });
            }
        }
        mainLayout->addLayout(ipLayout);

        QPushButton *btnOk = new QPushButton(tr("确认"), this);
        QPushButton *btnCancel = new QPushButton(tr("取消"), this);
        connect(btnOk, &QPushButton::clicked, this, &QDialog::accept);
        connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);

        QHBoxLayout *btnLayout = new QHBoxLayout;
        btnLayout->addWidget(btnOk);
        btnLayout->addWidget(btnCancel);
        mainLayout->addLayout(btnLayout);
    }
