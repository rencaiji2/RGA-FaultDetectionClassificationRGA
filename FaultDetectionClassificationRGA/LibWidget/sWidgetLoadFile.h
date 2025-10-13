#ifndef SWIDGETLOADFILE_H
#define SWIDGETLOADFILE_H

#include <QWidget>
#include <LibWidget/sFileManager.h>

namespace Ui {
class sWidgetLoadFile;
}

class sWidgetLoadFile : public QWidget
{
    Q_OBJECT

public:
    explicit sWidgetLoadFile(QWidget *parent = nullptr);
    ~sWidgetLoadFile();
    void show(QString& filePath);
    void show(QString& filePath, QStringList& filters);
    void resize();

private slots:
    void on_UI_PB_OK_WLF_clicked();
    void on_UI_PB_CANCEL_WLF_clicked();
    void onCurrentFile(QString);

private:
    Ui::sWidgetLoadFile *ui;
    QWidget* mParent= nullptr;
    sFileManager* mDestTable= nullptr;

signals:
    void sOpenFile(QString);
};

class singletonWidgetLoadFile{
public:
    static sWidgetLoadFile* getWidgetLoadFile(){
        static singletonWidgetLoadFile insWidgetLoadFile;
        return &(insWidgetLoadFile.mWidgetLoadFile);
    }
private:
    sWidgetLoadFile mWidgetLoadFile;
    singletonWidgetLoadFile(){}
    virtual ~singletonWidgetLoadFile(){}
    singletonWidgetLoadFile(const singletonWidgetLoadFile&){}
    singletonWidgetLoadFile& operator=(const singletonWidgetLoadFile&){
        static singletonWidgetLoadFile insWidgetLoadFile;
        return insWidgetLoadFile;
    }
};
#endif // SWIDGETLOADFILE_H
