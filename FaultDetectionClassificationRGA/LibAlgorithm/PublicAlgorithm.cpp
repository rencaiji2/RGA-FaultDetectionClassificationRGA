#include "PublicAlgorithm.h"

PublicAlgorithm::PublicAlgorithm(QObject *parent) : QObject(parent)
{

}

/**
 * @brief 计算标准差
 * @param values 数值列表
 * @param mean 平均值
 * @return 标准差
 */
double PublicAlgorithm:: calculateStandardDeviation(const QVector<double>& values, double mean)
{
    if (values.isEmpty()) {
        return 0.0;
    }

    double sumSquaredDiff = 0.0;
    for (double value : values) {
        double diff = value - mean;
        sumSquaredDiff += diff * diff;
    }

    return std::sqrt(sumSquaredDiff / values.size());
}

/**
 * @brief 处理原始数据生成平均值映射
 * @param pMap 原始数据映射
 * @param i_avgType 平均值算法  0  整体平均, 1 差值平均
 * @return 平均值映射
 */
QVariantMap PublicAlgorithm::processAverageData(const QMap<QString, QList<QPair<QDateTime, double> > >& pMap,
    int i_avgType)
{
    QVariantMap avgMap;

    // 首先收集所有数据用于Total计算
    QList<QString> keys = pMap.keys();
    QVector<QVector<double>> allOrgYValues;
    int maxLength = 0;

    // 收集所有orgYValues
    for (const QString& name : keys) {
        const QList<QPair<QDateTime, double>>& dataList = pMap.value(name);
        QVector<double> orgYValues;
        for (const QPair<QDateTime, double>& point : dataList) {
            orgYValues.append(point.second);
        }
        allOrgYValues.append(orgYValues);
        if (orgYValues.size() > maxLength) {
            maxLength = orgYValues.size();
        }
    }

    // 计算Total数据
    QVector<double> orgYValuesTotal(maxLength, 0.0);
    for (int i = 0; i < maxLength; ++i) {
        for (const QVector<double>& values : allOrgYValues) {
            if (i < values.size()) {
                orgYValuesTotal[i] += values[i];
            }
        }
    }

    // 将Total数据转换为字符串列表
    QStringList orgYListTotal;
    for (double value : orgYValuesTotal) {
        orgYListTotal.append(QString::number(value));
    }

    // 遍历每个数据轴
    for (auto it = pMap.constBegin(); it != pMap.constEnd(); ++it) {
        QString name = it.key();
        const QList<QPair<QDateTime, double>>& dataList = it.value();

        // 创建该轴的结果对象
        QVariantMap axisResult;

        // 提取y值列表
        QStringList orgYList;
        QVector<double> orgYValues; // 用于计算的double向量

        for (const QPair<QDateTime, double>& point : dataList) {
            orgYList.append(QString::number(point.second));
            orgYValues.append(point.second);
        }

        // 设置原始y值列表
        axisResult["org_y_list"] = orgYList;

        // 计算处理类型和结果
        int avgType = i_avgType; // 默认类型0
        axisResult["avg_type"] = avgType;

        double avgValue = 0.0;
        QStringList yList;
        QVector<double> yValues; // 用于计算的double向量

        if (avgType == 0) {
            // 类型0：计算原始数据的平均值
            if (!orgYValues.isEmpty()) {
                double sum = 0.0;
                for (double value : orgYValues) {
                    sum += value;
                }
                avgValue = sum / orgYValues.size();

                // y_list就是org_y_list
                yList = orgYList;
                yValues = orgYValues;
            }
        }
        else if (avgType == 1) {
            // 类型1：计算相邻点差值的平均值
            if (orgYValues.size() >= 3) { // 至少需要3个点
                yValues.reserve(orgYValues.size() - 2);

                // 计算每个点与前后两点的绝对差
                for (int i = 1; i < orgYValues.size() - 1; ++i) {
                    double diff1 = std::abs(orgYValues[i] - orgYValues[i - 1]);
                    double diff2 = std::abs(orgYValues[i + 1] - orgYValues[i]);
                    double avgDiff = (diff1 + diff2) / 2.0;
                    yValues.append(avgDiff);
                    yList.append(QString::number(avgDiff));
                }

                // 计算y_list的平均值
                if (!yValues.isEmpty()) {
                    double sum = 0.0;
                    for (double value : yValues) {
                        sum += value;
                    }
                    avgValue = sum / yValues.size();
                }
            }
            else {
                // 数据点不足，使用原始数据
                avgValue = 0.0;
                yList = orgYList;
                for (double value : orgYValues) {
                    yValues.append(value);
                }
                avgType = 0; // 回退到类型0
                axisResult["avg_type"] = avgType;
            }
        }

        // 设置平均值
        axisResult["avg"] = avgValue;

        // 设置y_list
        axisResult["y_list"] = yList;

        // ============= 计算标准差和上下限 =============
        if (!yValues.isEmpty()) {
            // 计算标准差
            double standardDeviation = calculateStandardDeviation(yValues, avgValue);
            axisResult["standard_deviation"] = standardDeviation;

            axisResult["ucl"] = avgValue + 3.0 * standardDeviation;
            axisResult["lcl"] = avgValue - 3.0 * standardDeviation;
        }
        else {
            // 如果没有数据，设置默认值
            axisResult["standard_deviation"] = 0.0;
            axisResult["ucl"] = avgValue;
            axisResult["lcl"] = avgValue;
        }
        // ==================================================

        // 添加到结果映射
        avgMap[name] = axisResult;
    }

    // 添加Total数据到结果映射
    QVariantMap totalResult;
    totalResult["org_y_list"] = orgYListTotal;
    totalResult["avg_type"] = i_avgType;

    // 计算Total的处理结果
    double totalAvgValue = 0.0;
    QStringList totalYList;
    QVector<double> totalYValues;

    if (i_avgType == 0) {
        // 类型0：计算原始数据的平均值
        if (!orgYValuesTotal.isEmpty()) {
            double sum = 0.0;
            for (double value : orgYValuesTotal) {
                sum += value;
            }
            totalAvgValue = sum / orgYValuesTotal.size();
            totalYList = orgYListTotal;
            totalYValues = orgYValuesTotal;
        }
    }
    else if (i_avgType == 1) {
        // 类型1：计算相邻点差值的平均值
        if (orgYValuesTotal.size() >= 3) {
            for (int i = 1; i < orgYValuesTotal.size() - 1; ++i) {
                double diff1 = std::abs(orgYValuesTotal[i] - orgYValuesTotal[i - 1]);
                double diff2 = std::abs(orgYValuesTotal[i + 1] - orgYValuesTotal[i]);
                double avgDiff = (diff1 + diff2) / 2.0;
                totalYValues.append(avgDiff);
                totalYList.append(QString::number(avgDiff));
            }

            if (!totalYValues.isEmpty()) {
                double sum = 0.0;
                for (double value : totalYValues) {
                    sum += value;
                }
                totalAvgValue = sum / totalYValues.size();
            }
        }
        else {
            totalAvgValue = 0.0;
            totalYList = orgYListTotal;
            totalYValues = orgYValuesTotal;
        }
    }

    totalResult["avg"] = totalAvgValue;
    totalResult["y_list"] = totalYList;

    // 计算Total的标准差和控制限
    if (!totalYValues.isEmpty()) {
        double totalStandardDeviation = calculateStandardDeviation(totalYValues, totalAvgValue);
        totalResult["standard_deviation"] = totalStandardDeviation;
        totalResult["ucl"] = totalAvgValue + 3.0 * totalStandardDeviation;
        totalResult["lcl"] = totalAvgValue - 3.0 * totalStandardDeviation;
    }
    else {
        totalResult["standard_deviation"] = 0.0;
        totalResult["ucl"] = totalAvgValue;
        totalResult["lcl"] = totalAvgValue;
    }

    avgMap["ALL"] = totalResult;

    return avgMap;
}

