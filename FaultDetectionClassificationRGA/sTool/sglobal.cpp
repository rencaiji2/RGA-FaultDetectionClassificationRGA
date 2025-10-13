#include "sglobal.h"
#include <sobject.h>
#include <QSize>

bool srls::saveSObject(SObject *pObj, const QString &strType, const QString &strFileName, const QString &strComment, const QString &strAuthor)
{
    if(pObj == NULL
            || strFileName.isEmpty())
        return false;
    SObject *pObjs[2];
    bool bRet;
    pObjs[0] = new SObject();
    pObjs[0]->setObjectName("sobject");
    pObjs[0]->setProperty("author", strAuthor);
    pObjs[0]->setProperty("path", strFileName);
    pObjs[0]->setProperty("comment", strComment);
    pObjs[0]->setProperty("type", strType);
    pObj->lock().lockForRead();
    pObjs[1] = (SObject*)pObj->parent();
    pObj->setParent(pObjs[0]);
    bRet = pObjs[0]->saveFile(strFileName);
    pObj->setParent(pObjs[1]);
    pObj->lock().unlock();
    delete pObjs[0];
    return bRet;
}


SObject *srls::readSObject(const QString &strFileName, QString &strType, QString &strComment, QString &strAuthor, SObject *parent)
{
    if(strFileName.isEmpty())
        return NULL;
    SObject *pObjs[2];
    pObjs[0] = new SObject();
    if(!pObjs[0]->openFile(strFileName)
            || pObjs[0]->objectName() != "sobject"
            || pObjs[0]->children().isEmpty()){
        delete pObjs[0];
        return NULL;
    }
    strAuthor = pObjs[0]->property("author").toString();
    strComment = pObjs[0]->property("comment").toString();
    strType = pObjs[0]->property("type").toString();
    pObjs[1] = (SObject*)pObjs[0]->children().first();
    pObjs[1]->setParent(parent);
    delete pObjs[0];
    return pObjs[1];
}


bool srls::isValidName(const QString &strName)
{
    if(strName.isEmpty()
            || strName.startsWith('_')
            || strName.contains(' ')
            || strName.contains(':')
            || strName.contains('(')
            || strName.contains(')')
            || strName.contains('@')
            || strName.contains('.'))
        return false;
    return true;
}


int srls::splitNameTypeID(const QString &strSrc, QString &strName, QString &strType, uint &uID)
{
    if(strSrc.isEmpty())
        return -1;
    QStringList lst1,lst2;
    lst1 = strSrc.split('(');
    strName = lst1[0];
    if(lst1.size() < 2)
        return 1;
    lst2 = lst1[1].split(')');
    strType = lst2[0];
    if(lst2.size() < 2)
        return 2;
    uID = lst2[1].right(lst2[1].size() - lst2[1].indexOf(':')).toUInt();
    return 3;
}


QString srls::assignID(QObject* pSrcObj, const char* szIDName, const QString& strType, QMap<QString, QVariantMap> &mapExist, bool bInstance)
{
    static QMap<QString, int> mapMaxID;
    QString strOrg = pSrcObj->property(szIDName).toString();
    if(!strOrg.isEmpty()){
        if(bInstance)
            return strOrg + "_i";
        else if(!mapExist.contains(strOrg))
            return strOrg;
    }
    int nMaxID = 0;
    if(mapMaxID.contains(strType)){
        nMaxID = mapMaxID[strType];
    }
    QString strID,strFmt;
    strFmt = strType + "_%1";
    do{
        strID = strFmt.arg(nMaxID++);
    }while (mapExist.contains(strID));
    mapMaxID[strType] = nMaxID;
    pSrcObj->setProperty(szIDName, strID);
    return bInstance ? (strID + "_i") : strID;
}


void srls::analyseObjectID(QObject *pSrcObj, QMap<QString, QVariantMap> &mapDes, const QString strType)
{
    if(pSrcObj == NULL)
        return;
    QString strID;
    strID = assignID(pSrcObj, "_id", strType, mapDes, false);
    mapDes[strID]["type"] = strType;
    mapDes[strID]["value"] = QVariant::fromValue((void*)pSrcObj);

    auto lstChildren = pSrcObj->children();
    foreach (auto iterChild, lstChildren) {
        analyseObjectID(iterChild, mapDes, strType);
    }
}


