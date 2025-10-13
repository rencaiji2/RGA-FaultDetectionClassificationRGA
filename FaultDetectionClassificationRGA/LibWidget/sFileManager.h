#ifndef sFileManager_H
#define sFileManager_H
/*注意此类线程不安全*/
#include <QCheckBox>
#include <QElapsedTimer>
#include <QFileInfoList>
#include <QFileSystemModel>
#include <QTableWidget>
#include <QWidget>

namespace Ui {
class sFileManager;
}

class sFileManager : public QWidget
{
    Q_OBJECT

public:
    enum _STYLE_WIDGET:quint32{_STYLE_ITEM,_STYLE_CHECKBOX};
    explicit sFileManager(QWidget *parent = nullptr, _STYLE_WIDGET style= _STYLE_ITEM);
    ~sFileManager();
    quint32 minWidthTableCell= 320;
    void updata(const QString& dirPath, bool updataRoot= true);
    void updata(const QString& dirPath, QStringList& filters/*filters << "*.txt" << "*.jpg"*/, bool updataRoot= true);
    bool getFilePath(QStringList& pStringList);//用于获取当前目录名或文件名
    bool getDirPath(QString& pString);//用于获取目录名

private slots:
    void on_tableWidget_cellClicked();
    void on_tableWidget_cellClicked(int row, int column);
    void on_UI_PB_OPEN_TW_clicked();
    void on_UI_PB_RETURN_TW_clicked();
    void on_UI_PB_DELETE_TW_clicked();
    void on_tableWidget_cellDoubleClicked(int row, int column);

private:
    Ui::sFileManager *ui;
    _STYLE_WIDGET mSTYLE_WIDGET;
    QStringList mRootDir, mCurrentDir;
    QStringList mFilters;
    QFileInfoList mCurrentFileList;
    QSize mSize;
    void paintEvent(QPaintEvent *);
    void GetFileInfoList(const QString& path, QStringList& filters);
    void showFileList();

    bool openDir(const QString &pathName);
    bool deleteDir(const QStringList &pathList);
    bool deleteDir(const QString &path);

    QElapsedTimer* mCellDoubleClickTimer=nullptr;

signals:
    void sCurrentFile(QString);//仅用于非CHECKBOX模式下，发送当前双击的文件名
};

class singletonFileManager{
public:
    static sFileManager* getWidgetFileManager(){
        static singletonFileManager insFileManager;
        return &(insFileManager.mFileManager);
    }
private:
    sFileManager mFileManager;
    singletonFileManager(){}
    virtual ~singletonFileManager(){}
    singletonFileManager(const singletonFileManager&){}
    singletonFileManager& operator=(const singletonFileManager&){
        static singletonFileManager insFileManager;
        return insFileManager;
    }
};
#endif // STABLEWIDGET_H
