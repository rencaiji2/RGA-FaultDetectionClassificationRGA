#ifndef IPINPUTDIALOG_H
#define IPINPUTDIALOG_H

#include <QObject>

#include <QDialog>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>

#define LOCAL_IP "127.0.0.1"

class IPInputDialog : public QDialog {
    Q_OBJECT
public:
    
    explicit IPInputDialog(QWidget *parent = nullptr);

    QString getIPAddress() const {
        QStringList parts;
        for (int i = 0; i < 4; ++i) {
            parts << m_ipEdits[i]->text();
        }
        return parts.join(".");
    }

private:
    QLineEdit *m_ipEdits[4];
};

#endif // IPINPUTDIALOG_H
