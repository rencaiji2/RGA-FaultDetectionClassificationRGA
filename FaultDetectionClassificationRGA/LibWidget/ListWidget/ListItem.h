#ifndef LISTITEM_H
#define LISTITEM_H

#include <QWidget>

namespace Ui {
class ListItem;
}

class ListItem : public QWidget
{
    Q_OBJECT

public:
    explicit ListItem(QWidget *parent = 0);
    ~ListItem();
    void setLineEdit(QString text);
    void setColor(QColor color);
    QColor getColor() { return myColor; }
    QString getLineEdit() { return massNum; }

private slots:
    void on_pushButton_clicked();

    void on_checkBox_stateChanged(int arg1);

signals:
    void itemRemove(ListItem* item);
    void itemChecked(ListItem* item,bool state);

private:
    Ui::ListItem *ui;
    QString massNum;
    QColor myColor;
};

#endif // LISTITEM_H
