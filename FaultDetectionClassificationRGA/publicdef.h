#ifndef PUBLICDEF_H
#define PUBLICDEF_H

#include <QObject>
#include <QVariant>
#include <QVariantMap>
#include <QMap>
#include <QDateTime>
#include <QStringList>
#include <QList>
#include <QComboBox>
#include <QDebug>
#include <QDir>
#include <QSettings>
#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <QQueue>
#include <QSet>


// 定义Channel信息结构
struct ChannelInfo {
    QString name;
    QString value;
    QString chamberName;
    QString equipmentGroupName;
    QString equipmentGroupIP;

    ChannelInfo() {}
    ChannelInfo(const QString& n, const QString& v, const QString& ch = "",
                const QString& eg = "", const QString& ip = "")
        : name(n), value(v), chamberName(ch), equipmentGroupName(eg), equipmentGroupIP(ip) {}

    QVariantMap toVariantMap()
    {
        QVariantMap dataMap;
        dataMap["name"] = name;
        dataMap["value"] = value;
        dataMap["chamberName"] = chamberName;
        dataMap["equipmentGroupName"] = equipmentGroupName;
        dataMap["equipmentGroupIP"] = equipmentGroupIP;

        return dataMap;
    }
};

// 定义EquipmentGroup信息结构
struct EquipmentGroupInfo {
    QString groupName;
    QString groupIP;
    QString groupNo;
    QString groupRecipe;
    QList<ChannelInfo> channels;

    QString info2String()
    {
        QStringList tmpList;
        tmpList.append(QString("-----groupInfo----------"));
        tmpList.append(QString("groupName:%1").arg(groupName));
        tmpList.append(QString("groupIP:%1").arg(groupIP));
        tmpList.append(QString("groupNo:%1").arg(groupNo));
        //tmpList.append(QString("groupRecipe:%1").arg(groupRecipe));
        tmpList.append(QString("-----channels-begin------------------"));
        foreach (ChannelInfo channel, channels) {
            QString name = channel.name;
            QString value = channel.value;
            QString chamberName = channel.chamberName;
            QString equipmentGroupName = channel.equipmentGroupName;
            QString equipmentGroupIP = channel.equipmentGroupIP;

            QString tmp = QString("name:%1,value:%2,chamberName:%3,equipmentGroupName:%4,equipmentGroupIP:%5")
                            .arg(name).arg(value).arg(chamberName).arg(equipmentGroupName).arg(equipmentGroupIP);

            tmpList.append(tmp);
        }
        tmpList.append("--------------channels-end-------------------");

        return tmpList.join("\n");
    }
};

// 解析Value字段为详细信息
struct ChannelValueInfo {
    QString gasName;        // 气体名称
    QString massChannels;   // 质量通道
    QString param1;         // 参数1 lcl
    QString param2;         // 参数2 ucl

    ChannelValueInfo() {}
    ChannelValueInfo(const QString& gas, const QString& mass, const QString& p1, const QString& p2)
        : gasName(gas), massChannels(mass), param1(p1), param2(p2) {}
};

class PublicDef : public QObject
{
    Q_OBJECT
public:
    explicit PublicDef(QObject *parent = nullptr);

signals:


public:
    // 清除当前节点选中状态[前提是已经chk的,即如果没有chk的不管，原先chk没有显示的也不管(否则这个会显示出没chk)]
    static void clearAllCheckState(QTreeWidgetItem* item) {
        if(item == nullptr) return;

        Qt::CheckState chk = item->checkState(0);
        if(chk == Qt::CheckState::Checked){
            item->setCheckState(0, Qt::Unchecked);
        }

        for(int i = 0; i < item->childCount(); ++i) {
            clearAllCheckState(item->child(i)); // 递归处理子节点
        }
    }
    // 逐步检查是否有重复元素（更高效，找到第一个重复就返回）
    static bool hasDuplicates(const QStringList& list)
    {
        QSet<QString> seen;
        for (const QString& item : list) {
            if (seen.contains(item)) {
                return true;  // 找到重复元素
            }
            seen.insert(item);
        }
        return false;  // 没有重复元素
    }

