#ifndef SFILEDIR_H
#define SFILEDIR_H

#include <QWidget>
#include <QDir>
namespace Ui {
class sFileDir;
}

class sFileDir : public QWidget
{
    Q_OBJECT
public:
    explicit sFileDir(QWidget *parent = 0);
    ~sFileDir();
    QStringList getFileNames(const QString &path);
    void showFileList(int nColumn, const QStringList list);
private slots:
    void on_tableWidget_clicked(const QModelIndex &index);
signals:
    void selectFile(bool,QString);
private:
    Ui::sFileDir *ui;

};

#endif // SFILEDIR_H
