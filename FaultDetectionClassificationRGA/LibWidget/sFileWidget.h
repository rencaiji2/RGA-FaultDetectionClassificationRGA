#ifndef SFILEWIDGET_H
#define SFILEWIDGET_H

#include <QFileInfoList>
#include <QWidget>

namespace Ui {
class sFileWidget;
}

class sFileWidget : public QWidget
{
    Q_OBJECT

public:
    explicit sFileWidget(QWidget *parent = 0);
    ~sFileWidget();
    bool creatNewFile(QString path,QString name);
    uint getFrameCount();
private:
    Ui::sFileWidget *ui;
    QString mfileName;
    QFileInfoList GetFileList(QString path);
    void showFileList(QFileInfoList list);
signals:
    void selectFile(bool,QString);
private slots:
    void on_PB_OK_clicked(){
        emit selectFile(true,mfileName);
    }

    void on_PB_CANCEL_clicked(){
        emit selectFile(false,mfileName);
    }
};

#endif // SFILEWIDGET_H