    static QList<QTreeWidgetItem*> traverseTreeBFS(QTreeWidget *tree)
    {
        QList<QTreeWidgetItem *> res;
        QQueue<QTreeWidgetItem*> queue;

        for (int i = 0; i < tree->topLevelItemCount(); ++i) {
            queue.enqueue(tree->topLevelItem(i));
            res.append(tree->topLevelItem(i));
        }


        while (!queue.isEmpty()) {
            QTreeWidgetItem *item = queue.dequeue();
            qDebug() << item->text(0);

            for (int i = 0; i < item->childCount(); ++i) {
                queue.enqueue(item->child(i));
                res.append(item->child(i));
            }
        }

        return res;
    }
    static QList<QTreeWidgetItem*> traverseTreeBFS(QTreeWidgetItem *treeItem)
    {
        QList<QTreeWidgetItem *> res;

        QQueue<QTreeWidgetItem*> queue;
        queue.enqueue(treeItem);
        res.append(treeItem);//结果包含自身

        while (!queue.isEmpty()) {
            QTreeWidgetItem *item = queue.dequeue();
            //qDebug() << "traverseTreeBFS:   "<< item->text(0);

            for (int i = 0; i < item->childCount(); ++i) {
                queue.enqueue(item->child(i));
                res.append(item->child(i));
            }
        }
        return res;
    }
    // 读取recipe文件夹中的配方中的JsCalculate
    static QMap<QString, QString> getIniFilesWithJsCalculate(const QString& recipeDir)
    {
        QMap<QString, QString> result;

        QDir dir(recipeDir);
        if (!dir.exists()) {
            return result;
        }

        // 遍历目录中的所有文件
        for (const QFileInfo& fileInfo : dir.entryInfoList(QDir::Files)) {
            QString filePath = fileInfo.absoluteFilePath();

            // 使用QSettings读取ini文件
            QSettings settings(filePath, QSettings::IniFormat);

            // 检查是否读取成功且包含Method.JsCalculate
            if (settings.status() == QSettings::NoError) {
                QString jsCalculateValue = settings.value("Method/JsCalculate").toString();
                if (!jsCalculateValue.isEmpty()) {
                    // 使用文件基础名作为键
                    result[fileInfo.completeBaseName()] = jsCalculateValue;
                }
            }
        }

        return result;
    }
    // 归类相连的相同数据
    static void groupConsecutiveItems(const QStringList& recipeRGAList,
                              QList<QStringList>& resList,
                              QList<QList<int>>& indexList)
    {
        resList.clear();
        indexList.clear();

        if (recipeRGAList.isEmpty()) {
            return;
        }

        // 初始化第一个分组
        QStringList currentGroup;
        QList<int> currentIndexGroup;

        QString currentString = recipeRGAList[0];
        currentGroup.append(currentString);
        currentIndexGroup.append(0);

        // 遍历剩余元素
        for (int i = 1; i < recipeRGAList.size(); ++i) {
            const QString& item = recipeRGAList[i];

            if (item == currentString) {
                // 相同元素，添加到当前分组
                currentGroup.append(item);
                currentIndexGroup.append(i);
            } else {
                // 不同元素，保存当前分组，开始新分组
                resList.append(currentGroup);
                indexList.append(currentIndexGroup);

                // 重置当前分组
                currentGroup.clear();
                currentIndexGroup.clear();

                currentString = item;
                currentGroup.append(currentString);
                currentIndexGroup.append(i);
            }
        }

        // 添加最后一个分组
        if (!currentGroup.isEmpty()) {
            resList.append(currentGroup);
            indexList.append(currentIndexGroup);
        }
    }

    static QPair<QList<QStringList>, QList<QList<int>>> groupConsecutiveItems(const QStringList& recipeRGAList)
    {
        QList<QStringList> resList;
        QList<QList<int>> indexList;

        if (recipeRGAList.isEmpty()) {
            return qMakePair(resList, indexList);
        }

        QStringList currentGroup;
        QList<int> currentIndexGroup;
        QString currentString = recipeRGAList[0];

        for (int i = 0; i < recipeRGAList.size(); ++i) {
            const QString& item = recipeRGAList[i];

            if (item == currentString) {
                currentGroup.append(item);
                currentIndexGroup.append(i);
            } else {
                resList.append(currentGroup);
                indexList.append(currentIndexGroup);

                currentGroup.clear();
                currentIndexGroup.clear();

                currentString = item;
                currentGroup.append(item);
                currentIndexGroup.append(i);
            }
        }

        // 添加最后一个分组
        resList.append(currentGroup);
        indexList.append(currentIndexGroup);

        return qMakePair(resList, indexList);
    }