/**
 * @brief 增强版本：支持自定义处理类型
 * @param pMap 原始数据映射
 * @param typeMap 每个轴的处理类型映射（可选，默认都为0）
 * @return 平均值映射
 */
QVariantMap PublicAlgorithm::processAverageDataAdvanced(const QMap<QString, QList<QPair<QDateTime, double>>>& pMap,
    const QMap<QString, int>& typeMap)
{
    QVariantMap avgMap;

    // 首先收集所有数据用于Total计算
    QList<QString> keys = pMap.keys();
    QVector<QVector<double>> allOrgYValues;
    int maxLength = 0;

    // 收集所有orgYValues
    for (const QString& name : keys) {
        const QList<QPair<QDateTime, double>>& dataList = pMap.value(name);
        QVector<double> orgYValues;
        for (const QPair<QDateTime, double>& point : dataList) {
            orgYValues.append(point.second);
        }
        allOrgYValues.append(orgYValues);
        if (orgYValues.size() > maxLength) {
            maxLength = orgYValues.size();
        }
    }

    // 计算Total数据
    QVector<double> orgYValuesTotal(maxLength, 0.0);
    for (int i = 0; i < maxLength; ++i) {
        for (const QVector<double>& values : allOrgYValues) {
            if (i < values.size()) {
                orgYValuesTotal[i] += values[i];
            }
        }
    }

    // 将Total数据转换为字符串列表
    QStringList orgYListTotal;
    for (double value : orgYValuesTotal) {
        orgYListTotal.append(QString::number(value, 'f', 6));
    }

    // 遍历每个数据轴
    for (auto it = pMap.constBegin(); it != pMap.constEnd(); ++it) {
        QString name = it.key();
        const QList<QPair<QDateTime, double>>& dataList = it.value();

        // 获取处理类型
        int avgType = typeMap.contains(name) ? typeMap[name] : 0;

        // 创建该轴的结果对象
        QVariantMap axisResult;
        axisResult["avg_type"] = avgType;

        // 提取y值列表
        QStringList orgYList;
        QVector<double> orgYValues;

        for (const QPair<QDateTime, double>& point : dataList) {
            orgYList.append(QString::number(point.second, 'f', 6));
            orgYValues.append(point.second);
        }

        // 设置原始y值列表
        axisResult["org_y_list"] = orgYList;

        double avgValue = 0.0;
        QStringList yList;
        QVector<double> yValues;

        if (avgType == 0) {
            // 类型0：计算原始数据的平均值
            if (!orgYValues.isEmpty()) {
                double sum = 0.0;
                for (double value : orgYValues) {
                    sum += value;
                }
                avgValue = sum / orgYValues.size();
                yList = orgYList;
                yValues = orgYValues;
            }
        }
        else if (avgType == 1) {
            // 类型1：计算相邻点差值
            if (orgYValues.size() >= 3) {
                // 计算每个点与前后两点的绝对差
                for (int i = 1; i < orgYValues.size() - 1; ++i) {
                    double diff1 = std::abs(orgYValues[i] - orgYValues[i - 1]);
                    double diff2 = std::abs(orgYValues[i + 1] - orgYValues[i]);
                    double avgDiff = (diff1 + diff2) / 2.0;
                    yValues.append(avgDiff);
                    yList.append(QString::number(avgDiff, 'f', 6));
                }

                // 计算y_list的平均值
                if (!yValues.isEmpty()) {
                    double sum = 0.0;
                    for (double value : yValues) {
                        sum += value;
                    }
                    avgValue = sum / yValues.size();
                }
            }
            else {
                // 数据点不足，使用原始数据
                avgValue = orgYValues.isEmpty() ? 0.0 : orgYValues.first();
                yList = orgYList;
                yValues = orgYValues;
            }
        }

        // 设置平均值
        axisResult["avg"] = avgValue;

        // 设置y_list
        axisResult["y_list"] = yList;

        // ========== 计算标准差和上下限 ==========
        if (!yValues.isEmpty()) {
            // 计算标准差
            double standardDeviation = calculateStandardDeviation(yValues, avgValue);
            axisResult["standard_deviation"] = standardDeviation;

            axisResult["ucl"] = avgValue + 3.0 * standardDeviation;
            axisResult["lcl"] = avgValue - 3.0 * standardDeviation;
        }
        else {
            // 如果没有数据，设置默认值
            axisResult["standard_deviation"] = 0.0;
            axisResult["ucl"] = avgValue;
            axisResult["lcl"] = avgValue;
        }
        // ========== 新增功能结束 ==========

        // 添加到结果映射
        avgMap[name] = axisResult;
    }

    // 添加Total数据到结果映射
    QVariantMap totalResult;
    totalResult["org_y_list"] = orgYListTotal;

    // 使用默认类型0处理Total数据（可以根据需要调整）
    int totalAvgType = 0;
    totalResult["avg_type"] = totalAvgType;

    // 计算Total的处理结果
    double totalAvgValue = 0.0;
    QStringList totalYList;
    QVector<double> totalYValues;

    if (totalAvgType == 0) {
        // 类型0：计算原始数据的平均值
        if (!orgYValuesTotal.isEmpty()) {
            double sum = 0.0;
            for (double value : orgYValuesTotal) {
                sum += value;
            }
            totalAvgValue = sum / orgYValuesTotal.size();
            totalYList = orgYListTotal;
            totalYValues = orgYValuesTotal;
        }
    }
    else if (totalAvgType == 1) {
        // 类型1：计算相邻点差值的平均值
        if (orgYValuesTotal.size() >= 3) {
            for (int i = 1; i < orgYValuesTotal.size() - 1; ++i) {
                double diff1 = std::abs(orgYValuesTotal[i] - orgYValuesTotal[i - 1]);
                double diff2 = std::abs(orgYValuesTotal[i + 1] - orgYValuesTotal[i]);
                double avgDiff = (diff1 + diff2) / 2.0;
                totalYValues.append(avgDiff);
                totalYList.append(QString::number(avgDiff, 'f', 6));
            }

            if (!totalYValues.isEmpty()) {
                double sum = 0.0;
                for (double value : totalYValues) {
                    sum += value;
                }
                totalAvgValue = sum / totalYValues.size();
            }
        }
        else {
            totalAvgValue = 0.0;
            totalYList = orgYListTotal;
            totalYValues = orgYValuesTotal;
        }
    }

    totalResult["avg"] = totalAvgValue;
    totalResult["y_list"] = totalYList;

    // 计算Total的标准差和控制限
    if (!totalYValues.isEmpty()) {
        double totalStandardDeviation = calculateStandardDeviation(totalYValues, totalAvgValue);
        totalResult["standard_deviation"] = totalStandardDeviation;
        totalResult["ucl"] = totalAvgValue + 3.0 * totalStandardDeviation;
        totalResult["lcl"] = totalAvgValue - 3.0 * totalStandardDeviation;
    }
    else {
        totalResult["standard_deviation"] = 0.0;
        totalResult["ucl"] = totalAvgValue;
        totalResult["lcl"] = totalAvgValue;
    }

    avgMap["ALL"] = totalResult;

    return avgMap;
}

