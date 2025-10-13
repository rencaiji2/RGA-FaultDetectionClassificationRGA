#ifndef SIPADDRESSBOX_H
#define SIPADDRESSBOX_H

#include <QSpinBox>
//#include <QLineEdit>
#include <QHBoxLayout>
#include <QWidget>

class sIPAddressBox : public QWidget
{
    Q_OBJECT
public:
    explicit sIPAddressBox(QWidget *parent = nullptr);
    QString getIPAddress() const {
        QString ip;
        QStringList strList;
        for (int i = 0; i < lineEdits.size(); ++i)
            strList<< QString::number(lineEdits.at(i)->value());
        ip= strList.join(".");
        return ip;
    }
    bool setIPAddress(QString& ip){
        QStringList strList= ip.split(".");
        if(strList.size()>4)
            return false;
        for (int i = 0; i < strList.size(); ++i)
            lineEdits.at(i)->setValue(strList[i].toInt());
        return true;
    }
    bool isReadOnly() const;
    void setReadOnly(bool);
signals:
    void ipAddressChanged(const QString &ipAddress);

private:
    QList<QSpinBox*> lineEdits;
    QHBoxLayout *lineLayout = new QHBoxLayout();

//signals:

//public slots:
};

#endif // SIPADDRESSBOX_H
