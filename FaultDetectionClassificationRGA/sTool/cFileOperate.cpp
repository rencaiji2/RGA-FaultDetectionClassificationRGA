#include "cFileOperate.h"
#include <QDir>
#include <QFile>

cFileOperate::cFileOperate()
{

}

cFileOperate::~cFileOperate()
{

}

bool cFileOperate::isDir(const QString &pathName)
{
    QFileInfo fileInfo(pathName);
    return fileInfo.isDir();
}

bool cFileOperate::isFile(const QString &pathName)
{
    QFileInfo fileInfo(pathName);
    return fileInfo.isFile();
}

bool cFileOperate::DirExist(QString fullPath)
{
    QDir dir(fullPath);
    if(dir.exists()){//存在当前文件夹
        return true;
    }else{//不存在则创建
        bool ok = dir.mkdir(fullPath); //只创建一级子目录，即必须保证上级目录存在
        return ok;
    }
}

bool cFileOperate::DirExistEx(QString fullPath)
{
    QDir dir(fullPath);
    if(dir.exists()){
        return true;
    }else{//不存在当前目录，创建，可创建多级目录
        bool ok = dir.mkpath(fullPath);
        return ok;
    }
}

void cFileOperate::copyFile(const QString& sourcePath, const QString& targetPath, int index)
{
    if (sourcePath == targetPath)
        return;
    QString targetPath1;
    if(index){
        targetPath1= targetPath+ "_"+ QString::number(index);
    }else
        targetPath1= targetPath;
    if(!QFile::exists(targetPath1)){
        if(!QFile::copy(sourcePath,targetPath1))
            return;
    }else{
        copyFile(sourcePath, targetPath, ++index);
    }
}

void cFileOperate::copyFolder(const QString& sourcePath, const QString& targetPath)
{
    if (sourcePath == targetPath)
        return;
    QDir sourceDir(sourcePath);
    if(!sourceDir.exists())
        return;

    QDir targetDir(targetPath);
    if(!targetDir.exists()){
        if(!targetDir.mkpath(targetPath)){
            return;
        }
    }
    sourceDir.setFilter(QDir::NoDotAndDotDot | QDir::AllEntries);
    QStringList sourceFileList = sourceDir.entryList();
    for(auto& fileName : sourceFileList){
        QFileInfo fileInfo(sourcePath + fileName);
        if(fileInfo.isFile()){
            copyFile(sourcePath + fileName,targetPath + fileName);
        }else if(fileInfo.isDir()){
            if(!targetDir.exists(targetDir.absolutePath() + "/" + fileName)){
                if(!targetDir.mkdir(fileName)){
                    return;
                }
            }
            copyFolder(sourcePath + fileName + "/",targetPath + fileName + "/");
        }
    }
}

bool cFileOperate::copyFileToPath(const QStringList& sourcePath, const QString& targetPath)
{
    //path.replace("\\", "/");
    QString targetPath1;
    foreach(QString path, sourcePath){
        targetPath1= targetPath+ "/"+ path.right(path.size()- path.lastIndexOf("/")-1);
        if(cFileOperate::isFile(path)){
            copyFile(path, targetPath1);
        }else if(cFileOperate::isDir(path)){
            copyFolder(path+="/", targetPath1+ "/");
        }
    }
    return true;
}

bool cFileOperate::deleteDir(const QString &path)
{
    if (path.isEmpty()){
        return false;
    }
    QDir dir(path);
    if(!dir.exists()){
        //QFile::remove(path);
        QFile file(path);
        if(! file.remove()){
            return false;
        }
        return true;
    }
    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot); //设置过滤
    QFileInfoList fileList = dir.entryInfoList(); // 获取所有的文件信息
    foreach (QFileInfo file, fileList){ //遍历文件信息
        if (file.isFile()){ // 是文件，删除
            file.dir().remove(file.fileName());
        }else{ // 递归调用函数，删除子文件夹
            deleteDir(file.absoluteFilePath());
        }
    }
    return dir.rmpath(dir.absolutePath()); // 这时候文件夹已经空了，再删除文件夹本身
}