///**
// * @brief 处理原始数据生成平均值映射
// * @param pMap 原始数据映射
// * @param i_avgType 平均值算法  0  整体平均, 1 差值平均
// * @return 平均值映射
// */
//QVariantMap PublicAlgorithm::processAverageData(const QMap<QString, QList<QPair<QDateTime, double> > > &pMap,
//                                                int i_avgType)
//{
//    QVariantMap avgMap;

//    // 遍历每个数据轴
//    for (auto it = pMap.constBegin(); it != pMap.constEnd(); ++it) {
//        QString name = it.key();
//        const QList<QPair<QDateTime, double>>& dataList = it.value();

//        // 创建该轴的结果对象
//        QVariantMap axisResult;

//        // 提取y值列表
//        QStringList orgYList;
//        QVector<double> orgYValues; // 用于计算的double向量

//        for (const QPair<QDateTime, double>& point : dataList) {
//            orgYList.append(QString::number(point.second));
//            orgYValues.append(point.second);
//        }

//        // 设置原始y值列表
//        axisResult["org_y_list"] = orgYList;

//        // 计算处理类型和结果
//        int avgType = i_avgType; // 默认类型0
//        axisResult["avg_type"] = avgType;

//        double avgValue = 0.0;
//        QStringList yList;
//        QVector<double> yValues; // 用于计算的double向量

