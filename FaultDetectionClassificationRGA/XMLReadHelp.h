#ifndef XMLREADHELP_H
#define XMLREADHELP_H

#include <QObject>

#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QMap>
#include <QList>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QDebug>

#include "publicdef.h"

class XMLReadHelp : public QObject
{
    Q_OBJECT
public:
    explicit XMLReadHelp(QObject *parent = nullptr);

    //chartFDC 配置中组的唯一键创建
    static QString createGroupKey(const QString& equipmentGroupNo , const QString& equipmentGroupName)
    {
        QString groupKey = equipmentGroupNo + "_" + equipmentGroupName;
        return groupKey;
    }

    // 解析XML文件并提取Channel信息
    static QList<ChannelInfo> parseChannelInfoFromXML(const QString& xmlContent)
    {
        QList<ChannelInfo> channelList;

        QDomDocument doc;
        QString errorMsg;
        int errorLine, errorColumn;

        if (!doc.setContent(xmlContent, &errorMsg, &errorLine, &errorColumn)) {
            qDebug() << "XML解析错误:" << errorMsg << "行:" << errorLine << "列:" << errorColumn;
            return channelList;
        }

        QDomElement root = doc.documentElement();
        if (root.tagName() != "EquipmentGroups") {
            qDebug() << "根节点不是EquipmentGroups";
            return channelList;
        }

        // 遍历EquipmentGroup节点
        QDomNode equipmentGroupNode = root.firstChild();
        while (!equipmentGroupNode.isNull()) {
            if (equipmentGroupNode.isElement() && equipmentGroupNode.nodeName() == "EquipmentGroup") {
                QDomElement equipmentGroupElement = equipmentGroupNode.toElement();

                QString equipmentGroupName = equipmentGroupElement.attribute("EquipmentGroupName");
                QString equipmentGroupIP = equipmentGroupElement.attribute("EquipmentGroupIP");
                QString equipmentGroupNo = equipmentGroupElement.attribute("EquipmentGroupNo");

                // 遍历Chamber节点
                QDomNode chamberNode = equipmentGroupNode.firstChild();
                while (!chamberNode.isNull()) {
                    if (chamberNode.isElement() && chamberNode.nodeName() == "Chamber") {
                        QDomElement chamberElement = chamberNode.toElement();
                        QString chamberName = chamberElement.attribute("ChamberName");

                        // 遍历Channel节点
                        QDomNode channelNode = chamberNode.firstChild();
                        while (!channelNode.isNull()) {
                            if (channelNode.isElement() && channelNode.nodeName() == "Channel") {
                                QDomElement channelElement = channelNode.toElement();

                                QString name = channelElement.attribute("Name");
                                QString value = channelElement.attribute("Value");

                                ChannelInfo channelInfo(name, value, chamberName,
                                                      equipmentGroupName, equipmentGroupIP);
                                channelList.append(channelInfo);
                            }
                            channelNode = channelNode.nextSibling();
                        }
                    }
                    chamberNode = chamberNode.nextSibling();
                }
            }
            equipmentGroupNode = equipmentGroupNode.nextSibling();
        }

        return channelList;
    }

    // 从文件解析XML
    static QList<ChannelInfo> parseChannelInfoFromFile(const QString& fileName)
    {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "无法打开文件:" << fileName;
            return QList<ChannelInfo>();
        }

        QString xmlContent = file.readAll();
        file.close();

