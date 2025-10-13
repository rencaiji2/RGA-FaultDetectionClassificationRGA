#ifndef PUBLICALGORITHM_H
#define PUBLICALGORITHM_H

#include <QObject>
#include <QMap>
#include <QList>
#include <QPair>
#include <QDateTime>
#include <QVariantMap>
#include <QVariantList>
#include <QStringList>
#include <QVector>
#include <QDebug>
#include <cmath>
#include <QVariant>
#include <QFile>
#include <QTextStream>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>

class cJsonFileOperate
{
public:
    cJsonFileOperate(){}

    static void jsonStr2File(const QString i_filePath,const QString i_jsonStr)
    {
        QFile tmpFile(i_filePath);
        if(tmpFile.open(QIODevice::WriteOnly | QIODevice::Text)){
            QTextStream out(&tmpFile);
            out << i_jsonStr;
        }
        tmpFile.close();
    }

    static QString variant2Json(QVariant i_val){
        QString jsonStr = "";
            if(i_val.type() == QVariant::Map)
            {
                //QVariantMap
                QJsonObject obj = QJsonObject::fromVariantMap(i_val.toMap());
                QJsonDocument jsonDoc(obj);
                jsonStr = jsonDoc.toJson(QJsonDocument::Indented);
            }
            else if(i_val.type() == QVariant::List)
            {
                //QVariantList  注意返回的data
                QJsonObject obj;
                obj["data"] = QJsonArray::fromVariantList(i_val.toList());
                QJsonDocument jsonDoc(obj);
                jsonStr = jsonDoc.toJson(QJsonDocument::Indented);
            }
            return jsonStr.toUtf8();
    }
    static QVariant json2Varaint(QString i_jsonStr){
        QVariant val;
        QJsonParseError parseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(i_jsonStr.toUtf8(),&parseError);
        if(!jsonDoc.isNull()){
            // 将QJsonDocument转换为QJsonObject
            if (jsonDoc.isObject()) {
                QJsonObject jsonObject = jsonDoc.object();
                val = jsonObject.toVariantMap();
            }
        }
        else{
            qDebug() << "json2Varaint error:" << parseError.errorString();
        }

        return val;
    }
    static void toJsonFile(QVariant i_val,QString i_fileFullName){
        QString jsonStr = variant2Json(i_val);
            QFile file(i_fileFullName);
            if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
                return;

            //QTextStream out(&file);
            //out << jsonStr.toUtf8();
            file.write(jsonStr.toUtf8());

            file.close();
    }
    static QVariant jsonFile2QVarint(QString i_fileFullName){
        return jsonByteFile2QVarint(i_fileFullName);
        /* 2025-0124暂时注释掉，测试发现json文件大于100kb的时候就转化失败了
        QFile file(i_fileFullName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return QVariant();

        QTextStream in(&file);
        QString jsonStr = in.readAll();
        file.close();
        return json2Varaint(jsonStr);
        */
    }

    static QVariant jsonByteFile2QVarint(QString i_fileFullName)
    {
        QFile file(i_fileFullName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return QVariant();

        QByteArray fileData = file.readAll();
        QJsonParseError jsonParseError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(fileData, &jsonParseError);

        if (jsonParseError.error != QJsonParseError::NoError) {
            qWarning() << "JSON parse error:" << jsonParseError.errorString();
            return QVariantMap();
        }

        if (!jsonDoc.isObject()) {
            qWarning() << "The JSON document is not an object.";
            return QVariantMap();
        }

        QJsonObject jsonObj = jsonDoc.object();
        QVariantMap variantMap = jsonObj.toVariantMap();

        file.close();
        return variantMap;
    }
};

class PublicAlgorithm : public QObject
{
    Q_OBJECT
public:
    explicit PublicAlgorithm(QObject *parent = nullptr);

    /**
     * @brief 计算标准差
     * @param values 数值列表
     * @param mean 平均值
     * @return 标准差
     */
    static double calculateStandardDeviation(const QVector<double>& values, double mean);


    static QVariantMap processAverageData(const QMap<QString, QList<QPair<QDateTime, double>>>& pMap,int i_avgType = 0);
    static QVariantMap processAverageDataAdvanced(const QMap<QString, QList<QPair<QDateTime, double>>>& pMap,
                                         const QMap<QString, int>& typeMap = QMap<QString, int>());
signals:

public slots:

private:

};

#endif // PUBLICALGORITHM_H