    // 使用示例
    static void exampleUsage()
    {
        QStringList recipeRGAList = {"a","a","a","b","b","a","a","c","c","a","a","d","e","e","a","a"};

        // 方法一：使用void函数
        QList<QStringList> resList;
        QList<QList<int>> indexList;
        groupConsecutiveItems(recipeRGAList, resList, indexList);

        qDebug() << "分组结果:";
        for (int i = 0; i < resList.size(); ++i) {
            qDebug() << "分组" << i << ":" << resList[i] << "索引:" << indexList[i];
        }

        // 方法二：使用返回值
        auto result = groupConsecutiveItems(recipeRGAList);
        QList<QStringList> resList2 = result.first;
        QList<QList<int>> indexList2 = result.second;

        qDebug() << "\n使用返回值的分组结果:";
        for (int i = 0; i < resList2.size(); ++i) {
            qDebug() << "分组" << i << ":" << resList2[i] << "索引:" << indexList2[i];
        }
    }

    // 验证结果的函数
    static void verifyResult(const QStringList& originalList,
                     const QList<QStringList>& groupedLists,
                     const QList<QList<int>>& indexLists)
    {
        qDebug() << "=== 验证结果 ===";
        qDebug() << "原始列表长度:" << originalList.size();
        qDebug() << "分组数量:" << groupedLists.size();
        qDebug() << "索引分组数量:" << indexLists.size();

        int totalItems = 0;
        for (int i = 0; i < groupedLists.size(); ++i) {
            const QStringList& group = groupedLists[i];
            const QList<int>& indices = indexLists[i];

            totalItems += group.size();
            qDebug() << "分组" << i << ": 长度=" << group.size() << ", 内容=" << group << ", 索引=" << indices;

            // 验证索引是否正确
            for (int j = 0; j < indices.size(); ++j) {
                int index = indices[j];
                if (index >= 0 && index < originalList.size()) {
                    if (originalList[index] != group[j]) {
                        qDebug() << "  错误: 索引" << index << "应该是" << group[j] << "但实际是" << originalList[index];
                    }
                }
            }
        }

        qDebug() << "总元素数:" << totalItems;
        qDebug() << "================";
    }
    /*!
     * \brief formatComboboxItemText 请注意参数的实际意思，这边也与ini中nameRule对应
     * \param i_keys
     * \return
     */
    static QString formatComboboxItemText(const QString& dateTimeStr,const QString& recipeEQ,
                                          const QString& recipeRGA,const QString& waferID,
                                          const QString& lotID,const QString& slotID)
    {
        QString itemStr = QString("%1(EQ@%2)(RGA@%3)(WaferID@%4)(LotID@%5)(SlotID@%6)")
                .arg(dateTimeStr).arg(recipeEQ).arg(recipeRGA).arg(waferID).arg(lotID).arg(slotID);

        return itemStr;
    }

    // 返回QStringList（按%1到%6顺序）
    static QStringList extractParametersList(const QString& data)
    {
        //这边是到combobox中的样式，如果修改需要全部修改
        //MASS_DATA_%1(EQ@%2)(RGA@%3)(WaferID@%4)(LotID@%5)(SlotID@%6)
        //MASS_DATA_20250810115013(EQ@NULL)(RGA@P-DMD_Idle.tuneFile)(WaferID@AS02368#07)(LotID@AS02368)(SlotID@25)
        QStringList parameters;

        QRegularExpression regex("^([^()]+)\\(EQ@([^)]*)\\)\\(RGA@([^)]*)\\)\\(WaferID@([^)]*)\\)\\(LotID@([^)]*)\\)\\(SlotID@([^)]*)\\)$");

        QRegularExpressionMatch match = regex.match(data.trimmed());

        if (match.hasMatch()) {
            parameters << match.captured(1);  // %1 (时间戳字符串)
            parameters << match.captured(2);  // %2 (EQ)
            parameters << match.captured(3);  // %3 (RGA)
            parameters << match.captured(4);  // %4 (WaferID)
            parameters << match.captured(5);  // %5 (LotID)
            parameters << match.captured(6);  // %6 (SlotID)
        }

        return parameters;
    }