//        if (avgType == 0) {
//            // 类型0：计算原始数据的平均值
//            if (!orgYValues.isEmpty()) {
//                double sum = 0.0;
//                for (double value : orgYValues) {
//                    sum += value;
//                }
//                avgValue = sum / orgYValues.size();

//                // y_list就是org_y_list
//                yList = orgYList;
//                yValues = orgYValues;
//            }
//        } else if (avgType == 1) {
//            // 类型1：计算相邻点差值的平均值
//            if (orgYValues.size() >= 3) { // 至少需要3个点
//                yValues.reserve(orgYValues.size() - 2);

//                // 计算每个点与前后两点的绝对差
//                for (int i = 1; i < orgYValues.size() - 1; ++i) {
//                    double diff1 = std::abs(orgYValues[i] - orgYValues[i-1]);
//                    double diff2 = std::abs(orgYValues[i+1] - orgYValues[i]);
//                    double avgDiff = (diff1 + diff2) / 2.0;
//                    yValues.append(avgDiff);
//                    yList.append(QString::number(avgDiff));
//                }

//                // 计算y_list的平均值
//                if (!yValues.isEmpty()) {
//                    double sum = 0.0;
//                    for (double value : yValues) {
//                        sum += value;
//                    }
//                    avgValue = sum / yValues.size();
//                }
//            } else {
//                // 数据点不足，使用原始数据
//                avgValue = 0.0;
//                yList = orgYList;
//                for (double value : orgYValues) {
//                    yValues.append(value);
//                }
//                avgType = 0; // 回退到类型0
//                axisResult["avg_type"] = avgType;
//            }
//        }

//        // 设置平均值
//        axisResult["avg"] = avgValue;