void srls::vl2ba(const QVariantList &vl, QByteArray &ba)
{
    ba.clear();
    for(auto iterItem = vl.begin(); iterItem != vl.end(); iterItem++){
        ba.append((char)vl.first().toInt());
    }
}

void srls::ba2vl(const QByteArray &ba, QVariantList &vl)
{
    vl.clear();
    for(auto iterItem = ba.begin(); iterItem != ba.end(); iterItem++){
        vl.append((int)*iterItem);
    }
}

namespace srls{
namespace attribute {
SRLS_CS Limit = "limit";
SRLS_CS Tip = "tip";
SRLS_CS Role = "role";
SRLS_CS RoleParam = "role_param";
namespace role {
SRLS_CS SelectMul = "select_mul";
SRLS_CS SelectSignle = "select_signle";
SRLS_CS LayoutParam = "layout_param";
SRLS_CS LayoutValue = "layout_value";
SRLS_CS Image = "image";
SRLS_CS Action = "action";
SRLS_CS Map2Image = "map2image";
SRLS_CS Map2Action = "map2action";
SRLS_CS Map2Object = "map2object";
SRLS_CS Map2Property = "map2property";
}
}
}




void srls::addWidgetParam(SObject *pSrc)
{
    SObject* pParent = (SObject*)pSrc->parent();

    pSrc->setProperty("ui_show", true);
    pSrc->setProperty("ui_layout", "grid");
    pSrc->propertyInfo()["ui_layout"].attribute[attribute::Role] = attribute::role::SelectSignle;
    pSrc->propertyInfo()["ui_layout"].attribute[attribute::RoleParam] = SRLS_LAYOUTS;
    pSrc->setProperty("ui_layout_param", "");
    pSrc->propertyInfo()["ui_layout_param"].attribute[attribute::Role] = attribute::role::LayoutParam;
    pSrc->setProperty("ui_min_size", QSize(0, 0));
    pSrc->setProperty("ui_back_image", "");
    pSrc->propertyInfo()["ui_back_image"].attribute[attribute::Role] = attribute::role::Map2Image;
    if(pParent){
        QString strLayout,strType,strClass;

        strLayout = pParent->property("ui_layout").toString();
        if(!strLayout.isEmpty()){
            if(strLayout == "hbox"
                     || strLayout == "vbox"){
                pSrc->setProperty("_ui_layout", "0,0,0");
            }else if(strLayout == "grid"){
                pSrc->setProperty("_ui_layout", "0,0,1,1,0");
            }else if(strLayout == "form"){
                pSrc->setProperty("_ui_layout", "0,0");
            }else if(strLayout == "stack"){
                pSrc->setProperty("_ui_layout", "0");
            }else if(strLayout == "geometry"){
                pSrc->setProperty("_ui_layout", "1,1,100,60");
            }
            pSrc->propertyInfo()["_ui_layout"].attribute[attribute::Role] = attribute::role::LayoutValue;
        }

        strType = pParent->property("__type").toString();
        if(strType == "dock"){
            pSrc->setProperty("_ui_area", "left");
            pSrc->propertyInfo()["_ui_area"].attribute[attribute::Role] = attribute::role::SelectSignle;
            pSrc->propertyInfo()["_ui_area"].attribute[attribute::RoleParam] = "left,right,bottom,top";
            pSrc->setProperty("_ui_index", 0);
            pSrc->setProperty("_ui_title", "dock");
            pSrc->setProperty("_ui_floating", false);
            pSrc->setProperty("_ui_closable", true);
            pSrc->setProperty("_ui_allowed_areas", "all");
            pSrc->propertyInfo()["_ui_allowed_areas"].attribute[attribute::Role] = attribute::role::SelectMul;
            pSrc->propertyInfo()["_ui_allowed_areas"].attribute[attribute::RoleParam] = "left,right,bottom,top";
            return;
        }else if(strType == "container"){
            strClass = pParent->property("ui_class").toString();
            if(strClass == "split"){
                pSrc->setProperty("_ui_layout", "0");
            }else if(strClass == "tab"){
                pSrc->setProperty("_ui_layout", "0");
                pSrc->setProperty("_ui_title", "tab");
                pSrc->setProperty("_ui_icon", "");
                pSrc->propertyInfo()["_ui_icon"].attribute[attribute::Role] = attribute::role::Map2Image;
            }
            pSrc->propertyInfo()["_ui_layout"].attribute[attribute::Role] = attribute::role::LayoutValue;
        }else if(strType == "statusbar"){
            pSrc->setProperty("_ui_index", 0);
            pSrc->setProperty("_ui_stretch", 0);
            pSrc->setProperty("_ui_left", true);
        }
    }
}