    // 获取QComboBox中的所有选项
    static QStringList getAllItems(QComboBox* comboBox)
    {
        QStringList items;
        for (int i = 0; i < comboBox->count(); ++i) {
            items.append(comboBox->itemText(i));
        }
        return items;
    }
    static int findDateTimeIndex(const QList<QPair<QDateTime, double>>& pntsList, const QDateTime& a)
    {
        if (pntsList.isEmpty()) {
            return -1;  // 列表为空，返回-1表示未找到
        }

        // 如果只有一个元素，直接返回0
        if (pntsList.size() == 1) {
            return 0;
        }

        // 二分查找优化版本（假设列表按时间排序）
        int left = 0;
        int right = pntsList.size() - 1;

        // 首先检查边界情况
        if (a <= pntsList.first().first) {
            return 0;  // a小于等于第一个时间点
        }

        if (a >= pntsList.last().first) {
            return pntsList.size() - 1;  // a大于等于最后一个时间点
        }

        // 二分查找
        while (left <= right) {
            int mid = left + (right - left) / 2;
            const QDateTime& midTime = pntsList[mid].first;

            if (midTime == a) {
                return mid;  // 精确匹配
            } else if (midTime < a) {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }

        // 没有找到精确匹配，找到最近的时间点
        // 此时 left > right，检查 left 和 right 哪个更近
        if (left >= pntsList.size()) {
            return pntsList.size() - 1;
        }

        if (right < 0) {
            return 0;
        }

        // 比较两个相邻点的距离
        qint64 leftDiff = qAbs(pntsList[left].first.toMSecsSinceEpoch() - a.toMSecsSinceEpoch());
        qint64 rightDiff = qAbs(a.toMSecsSinceEpoch() - pntsList[right].first.toMSecsSinceEpoch());

        return (leftDiff <= rightDiff) ? left : right;
    }

    // 线性查找版本（适用于小数据集或无序数据）
    static int findDateTimeIndexLinear(const QList<QPair<QDateTime, double>>& pntsList, const QDateTime& a)
    {
        if (pntsList.isEmpty()) {
            return -1;
        }

        int closestIndex = 0;
        qint64 minDiff = qAbs(pntsList[0].first.toMSecsSinceEpoch() - a.toMSecsSinceEpoch());

        for (int i = 1; i < pntsList.size(); ++i) {
            qint64 diff = qAbs(pntsList[i].first.toMSecsSinceEpoch() - a.toMSecsSinceEpoch());
            if (diff < minDiff) {
                minDiff = diff;
                closestIndex = i;
            }

            // 如果找到精确匹配，直接返回
            if (pntsList[i].first == a) {
                return i;
            }
        }

        return closestIndex;
    }

     // 区分精确匹配和近似匹配，可以使用这个版本
    static QPair<int, bool> findDateTimeIndexWithMatchInfo(const QList<QPair<QDateTime, double>>& pntsList, const QDateTime& a)
    {
        // 返回值：first是索引，second表示是否精确匹配
        if (pntsList.isEmpty()) {
            return qMakePair(-1, false);
        }

        int closestIndex = 0;
        qint64 minDiff = qAbs(pntsList[0].first.toMSecsSinceEpoch() - a.toMSecsSinceEpoch());

        // 检查是否精确匹配第一个元素
        if (pntsList[0].first == a) {
            return qMakePair(0, true);
        }

        for (int i = 1; i < pntsList.size(); ++i) {
            // 检查精确匹配
            if (pntsList[i].first == a) {
                return qMakePair(i, true);
            }

            qint64 diff = qAbs(pntsList[i].first.toMSecsSinceEpoch() - a.toMSecsSinceEpoch());
            if (diff < minDiff) {
                minDiff = diff;
                closestIndex = i;
            }
        }

        return qMakePair(closestIndex, false);
    }

public slots:

public:
    QVariantMap m_dataInfoMap;//规整后的CL数据
    //手动输入的控制限的上下两个值
    double m_uclValueManual = 1.0;
    double m_lclValueManual = -1.0;
    double m_min_x = 0;//x轴的最小值
    double m_max_x = 0;//x轴的最大值
    double m_min_y = 0;//y轴的最小值
    double m_max_y = 0;//y轴的最大值
};

class sPublicDefSingleton
{
public:
    static PublicDef* GetInstance(){
        static sPublicDefSingleton sInstance;
        return &(sInstance.m_publicDef);
    }
private:
    PublicDef m_publicDef;
    sPublicDefSingleton(){}
    virtual ~sPublicDefSingleton(){}
    sPublicDefSingleton(const sPublicDefSingleton&){}
    sPublicDefSingleton& operator=(const sPublicDefSingleton&){
        static sPublicDefSingleton instance;
        return instance;
    }
};

#endif // PUBLICDEF_H