        return parseChannelInfoFromXML(xmlContent);
    }

    // 按EquipmentGroup分组的解析函数
    static QMap<QString, EquipmentGroupInfo> parseEquipmentGroupsFromXML(const QString& xmlContent)
    {
        QMap<QString, EquipmentGroupInfo> equipmentGroupMap;

        QDomDocument doc;
        QString errorMsg;
        int errorLine, errorColumn;

        if (!doc.setContent(xmlContent, &errorMsg, &errorLine, &errorColumn)) {
            qDebug() << "XML解析错误:" << errorMsg << "行:" << errorLine << "列:" << errorColumn;
            return equipmentGroupMap;
        }

        QDomElement root = doc.documentElement();
        if (root.tagName() != "EquipmentGroups") {
            qDebug() << "根节点不是EquipmentGroups";
            return equipmentGroupMap;
        }

        // 遍历EquipmentGroup节点
        QDomNode equipmentGroupNode = root.firstChild();
        while (!equipmentGroupNode.isNull()) {
            if (equipmentGroupNode.isElement() && equipmentGroupNode.nodeName() == "EquipmentGroup") {
                QDomElement equipmentGroupElement = equipmentGroupNode.toElement();

                QString equipmentGroupNo = equipmentGroupElement.attribute("EquipmentGroupNo");
                QString equipmentGroupName = equipmentGroupElement.attribute("EquipmentGroupName");
                QString equipmentGroupIP = equipmentGroupElement.attribute("EquipmentGroupIP");
                QString equipmentGroupRecipe = equipmentGroupElement.attribute("EquipmentGroupRecipe");

                EquipmentGroupInfo groupInfo;
                groupInfo.groupNo = equipmentGroupNo;
                groupInfo.groupName = equipmentGroupName;
                groupInfo.groupIP = equipmentGroupIP;
                groupInfo.groupRecipe = equipmentGroupRecipe;

                // 遍历Chamber节点
                QDomNode chamberNode = equipmentGroupNode.firstChild();
                while (!chamberNode.isNull()) {
                    if (chamberNode.isElement() && chamberNode.nodeName() == "Chamber") {
                        QDomElement chamberElement = chamberNode.toElement();
                        QString chamberName = chamberElement.attribute("ChamberName");

                        // 遍历Channel节点
                        QDomNode channelNode = chamberNode.firstChild();
                        while (!channelNode.isNull()) {
                            if (channelNode.isElement() && channelNode.nodeName() == "Channel") {
                                QDomElement channelElement = channelNode.toElement();

                                QString name = channelElement.attribute("Name");
                                QString value = channelElement.attribute("Value");

                                ChannelInfo channelInfo(name, value, chamberName,
                                                      equipmentGroupName, equipmentGroupIP);
                                groupInfo.channels.append(channelInfo);
                            }
                            channelNode = channelNode.nextSibling();
                        }
                    }
                    chamberNode = chamberNode.nextSibling();
                }

                QString groupKey = createGroupKey(equipmentGroupNo,equipmentGroupName);
                equipmentGroupMap[groupKey] = groupInfo;
            }
            equipmentGroupNode = equipmentGroupNode.nextSibling();
        }

        return equipmentGroupMap;
    }

    // 按Chamber分组的Channel信息
    static QMap<QString, QList<ChannelInfo>> parseChannelsByChamber(const QString& xmlContent)
    {
        QMap<QString, QList<ChannelInfo>> chamberChannelMap;

        QDomDocument doc;
        QString errorMsg;
        int errorLine, errorColumn;

        if (!doc.setContent(xmlContent, &errorMsg, &errorLine, &errorColumn)) {
            qDebug() << "XML解析错误:" << errorMsg << "行:" << errorLine << "列:" << errorColumn;
            return chamberChannelMap;
        }

        QDomElement root = doc.documentElement();
        if (root.tagName() != "EquipmentGroups") {
            qDebug() << "根节点不是EquipmentGroups";
            return chamberChannelMap;
        }

        // 遍历所有节点提取Channel信息
        QDomNode equipmentGroupNode = root.firstChild();
        while (!equipmentGroupNode.isNull()) {
            if (equipmentGroupNode.isElement() && equipmentGroupNode.nodeName() == "EquipmentGroup") {
                QDomElement equipmentGroupElement = equipmentGroupNode.toElement();
                QString equipmentGroupName = equipmentGroupElement.attribute("EquipmentGroupName");
                QString equipmentGroupIP = equipmentGroupElement.attribute("EquipmentGroupIP");

                QDomNode chamberNode = equipmentGroupNode.firstChild();
                while (!chamberNode.isNull()) {
                    if (chamberNode.isElement() && chamberNode.nodeName() == "Chamber") {
                        QDomElement chamberElement = chamberNode.toElement();
                        QString chamberName = chamberElement.attribute("ChamberName");
                        QString chamberKey = equipmentGroupName + "_" + chamberName;

                        QList<ChannelInfo> channelList;

                        QDomNode channelNode = chamberNode.firstChild();
                        while (!channelNode.isNull()) {
                            if (channelNode.isElement() && channelNode.nodeName() == "Channel") {
                                QDomElement channelElement = channelNode.toElement();

                                QString name = channelElement.attribute("Name");
                                QString value = channelElement.attribute("Value");

                                ChannelInfo channelInfo(name, value, chamberName,
                                                      equipmentGroupName, equipmentGroupIP);
                                channelList.append(channelInfo);
                            }
                            channelNode = channelNode.nextSibling();
                        }

                        chamberChannelMap[chamberKey] = channelList;
                    }
                    chamberNode = chamberNode.nextSibling();
                }
            }
            equipmentGroupNode = equipmentGroupNode.nextSibling();
        }

        return chamberChannelMap;
    }

    static ChannelValueInfo parseChannelValue(const QString& value)
    {
        QStringList parts = value.split(',');
        ChannelValueInfo info;

        if (parts.size() >= 4) {
            info.gasName = parts[0];
            info.massChannels = parts[1];
            info.param1 = parts[2];
            info.param2 = parts[3];
        }

        return info;
    }

    // 使用示例
    static void exampleUsage()
    {
        QString xmlContent = R"(
            <EquipmentGroups>
                <EquipmentGroup EquipmentGroupNo="0" EquipmentGroupName="" EquipmentGroupRecipe="[{&quot;children&quot;:[{&quot;level&quot;:1,&quot;widgets&quot;:{&quot;0&quot;:&quot;Recipe&quot;,&quot;1&quot;:&quot;NULL&quot;,&quot;2&quot;:&quot;P-DMD_Idle.tuneFile&quot;}},{&quot;level&quot;:1,&quot;widgets&quot;:{&quot;0&quot;:&quot;Recipe&quot;,&quot;1&quot;:&quot;P-DMD15S.Degas.rcp&quot;,&quot;2&quot;:&quot;P-DMD15S.tuneFile&quot;}}],&quot;level&quot;:0,&quot;widgets&quot;:{&quot;0&quot;:&quot;192.168.0.195&quot;,&quot;1&quot;:&quot;&quot;,&quot;2&quot;:&quot;&quot;}}]" EquipmentGroupIP="127.0.0.1">
                    <Chamber ChamberName="CHE" ChamberIP="">
                        <Channel Value="N2/H2O,M28/M18,N,N" Name="N2/H2O"/>
                        <Channel Value="O2/H2O,M32/M18,1,2" Name="O2/H2O"/>
                        <Channel Value="N2/Ar,M28/M40,N,N" Name="N2/Ar"/>
                    </Chamber>
                    <Chamber ChamberName="TM1" ChamberIP="">
                        <Channel Value="N2/H2O,M28/M18,N,N" Name="N2/H2O"/>
                        <Channel Value="O2/H2O,M32/M18,1,2" Name="O2/H2O"/>
                        <Channel Value="N2/Ar,M28/M40,N,N" Name="N2/Ar"/>
                    </Chamber>
                </EquipmentGroup>
                <EquipmentGroup EquipmentGroupNo="1" EquipmentGroupName="测试服务器" EquipmentGroupRecipe="[]" EquipmentGroupIP="192.168.69.27">
                    <Chamber ChamberName="CHE" ChamberIP="">
                        <Channel Value="N2/H2O,M28/M18,N,N" Name="N2/H2O"/>
                        <Channel Value="O2/H2O,M32/M18,1,2" Name="O2/H2O"/>
                        <Channel Value="N2/Ar,M28/M40,N,N" Name="N2/Ar"/>
                    </Chamber>
                    <Chamber ChamberName="TM1" ChamberIP="">
                        <Channel Value="N2/H2O,M28/M18,N,N" Name="N2/H2O"/>
                        <Channel Value="O2/H2O,M32/M18,1,2" Name="O2/H2O"/>
                        <Channel Value="N2/Ar,M28/M40,N,N" Name="N2/Ar"/>
                    </Chamber>
                </EquipmentGroup>
            </EquipmentGroups>
            )";

        // 方法1：获取所有Channel信息
        QList<ChannelInfo> allChannels = parseChannelInfoFromXML(xmlContent);
        qDebug() << "总共找到" << allChannels.size() << "个Channel";

        for (const ChannelInfo& channel : allChannels) {
            qDebug() << "设备组:" << channel.equipmentGroupName
                     << "IP:" << channel.equipmentGroupIP
                     << "腔室:" << channel.chamberName
                     << "名称:" << channel.name
                     << "值:" << channel.value;

            // 解析Value详细信息
            ChannelValueInfo valueInfo = parseChannelValue(channel.value);
            qDebug() << "  气体:" << valueInfo.gasName
                     << "质量通道:" << valueInfo.massChannels
                     << "参数1:" << valueInfo.param1
                     << "参数2:" << valueInfo.param2;
        }

        // 方法2：按EquipmentGroup分组
        QMap<QString, EquipmentGroupInfo> equipmentGroups = parseEquipmentGroupsFromXML(xmlContent);
        qDebug() << "\n设备组数量:" << equipmentGroups.size();

        for (auto it = equipmentGroups.begin(); it != equipmentGroups.end(); ++it) {
            QString groupKey = it.key();
            EquipmentGroupInfo groupInfo = it.value();

            qDebug() << "设备组:" << groupKey
                     << "名称:" << groupInfo.groupName
                     << "IP:" << groupInfo.groupIP
                     << "包含" << groupInfo.channels.size() << "个Channel";

            for (const ChannelInfo& channel : groupInfo.channels) {
                qDebug() << "  腔室:" << channel.chamberName
                         << "Channel:" << channel.name
                         << "值:" << channel.value;
            }
        }

        // 方法3：按腔室分组
        QMap<QString, QList<ChannelInfo>> chamberChannels = parseChannelsByChamber(xmlContent);
        qDebug() << "\n腔室数量:" << chamberChannels.size();

        for (auto it = chamberChannels.begin(); it != chamberChannels.end(); ++it) {
            QString chamberKey = it.key();
            const QList<ChannelInfo>& channels = it.value();

            qDebug() << "腔室:" << chamberKey << "包含" << channels.size() << "个Channel";
            for (const ChannelInfo& channel : channels) {
                qDebug() << "  Channel:" << channel.name << "值:" << channel.value;
            }
        }
    }

signals:

public slots:
};

#endif // XMLREADHELP_H