SObject* srls::createContainerSObject(const QString& strType, const QString& strName, QMap<QString, QVariantMap> &mapIDs, SObject* pParent)
{
    SObject* pObjs[5];
    QString strID;
    pObjs[0] = pParent;
    pObjs[1] = new SObject(pObjs[0]);
    pObjs[1]->setObjectName(strName);
    pObjs[1]->setProperty("__type", "container");
    pObjs[1]->setProperty("ui_class", strType);
    strID = assignID(pObjs[1], "_id", "container", mapIDs);
    mapIDs[strID]["type"] = "sobject";
    mapIDs[strID]["value"] = QVariant::fromValue((void*)pObjs[1]);
    addWidgetParam(pObjs[1]);
    if(strType == "widget"
            || strType == "scroll"){
    }else if(strType == "split"){
        pObjs[1]->setProperty("ui_orientation", "horizontal");
        pObjs[1]->propertyInfo()["ui_orientation"].attribute[attribute::Role] = attribute::role::SelectSignle;
        pObjs[1]->propertyInfo()["ui_orientation"].attribute[attribute::RoleParam] = "horizontal,vertical";
    }else if(strType == "tab"){
        pObjs[1]->setProperty("ui_closable", true);
        pObjs[1]->setProperty("ui_alignment", "top");
        pObjs[1]->propertyInfo()["ui_alignment"].attribute[attribute::Role] = attribute::role::SelectSignle;
        pObjs[1]->propertyInfo()["ui_alignment"].attribute[attribute::RoleParam] = "left,right,top";
    }else if(strType == "group"){
        pObjs[1]->setProperty("ui_title", "group");
        pObjs[1]->setProperty("ui_alignment", "top");
        pObjs[1]->propertyInfo()["ui_alignment"].attribute[attribute::Role] = attribute::role::SelectSignle;
        pObjs[1]->propertyInfo()["ui_alignment"].attribute[attribute::RoleParam] = "left,right,top";
    }
    return pObjs[1];
}

