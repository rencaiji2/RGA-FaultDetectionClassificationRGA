#ifndef SINPUTDIALOG_H
#define SINPUTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>

class SInputDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SInputDialog(QStringList lstName, QStringList lstValue, QStringList lstButton, QWidget *parent = 0);
    QString parameter(const QString& strName);
    QString lastClickedButton();
    QWidget* itemWidget(const QString& strName);
    QString execEx();
private:
    QMap<QString, QWidget*> m_parameter;
    QPushButton* m_clickedButton;
signals:

public slots:
    void onClickButton();
};

#endif // SINPUTDIALOG_H