//        // 设置y_list
//        axisResult["y_list"] = yList;

//        // ============= 计算标准差和上下限 =============
//        if (!yValues.isEmpty()) {
//            // 计算标准差
//            double standardDeviation = calculateStandardDeviation(yValues, avgValue);
//            axisResult["standard_deviation"] = standardDeviation;

//            axisResult["ucl"] = avgValue + 3.0 * standardDeviation;
//            axisResult["lcl"] = avgValue - 3.0 * standardDeviation;
//        } else {
//            // 如果没有数据，设置默认值
//            axisResult["standard_deviation"] = 0.0;
//            axisResult["ucl"] = avgValue;
//            axisResult["lcl"] = avgValue;
//        }
//        // ==================================================

//        // 添加到结果映射
//        avgMap[name] = axisResult;
//    }

//    return avgMap;
//}

///**
// * @brief 增强版本：支持自定义处理类型
// * @param pMap 原始数据映射
// * @param typeMap 每个轴的处理类型映射（可选，默认都为0）
// * @return 平均值映射
// */
//QVariantMap PublicAlgorithm:: processAverageDataAdvanced(const QMap<QString, QList<QPair<QDateTime, double>>>& pMap,
//                                     const QMap<QString, int>& typeMap)
//{
//    QVariantMap avgMap;

//    // 遍历每个数据轴
//    for (auto it = pMap.constBegin(); it != pMap.constEnd(); ++it) {
//        QString name = it.key();
//        const QList<QPair<QDateTime, double>>& dataList = it.value();

//        // 获取处理类型
//        int avgType = typeMap.contains(name) ? typeMap[name] : 0;

//        // 创建该轴的结果对象
//        QVariantMap axisResult;
//        axisResult["avg_type"] = avgType;

//        // 提取y值列表
//        QStringList orgYList;
//        QVector<double> orgYValues;

//        for (const QPair<QDateTime, double>& point : dataList) {
//            orgYList.append(QString::number(point.second, 'f', 6));
//            orgYValues.append(point.second);
//        }

//        // 设置原始y值列表
//        axisResult["org_y_list"] = orgYList;

//        double avgValue = 0.0;
//        QStringList yList;
//        QVector<double> yValues;

//        if (avgType == 0) {
//            // 类型0：计算原始数据的平均值
//            if (!orgYValues.isEmpty()) {
//                double sum = 0.0;
//                for (double value : orgYValues) {
//                    sum += value;
//                }
//                avgValue = sum / orgYValues.size();
//                yList = orgYList;
//                yValues = orgYValues;
//            }
//        } else if (avgType == 1) {
//            // 类型1：计算相邻点差值
//            if (orgYValues.size() >= 3) {
//                // 计算每个点与前后两点的绝对差
//                for (int i = 1; i < orgYValues.size() - 1; ++i) {
//                    double diff1 = std::abs(orgYValues[i] - orgYValues[i-1]);
//                    double diff2 = std::abs(orgYValues[i+1] - orgYValues[i]);
//                    double avgDiff = (diff1 + diff2) / 2.0;
//                    yValues.append(avgDiff);
//                    yList.append(QString::number(avgDiff, 'f', 6));
//                }

//                // 计算y_list的平均值
//                if (!yValues.isEmpty()) {
//                    double sum = 0.0;
//                    for (double value : yValues) {
//                        sum += value;
//                    }
//                    avgValue = sum / yValues.size();
//                }
//            } else {
//                // 数据点不足，使用原始数据
//                avgValue = orgYValues.isEmpty() ? 0.0 : orgYValues.first();
//                yList = orgYList;
//                yValues = orgYValues;
//            }
//        }

//        // 设置平均值
//        axisResult["avg"] = avgValue;

//        // 设置y_list
//        axisResult["y_list"] = yList;

//        // ========== 计算标准差和上下限 ==========
//        if (!yValues.isEmpty()) {
//            // 计算标准差
//            double standardDeviation = calculateStandardDeviation(yValues, avgValue);
//            axisResult["standard_deviation"] = standardDeviation;

//            axisResult["ucl"] = avgValue + 3.0 * standardDeviation;
//            axisResult["lcl"] = avgValue - 3.0 * standardDeviation;
//        } else {
//            // 如果没有数据，设置默认值
//            axisResult["standard_deviation"] = 0.0;
//            axisResult["ucl"] = avgValue;
//            axisResult["lcl"] = avgValue;
//        }
//        // ========== 新增功能结束 ==========

//        // 添加到结果映射
//        avgMap[name] = axisResult;
//    }

//    return avgMap;
//}