SObject* srls::createProjectSObject(QMap<QString, QVariantMap> &mapIDs, const QString& strProjectName)
{
    SObject *pObjs[5];

    pObjs[0] = new SObject();
    pObjs[0]->setObjectName("sobject");
    pObjs[0]->setProperty("author", "SRLS");
    pObjs[0]->setProperty("path", "");
    pObjs[0]->setProperty("comment", "SRLS project");
    pObjs[0]->setProperty("type", "instrument");
    pObjs[0]->setProperty("_id", "sobject");
    mapIDs["sobject"]["type"] = "sobject";
    mapIDs["sobject"]["value"] = QVariant::fromValue((void*)pObjs[0]);

    pObjs[1] = new SObject(pObjs[0]);
    pObjs[1]->setObjectName("instrument");
    pObjs[1]->setProperty("name", strProjectName);
    pObjs[1]->setProperty("comment", "project");
    pObjs[1]->setProperty("icon", "");
    pObjs[1]->propertyInfo()["icon"].attribute[attribute::Role] = attribute::role::Map2Image;
    pObjs[1]->setProperty("init_action", "");
    pObjs[1]->propertyInfo()["init_action"].attribute[attribute::Role] = attribute::role::Map2Action;
    pObjs[1]->setProperty("release_action", "");
    pObjs[1]->propertyInfo()["release_action"].attribute[attribute::Role] = attribute::role::Map2Action;
    pObjs[1]->setProperty("timer_interval", (uint)1000);
    pObjs[1]->setProperty("timer_counter", uint(0));
    pObjs[1]->setProperty("_id", "instrument");
    mapIDs["instrument"]["type"] = "sobject";
    mapIDs["instrument"]["value"] = QVariant::fromValue((void*)pObjs[1]);

    pObjs[2] = new SObject(pObjs[1]);
    pObjs[2]->setObjectName("communication");
    pObjs[2]->setProperty("_id", "communication");
    mapIDs["communication"]["type"] = "sobject";
    mapIDs["communication"]["value"] = QVariant::fromValue((void*)pObjs[2]);

    pObjs[2] = new SObject(pObjs[1]);
    pObjs[2]->setObjectName("data");
    pObjs[2]->setProperty("_id", "data");
    mapIDs["data"]["type"] = "sobject";
    mapIDs["data"]["value"] = QVariant::fromValue((void*)pObjs[2]);

    pObjs[2] = new SObject(pObjs[1]);
    pObjs[2]->setObjectName("resource");
    pObjs[2]->setProperty("_id", "resource");
    mapIDs["resource"]["type"] = "sobject";
    mapIDs["resource"]["value"] = QVariant::fromValue((void*)pObjs[2]);

    pObjs[3] = new SObject(pObjs[2]);
    pObjs[3]->setObjectName("icon");
    pObjs[3]->setProperty("_id", "icon");
    mapIDs["icon"]["type"] = "sobject";
    mapIDs["icon"]["value"] = QVariant::fromValue((void*)pObjs[3]);
    pObjs[3] = new SObject(pObjs[2]);
    pObjs[3]->setObjectName("action");
    pObjs[3]->setProperty("_id", "action");
    mapIDs["action"]["type"] = "sobject";
    mapIDs["action"]["value"] = QVariant::fromValue((void*)pObjs[3]);

    pObjs[2] = new SObject(pObjs[1]);
    pObjs[2]->setObjectName("ui");
    pObjs[2]->setProperty("show", "max");
    pObjs[2]->propertyInfo()["show"].attribute[attribute::Tip] = SObject::tr("[max] | [normal] | [width,height]");
    pObjs[2]->setProperty("show_toolbar", true);
    pObjs[2]->setProperty("show_statusbar", true);
    pObjs[2]->setProperty("_id", "ui");
    mapIDs["ui"]["type"] = "sobject";
    mapIDs["ui"]["value"] = QVariant::fromValue((void*)pObjs[2]);

    pObjs[3] = new SObject(pObjs[2]);
    pObjs[3]->setObjectName("menu");
    pObjs[3]->setProperty("_id", "action");
    mapIDs["action"]["type"] = "sobject";
    mapIDs["action"]["value"] = QVariant::fromValue((void*)pObjs[3]);

    pObjs[3] = new SObject(pObjs[2]);
    pObjs[3]->setObjectName("central");
    pObjs[3]->setProperty("ui_show", true);
    pObjs[3]->setProperty("ui_layout", "vbox");
    pObjs[3]->setProperty("_id", "central");
    mapIDs["central"]["type"] = "sobject";
    mapIDs["central"]["value"] = QVariant::fromValue((void*)pObjs[3]);

    pObjs[3] = new SObject(pObjs[2]);
    pObjs[3]->setObjectName("dock");
    pObjs[3]->setProperty("_id", "dock");
    mapIDs["dock"]["type"] = "sobject";
    mapIDs["dock"]["value"] = QVariant::fromValue((void*)pObjs[3]);

    pObjs[3] = new SObject(pObjs[2]);
    pObjs[3]->setObjectName("pop_widget");
    pObjs[3]->setProperty("_id", "pop_widget");
    mapIDs["pop_widget"]["type"] = "sobject";
    mapIDs["pop_widget"]["value"] = QVariant::fromValue((void*)pObjs[3]);

    pObjs[3] = new SObject(pObjs[2]);
    pObjs[3]->setObjectName("statusbar");
    pObjs[3]->setProperty("_id", "statusbar");
    mapIDs["statusbar"]["type"] = "sobject";
    mapIDs["statusbar"]["value"] = QVariant::fromValue((void*)pObjs[3]);

    return pObjs[0];
}
