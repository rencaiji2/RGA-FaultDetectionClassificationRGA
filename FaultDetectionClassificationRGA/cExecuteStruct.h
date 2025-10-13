#ifndef CEXECUTESTRUCT_H
#define CEXECUTESTRUCT_H

#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMap>

struct cExecuteStruct{
    enum _TYPE_ACTION:quint32{_NULL_ACTION, _query_ACTION, _recipeConfig_ACTION, _recipeListEQ_ACTION, _recipeListRGA_ACTION,
                              };
    static bool Array2Json(const QByteArray &data, QJsonObject& JsonObject){
        if(data.isEmpty())
            return false;
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
        if (parseError.error != QJsonParseError::NoError)
            return false;
        if (!doc.isObject())
            return false;
        JsonObject= doc.object();
        if(JsonObject.isEmpty())
            return false;
        return true;
    }
    static QByteArray createErrorServer(QString& action){
        QJsonObject response;
        response["action"] = action;
        response["status"] = "error";
        response["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
        response["message"] = "Unknown action";

        QJsonDocument doc(response);
        return doc.toJson(QJsonDocument::Compact) + "\n";
    }
    static _TYPE_ACTION getAction(const QByteArray& data, QJsonObject &request, QString& action){
        if (data.isEmpty())
            return _NULL_ACTION;
        if(!Array2Json(data, request))
            return _NULL_ACTION;
        action= request.value("action").toString();
        if(action== "query"){
            return _query_ACTION;
        }else if(action== "recipeConfig"){
            return _recipeConfig_ACTION;
        }else if(action== "recipeListEQ"){
            return _recipeListEQ_ACTION;
        }else if(action== "recipeListRGA"){
            return _recipeListRGA_ACTION;
        }else {
            return _NULL_ACTION;
        }
    }
    /**
     * @brief createQueryClient
     * @param keyEquipmentGroup
     * @param startDate
     * @param endDate
     * @param chamber
     * @return
     */
    static QByteArray createQueryClient(const QString& keyEquipmentGroup, const QString& RecipeEQ,
                                        const QDate& startDate, const QDate& endDate, const QString& lotID,
                                        const QString& waferID, const QString& chamberID,
                                        QMap<QString, QStringList>& chamber){
        QJsonObject EquipmentGroup;
        EquipmentGroup["EquipmentGroupName"] = keyEquipmentGroup;
        EquipmentGroup["RecipeEQ"] = RecipeEQ;
        EquipmentGroup["startDate"] = startDate.toString("yyyy-MM-dd");
        EquipmentGroup["endDate"] = endDate.toString("yyyy-MM-dd");
        EquipmentGroup["lotID"] = lotID;
        EquipmentGroup["waferID"] = waferID;
        EquipmentGroup["chamberID"] = chamberID;
        for(auto& keyChamber: chamber.keys()){
            QJsonObject Chamber;
            Chamber["Channel"] = chamber[keyChamber].join(";");
            EquipmentGroup[keyChamber]=Chamber;
        }

        QJsonObject request;
        request["action"] = "query";
        request["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
        request["params"] = EquipmentGroup;
        QJsonDocument doc(request);
        return doc.toJson(QJsonDocument::Compact) + "\n";
    }
    static bool parsedQueryServer(const QJsonObject &request, QString& EquipmentGroupName,QString& RecipeEQ,
                        QDate& startDate, QDate& endDate, QString& lotID,
                                  QString& waferID, QString& chamberID, QMap<QString, QString>& chamber){
        chamber.clear();
        QDateTime DateTime= QDateTime::fromString(request.value("timestamp").toString(), Qt::ISODate);
        QJsonObject EquipmentGroup = request.value("params").toObject();
        EquipmentGroupName = EquipmentGroup.value("EquipmentGroupName").toString();
        RecipeEQ = EquipmentGroup.value("RecipeEQ").toString();
        startDate = QDate::fromString(EquipmentGroup.value("startDate").toString(), "yyyy-MM-dd");
        endDate = QDate::fromString(EquipmentGroup.value("endDate").toString(), "yyyy-MM-dd");
        lotID= EquipmentGroup.value("lotID").toString();
        waferID= EquipmentGroup.value("waferID").toString();
        chamberID= EquipmentGroup.value("chamberID").toString();

        for (auto it = EquipmentGroup.begin(); it != EquipmentGroup.end(); ++it) {
            if (!it.value().isObject())
                continue;
            QJsonObject Chamber = it.value().toObject();
            chamber[it.key()] = Chamber.value("Channel").toString();
        }
        return true;
    }
    static QByteArray createQueryServer(QJsonObject& response, const QString& EquipmentGroupName){
        response["action"] = "query";
        response["status"] = "success";
        response["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
        response["EquipmentGroupName"] = EquipmentGroupName;
        QJsonDocument doc(response);
        return doc.toJson(QJsonDocument::Compact) + "\n";
    }
    static bool parsedQueryClient(const QByteArray& data, QJsonObject& request, QString& EquipmentGroupName){
        if(!cExecuteStruct::Array2Json(data, request))
            return false;
        if(request.value("status").toString()!= "success")
            return false;
        EquipmentGroupName= request.value("EquipmentGroupName").toString();
        QDateTime DateTime= QDateTime::fromString(request.value("timestamp").toString(), Qt::ISODate);
        return true;
    }
    /**
     * @brief createRecipeListEQ_Client
     * @param keyEquipmentGroup
     * @param ip
     * @return
     */
    static QByteArray createRecipeListEQ_Client(QString& keyEquipmentGroup, QString& ip){
//        QJsonObject EquipmentGroup;
//        EquipmentGroup["EquipmentGroupName"] = keyEquipmentGroup;
//        EquipmentGroup["ip"] = ip;

        QJsonObject request;
        request["action"] = "recipeListEQ";
        request["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
        request["EquipmentGroupName"] = keyEquipmentGroup;
        request["ip"] = ip;
        //request["params"] = EquipmentGroup;
        QJsonDocument doc(request);
        return doc.toJson(QJsonDocument::Compact) + "\n";
    }
    static bool parsedRecipeListEQ_Server(const QJsonObject &request, QString& EquipmentGroupName,
                        QString& ip){
        QDateTime DateTime= QDateTime::fromString(request.value("timestamp").toString(), Qt::ISODate);
        //QJsonObject EquipmentGroup = request.value("params").toObject();
        EquipmentGroupName = request.value("EquipmentGroupName").toString();
        ip = request.value("ip").toString();
        return true;
    }
    static QByteArray createRecipeListEQ_Server(const QString& EquipmentGroupName, const QString& ip,
                                                const QStringList& RecipeListEQ){
        QJsonObject response;
        response["action"] = "recipeListEQ";
        response["status"] = "success";
        response["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
        response["EquipmentGroupName"] = EquipmentGroupName;
        response["ip"] = ip;
        response["data"] = RecipeListEQ.join(";");
        QJsonDocument doc(response);
        return doc.toJson(QJsonDocument::Compact) + "\n";
    }
    static bool parsedRecipeListEQ_Client(const QByteArray& data, QJsonObject& request, QString& EquipmentGroupName, QString& ip,
                                          QStringList& RecipeListEQ){
        RecipeListEQ.clear();
        if(!cExecuteStruct::Array2Json(data, request))
            return false;
        if(request.value("status").toString()!= "success")
            return false;
        QDateTime DateTime= QDateTime::fromString(request.value("timestamp").toString(), Qt::ISODate);
        EquipmentGroupName= request.value("EquipmentGroupName").toString();
        ip= request.value("ip").toString();
        RecipeListEQ= request.value("data").toString().split(";");
        return true;
    }
    /**
     * @brief createRecipeListRGA_Client
     * @param keyEquipmentGroup
     * @param ip
     * @return
     */
    static QByteArray createRecipeListRGA_Client(QString& keyEquipmentGroup, QString& ip){
//        QJsonObject EquipmentGroup;
//        EquipmentGroup["EquipmentGroupName"] = keyEquipmentGroup;
//        EquipmentGroup["ip"] = ip;

        QJsonObject request;
        request["action"] = "recipeListRGA";
        request["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
        request["EquipmentGroupName"] = keyEquipmentGroup;
        request["ip"] = ip;
        //request["params"] = EquipmentGroup;
        QJsonDocument doc(request);
        return doc.toJson(QJsonDocument::Compact) + "\n";
    }
    static bool parsedRecipeListRGA_Server(const QJsonObject &request, QString& EquipmentGroupName,
                        QString& ip){
        QDateTime DateTime= QDateTime::fromString(request.value("timestamp").toString(), Qt::ISODate);
        //QJsonObject EquipmentGroup = request.value("params").toObject();
        EquipmentGroupName = request.value("EquipmentGroupName").toString();
        ip = request.value("ip").toString();
        return true;
    }
    static QByteArray createRecipeListRGA_Server(const QString& EquipmentGroupName, const QString& ip,
                                                const QStringList& RecipeListEQ){
        QJsonObject response;
        response["action"] = "recipeListRGA";
        response["status"] = "success";
        response["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
        response["EquipmentGroupName"] = EquipmentGroupName;
        response["ip"] = ip;
        response["data"] = RecipeListEQ.join(";");
        QJsonDocument doc(response);
        return doc.toJson(QJsonDocument::Compact) + "\n";
    }
    static bool parsedRecipeListRGA_Client(const QByteArray& data, QJsonObject& request, QString& EquipmentGroupName, QString& ip,
                                          QStringList& RecipeListEQ){
        RecipeListEQ.clear();
        if(!cExecuteStruct::Array2Json(data, request))
            return false;
        if(request.value("status").toString()!= "success")
            return false;
        QDateTime DateTime= QDateTime::fromString(request.value("timestamp").toString(), Qt::ISODate);
        EquipmentGroupName= request.value("EquipmentGroupName").toString();
        ip= request.value("ip").toString();
        RecipeListEQ= request.value("data").toString().split(";");
        return true;
    }
};

#endif // CEXECUTESTRUCT_H
