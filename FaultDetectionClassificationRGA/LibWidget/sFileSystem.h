#ifndef SFILESYSTEM_H
#define SFILESYSTEM_H

#include <QMenu>
#include <QWidget>
//#include "sTableWidget.h"
#include "sFileManager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class sFileSystem; }
QT_END_NAMESPACE

class sFileSystem : public QWidget
{
    Q_OBJECT

public:
    sFileSystem(QWidget *parent = nullptr);
    ~sFileSystem();
    void show(QString& dir);
    void resize();

private slots:
    void on_UI_PB_SAVEAS_FS_clicked();
    void on_UI_PB_LOAD_FS_clicked();
    void on_UI_PB_DIR_FS_clicked();
    void slot_action_triggered(QAction *action);
    void on_UI_PB_CLOSE_FS_clicked();

private:
    Ui::sFileSystem *ui;
    QWidget* mParent= nullptr;
    sFileManager* mTableWidget= nullptr;
    sFileManager* mDestTable= nullptr;
    QMenu *mymenu=nullptr;
};
#endif // SFILESYSTEM_H
