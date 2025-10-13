#include "sInputdialog.h"

#include <QGridLayout>
#include <QLabel>
#include <QHBoxLayout>

SInputDialog::SInputDialog(QStringList lstName, QStringList lstValue, QStringList lstButton, QWidget *parent) :
    QDialog(parent)
{
    QGridLayout* pLayout;
    QHBoxLayout* pBtLayout;
    QLabel* pLabel;
    QPushButton* pButton;
    QWidget* pValueWidget;
    int nIndex = 0;

    m_clickedButton = NULL;
    pLayout = new QGridLayout(this);
    foreach (QString strName, lstName) {
        if(nIndex < lstValue.size()){
            pValueWidget = new QLineEdit(lstValue[nIndex], this);
        }else{
            pValueWidget = new QLineEdit("", this);
        }
        pLabel = new QLabel(strName, this);
        m_parameter[strName] = pValueWidget;
        pLayout->addWidget(pLabel, nIndex, 0);
        pLayout->addWidget(pValueWidget, nIndex, 1);
        nIndex++;
    }

    pBtLayout = new QHBoxLayout();
    foreach (QString strButton, lstButton) {
        pButton = new QPushButton(this);
        pButton->setText(strButton);
        connect(pButton, SIGNAL(clicked()), this, SLOT(onClickButton()));
        pBtLayout->addWidget(pButton);
    }
    pLayout->addLayout(pBtLayout, nIndex, 0, 1, 2);
    setLayout(pLayout);
}

QString SInputDialog::parameter(const QString &strName)
{
    auto iterParam = m_parameter.find(strName);
    if(iterParam != m_parameter.end()){
        return ((QLineEdit*)iterParam.value())->text();
    }
    return QString();
}

QString SInputDialog::lastClickedButton()
{
    if(m_clickedButton != NULL){
        return m_clickedButton->text();
    }
    return QString();
}

QWidget *SInputDialog::itemWidget(const QString &strName)
{
    auto iterParam = m_parameter.find(strName);
    return iterParam != m_parameter.end() ? iterParam.value() : NULL;
}


QString SInputDialog::execEx()
{
    QDialog::exec();
    return lastClickedButton();
}

void SInputDialog::onClickButton()
{
    m_clickedButton = (QPushButton*)sender();
    accept();
}
