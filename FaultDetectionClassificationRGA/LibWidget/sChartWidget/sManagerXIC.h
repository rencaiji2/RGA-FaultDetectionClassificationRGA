#ifndef SMANAGERXIC_H
#define SMANAGERXIC_H

#include <QListWidgetItem>
#include <QWidget>

namespace Ui {
class sManagerXIC;
}

class sManagerXIC : public QWidget
{
    Q_OBJECT

public:
    enum _MANAGE_TYPE{_MANAGE_ADD,
                      _MANAGE_MODIFY,
                      _MANAGE_REMOVE};
    enum _MANAGE_VALUE{_MANAGE_Event=Qt::UserRole,
                       _MANAGE_Mass,
                       _MANAGE_Color,
                       _MANAGE_Gain,
                       _MANAGE_Offset,
                       _MANAGE_Type};
    explicit sManagerXIC(QWidget *parent = nullptr);
    ~sManagerXIC();
    void setXic(QListWidgetItem* pQListWidgetItem);
    uint getXic(QListWidgetItem* pQListWidgetItem);
private slots:
    void on_B_Color_clicked();
    void on_B_OK_clicked(){
        emit B_clicked(mQListWidgetItem,true);
    }
    void on_B_Cancel_clicked(){
        emit B_clicked(mQListWidgetItem,false);
    }
private:
    Ui::sManagerXIC *ui;
    QListWidgetItem* mQListWidgetItem= nullptr;
signals:
    void B_clicked(QListWidgetItem*, bool );
};

#endif // SMANAGERXIC_H
