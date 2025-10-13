#ifndef CFILEOPERATE_H
#define CFILEOPERATE_H

#include <QString>

class cFileOperate {
public:
    cFileOperate();
    ~cFileOperate();

    static bool isDir(const QString &pathName);
    static bool isFile(const QString &pathName);
    ///
    /// \brief 判断文件夹是否存在，不存在则创建
    /// \param fullPath
    /// \return
    ///
    static bool DirExist(QString fullPath);
    ///
    /// \brief 判断文件夹是否存在，不存在则创建, 可创建多级目录
    /// \param fullPath
    /// \return
    ///
    static bool DirExistEx(QString fullPath);
    static void copyFile(const QString& sourcePath,const QString& targetPath, int index=0);
    static void copyFolder(const QString& sourcePath,const QString& targetPath);
    static bool copyFileToPath(const QStringList& sourcePath,const QString& targetPath);
    static bool deleteDir(const QString &path);

};

#endif // CFILEOPERATE_H
