#ifndef STICLIST_H
#define STICLIST_H

#include <QWidget>
#include "ListItem.h"
#include <QList>
#include <QListWidgetItem>

namespace Ui {
class sTICList;
}

class sTICList : public QWidget
{
    Q_OBJECT

public:
    explicit sTICList(QWidget *parent = 0);
    ~sTICList();
    QColor getColor();

private slots:
    void on_BtnAdd_clicked();
    void on_ItemRemove(ListItem* item);
    void on_ItemChecked(ListItem* item,bool state);

private:
    Ui::sTICList *ui;
    QList<QPair<ListItem*,QListWidgetItem*>> ItemList;
    QList<QPair<QColor,bool>> colorList;
    void modifyList();

signals:
    //删除item信号，mass为质量数
    void itemRemove(QString mass);
    //改变选中状态信号，mass为质量数，state为状态：0未选择，1选中
    //void itemChecked(QString mass,bool state);
    void HideXIC(uint32_t , QString , bool );
    void ChangedManager(QString, bool);//-schartwidget
};

#endif // STICLIST_H
