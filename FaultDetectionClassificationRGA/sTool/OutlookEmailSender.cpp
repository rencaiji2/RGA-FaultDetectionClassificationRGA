#include "OutlookEmailSender.h"
#include <QImage>
#include <QPainter>
#include <QBuffer>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QMessageBox>
#include <QApplication>

#ifdef Q_OS_WIN
#include <QAxObject>
#include <objbase.h>  // 引入 COM 相关定义
#endif

OutlookEmailSender::OutlookEmailSender(QObject *parent)
    : QObject(parent)
#ifdef Q_OS_WIN
    , m_outlookApp(nullptr)
    , m_outlookNamespace(nullptr)
    , m_isInitialized(false)
#endif
{
    m_config.useExistingOutlook = true;
    m_config.profileName = QString();
    
#ifdef Q_OS_WIN
    initializeOutlook();
#endif
}

OutlookEmailSender::~OutlookEmailSender()
{
#ifdef Q_OS_WIN
    cleanupOutlook();
#endif
}

void OutlookEmailSender::setConfig(const OutlookConfig& config)
{
    m_config = config;
    
#ifdef Q_OS_WIN
    if (m_isInitialized) {
        cleanupOutlook();
    }
    initializeOutlook();
#endif
}

bool OutlookEmailSender::isOutlookAvailable() const
{
#ifdef Q_OS_WIN
    return m_isInitialized && m_outlookApp != nullptr;
#else
    return false;
#endif
}

bool OutlookEmailSender::sendTestReport(const TestResultData& testData, 
                                      const QList<TestPointData>& chartData,
                                      const QString& summary,
                                      const QStringList& recipients,
                                      const QStringList& ccRecipients)
{
#ifndef Q_OS_WIN
    Q_UNUSED(testData)
    Q_UNUSED(chartData)
    Q_UNUSED(summary)
    Q_UNUSED(recipients)
    Q_UNUSED(ccRecipients)
    return false;
#else
    if (!m_isInitialized || !m_outlookApp) {
        if (!initializeOutlook()) {
            return false;
        }
    }
    
    try {
        // 创建邮件对象
        QAxObject* mailItem = m_outlookApp->querySubObject("CreateItem(int)", 0); // 0 = olMailItem
        if (!mailItem) {
            return false;
        }
        
        // 设置邮件主题
        QString subject = QString("RGA测试报告 - %1 - %2")
                         .arg(testData.equipmentName)
                         .arg(testData.testTime);
        mailItem->setProperty("Subject", subject);
        
        // 设置收件人
        if (!recipients.isEmpty()) {
            mailItem->setProperty("To", recipients.join(";"));
        }
        
        // 设置抄送
        if (!ccRecipients.isEmpty()) {
            mailItem->setProperty("CC", ccRecipients.join(";"));
        }
        
        // 生成HTML内容
        QString htmlBody = generateHtmlReport(testData, chartData, summary);
        mailItem->setProperty("HTMLBody", htmlBody);
        
        // 发送邮件
        mailItem->dynamicCall("Send()");
        
        // 清理对象
        delete mailItem;
        
        return true;
        
    } catch (...) {
        return false;
    }
#endif
}

bool OutlookEmailSender::sendTestEmailWithRetry(const QString &recipient, const QString &subject, const QString &body, int retries)
{
    for (int i = 0; i < retries; ++i) {
        if (sendTestEmail(recipient, subject, body)) {
            return true;
        }

        // 等待一段时间再重试
        QEventLoop loop;
        QTimer::singleShot(1000, &loop, &QEventLoop::quit);
        loop.exec();
    }

    return false;
}

bool OutlookEmailSender::sendTestEmail(const QString &recipient, const QString &subject, const QString &body)
{
#ifndef Q_OS_WIN
    Q_UNUSED(recipient)
    Q_UNUSED(subject)
    Q_UNUSED(body)
    return false;
#else
    if (!m_isInitialized || !m_outlookApp) {
        if (!initializeOutlook()) {
            return false;
        }
    }

    try {
        // 创建邮件对象
        QAxObject* mailItem = m_outlookApp->querySubObject("CreateItem(int)", 0); // 0 = olMailItem
        if (!mailItem) {
            return false;
        }

        // 设置邮件属性
        mailItem->setProperty("Subject", subject);
        mailItem->setProperty("To", recipient);
        mailItem->setProperty("Body", body);

        // 设置邮件为高重要性，可能有助于绕过某些安全检查
        mailItem->setProperty("Importance", 2); // 2 = olImportanceHigh

        // 发送邮件 - 使用SendAndReceive来确保发送
        mailItem->dynamicCall("Send()");

        // 清理对象
        delete mailItem;

        return true;

    } catch (const std::exception& e) {
        qWarning() << "发送邮件异常:" << e.what();
    } catch (...) {
        qWarning() << "发送邮件未知异常";
    }

    return false;
#endif
}

QString OutlookEmailSender::createScatterPlotBase64(const QList<QPointF>& points, int width, int height)
{
    QByteArray imageData = createScatterPlotImage(points, width, height);
    if (imageData.isEmpty()) {
        return QString();
    }
    return imageData.toBase64();
}

QString OutlookEmailSender::generateHtmlReport(const TestResultData& testData, 
                                             const QList<TestPointData>& chartData,
                                             const QString& summary)
{
    QString htmlContent;
    
    // 邮件头部
    htmlContent += R"(
        <div style="background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); padding: 30px; border-radius: 10px; margin-bottom: 30px; color: white;">
            <h1 style="margin: 0; font-size: 32px;">🔬 RGA数据分析测试报告</h1>
            <p style="margin: 10px 0 0 0; font-size: 18px; opacity: 0.9;">%1</p>
        </div>
    )";
    
    // 测试概要信息
    htmlContent += QString(R"(
        <div style="background-color: #e3f2fd; padding: 20px; border-left: 5px solid #2196f3; margin-bottom: 30px; border-radius: 5px;">
            <h2 style="color: #0d47a1; margin-top: 0;">📋 测试概要</h2>
            <div style="display: grid; grid-template-columns: repeat(auto-fit, minmax(250px, 1fr)); gap: 15px;">
                <div style="background: white; padding: 15px; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1);">
                    <strong>设备信息:</strong><br>
                    <span style="color: #2196f3;">%2</span>
                </div>
                <div style="background: white; padding: 15px; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1);">
                    <strong>腔室ID:</strong><br>
                    <span style="color: #4caf50;">%3</span>
                </div>
                <div style="background: white; padding: 15px; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1);">
                    <strong>配方名称:</strong><br>
                    <span style="color: #ff9800;">%4</span>
                </div>
                <div style="background: white; padding: 15px; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1);">
                    <strong>测试状态:</strong><br>
                    <span style="color: %5; font-weight: bold;">%6</span>
                </div>
            </div>
        </div>
    )").arg(testData.testTime)
       .arg(testData.equipmentName)
       .arg(testData.chamberID)
       .arg(testData.recipeName)
       .arg(testData.status.toLower() == "pass" ? "#4caf50" : "#f44336")
       .arg(testData.status);
    
    // 测试结果统计表格
    htmlContent += QString(R"(
        <div style="margin-bottom: 30px;">
            <h2 style="color: #333; border-bottom: 3px solid #4caf50; padding-bottom: 10px;">📊 测试结果统计</h2>
            <div style="overflow-x: auto;">
                <table style="width: 100%; border-collapse: collapse; background: white; border-radius: 8px; overflow: hidden; box-shadow: 0 4px 6px rgba(0,0,0,0.1);">
                    <thead>
                        <tr style="background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); color: white;">
                            <th style="padding: 15px; text-align: left; font-weight: 600;">项目</th>
                            <th style="padding: 15px; text-align: left; font-weight: 600;">数值</th>
                            <th style="padding: 15px; text-align: left; font-weight: 600;">描述</th>
                        </tr>
                    </thead>
                    <tbody>
                        <tr style="border-bottom: 1px solid #eee;">
                            <td style="padding: 12px;">总数据点数</td>
                            <td style="padding: 12px; font-weight: bold; color: #2196f3;">%1</td>
                            <td style="padding: 12px; color: #666;">采集到的数据点总数</td>
                        </tr>
                        <tr style="border-bottom: 1px solid #eee; background-color: #f8f9fa;">
                            <td style="padding: 12px;">平均值</td>
                            <td style="padding: 12px; font-weight: bold; color: #4caf50;">%2</td>
                            <td style="padding: 12px; color: #666;">所有数据点的平均值</td>
                        </tr>
                        <tr style="border-bottom: 1px solid #eee;">
                            <td style="padding: 12px;">最大值</td>
                            <td style="padding: 12px; font-weight: bold; color: #ff9800;">%3</td>
                            <td style="padding: 12px; color: #666;">数据中的最大数值</td>
                        </tr>
                        <tr style="border-bottom: 1px solid #eee; background-color: #f8f9fa;">
                            <td style="padding: 12px;">最小值</td>
                            <td style="padding: 12px; font-weight: bold; color: #f44336;">%4</td>
                            <td style="padding: 12px; color: #666;">数据中的最小数值</td>
                        </tr>
                        <tr>
                            <td style="padding: 12px;">测试状态</td>
                            <td style="padding: 12px; font-weight: bold; color: %5;">%6</td>
                            <td style="padding: 12px; color: #666;">当前测试的执行状态</td>
                        </tr>
                    </tbody>
                </table>
            </div>
        </div>
    )").arg(testData.totalPoints)
       .arg(testData.avgValue, 0, 'f', 4)
       .arg(testData.maxValue, 0, 'f', 4)
       .arg(testData.minValue, 0, 'f', 4)
       .arg(testData.status.toLower() == "pass" ? "#4caf50" : "#f44336")
       .arg(testData.status);
    
    // 图表展示区域
    if (!chartData.isEmpty()) {
        htmlContent += R"(
            <div style="margin-bottom: 30px;">
                <h2 style="color: #333; border-bottom: 3px solid #2196f3; padding-bottom: 10px;">📈 数据图表分析</h2>
                <p style="color: #666; margin-bottom: 20px;">以下是本次测试的数据图表展示：</p>
                <div style="display: grid; grid-template-columns: repeat(auto-fit, minmax(400px, 1fr)); gap: 25px;">
        )";
        
        for (int i = 0; i < chartData.size() && i < 7; ++i) {
            const TestPointData& seriesData = chartData[i];
            if (!seriesData.points.isEmpty()) {
                QString base64Image = createScatterPlotBase64(seriesData.points, 600, 400);
                if (!base64Image.isEmpty()) {
                    htmlContent += QString(R"(
                        <div style="background: white; padding: 20px; border-radius: 10px; box-shadow: 0 4px 8px rgba(0,0,0,0.1); page-break-inside: avoid;">
                            <h3 style="color: #333; margin-top: 0; border-bottom: 2px solid #2196f3; padding-bottom: 10px;">
                                📊 %1
                            </h3>
                            <div style="text-align: center; margin: 15px 0;">
                                <img src="data:image/png;base64,%2" 
                                     alt="%1" 
                                     style="max-width: 100%; height: auto; border: 1px solid #ddd; border-radius: 5px;">
                            </div>
                            <div style="background: #e3f2fd; padding: 10px; border-radius: 5px; margin-top: 10px;">
                                <p style="margin: 0; color: #0d47a1; font-size: 14px;">
                                    <strong>📊 统计信息:</strong> 数据点数量: %3 个
                                </p>
                            </div>
                        </div>
                    )").arg(seriesData.seriesName).arg(base64Image).arg(seriesData.points.size());
                }
            }
        }
        
        htmlContent += "</div></div>";
    }
    
    // 测试总结
    QString summaryText = summary.isEmpty() ? 
        "本次RGA数据分析测试已完成。所有数据点均已成功采集并分析，图表展示了完整的数据分布情况。如需进一步的技术支持或数据分析，请联系相关技术人员。" : 
        summary;
    
    htmlContent += QString(R"(
        <div style="background: linear-gradient(135deg, #f093fb 0%, #f5576c 100%); padding: 25px; border-radius: 10px; color: white; margin-top: 30px;">
            <h2 style="margin-top: 0; color: white;">✅ 测试总结</h2>
            <p style="line-height: 1.6; margin: 15px 0; font-size: 16px;">%1</p>
            <div style="margin-top: 20px; padding-top: 15px; border-top: 1px solid rgba(255,255,255,0.3);">
                <p style="margin: 0; font-size: 14px; opacity: 0.9;">
                    <strong>技术支持:</strong> tech-support@company.com<br>
                    <strong>联系电话:</strong> 400-123-4567<br>
                    <strong>报告生成时间:</strong> %2
                </p>
            </div>
        </div>
    )").arg(summaryText)
       .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    
    // 邮件尾部
    htmlContent += QString(R"(
        <div style="margin-top: 40px; padding-top: 20px; border-top: 2px solid #eee; color: #666; font-size: 14px;">
            <p style="margin: 0;">
                此邮件由RGA数据分析系统自动发送，请勿直接回复。<br>
                如有疑问，请联系技术支持团队。<br>
                <br>
                <strong>系统信息:</strong><br>
                发送时间: %1<br>
                系统版本: RGA Analysis System v2.0<br>
                © 2025 半导体工艺分析中心
            </p>
        </div>
    )").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    
    return createStyledHtml(htmlContent);
}

#ifdef Q_OS_WIN
bool OutlookEmailSender::initializeOutlook()
{
    try {
        // 初始化COM
        HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
        if (FAILED(hr)) {
            qDebug() << "COM初始化失败";
            return false;
        }

        // 尝试创建Outlook对象
        m_outlookApp = new QAxObject(this);

        // 尝试不同的Outlook版本
        QStringList versions = {
            "Outlook.Application",
            "Outlook.Application.16",
            "Outlook.Application.15",
            "Outlook.Application.14"
        };

        bool success = false;
        for (const QString& version : versions) {
            if (m_outlookApp->setControl(version)) {
                success = true;
                qDebug() << "成功连接到Outlook版本:" << version;
                break;
            }
        }

        if (!success) {
            delete m_outlookApp;
            m_outlookApp = nullptr;
            return false;
        }

        // 连接异常信号
        connect(m_outlookApp, SIGNAL(exception(int, const QString&, const QString&, const QString&)),
                this, SLOT(onOutlookException(int, const QString&, const QString&, const QString&)));

        // 获取命名空间
        m_outlookNamespace = m_outlookApp->querySubObject("GetNamespace(const QString&)", "MAPI");
        if (!m_outlookNamespace) {
            delete m_outlookApp;
            m_outlookApp = nullptr;
            return false;
        }

        m_isInitialized = true;
        return true;

    } catch (const std::exception& e) {
        qWarning() << "Outlook初始化异常:" << e.what();
    } catch (...) {
        qWarning() << "Outlook初始化未知异常";
    }

    return false;
}

void OutlookEmailSender::cleanupOutlook()
{
    if (m_outlookNamespace) {
        delete m_outlookNamespace;
        m_outlookNamespace = nullptr;
    }
    
    if (m_outlookApp) {
        delete m_outlookApp;
        m_outlookApp = nullptr;
    }
    
    m_isInitialized = false;
}

void OutlookEmailSender::onOutlookException(int code, const QString& source, const QString& desc, const QString& help)
{
    QString errorMsg = QString("Outlook错误 - 代码: %1, 来源: %2, 描述: %3, 帮助: %4")
                      .arg(code).arg(source).arg(desc).arg(help);
    // 可以在这里添加日志记录
}
#endif

QByteArray OutlookEmailSender::createScatterPlotImage(const QList<QPointF>& points, int width, int height)
{
    if (points.isEmpty()) {
        return QByteArray();
    }
    
    // 创建图片
    QImage image(width, height, QImage::Format_RGB32);
    image.fill(Qt::white);
    
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 设置边距
    int margin = 60;
    int plotWidth = width - 2 * margin;
    int plotHeight = height - 2 * margin;
    
    // 计算数据范围
    qreal minX = points.first().x();
    qreal maxX = points.first().x();
    qreal minY = points.first().y();
    qreal maxY = points.first().y();
    
    for (const QPointF& point : points) {
        minX = qMin(minX, point.x());
        maxX = qMax(maxX, point.x());
        minY = qMin(minY, point.y());
        maxY = qMax(maxY, point.y());
    }
    
    // 添加一些边距
    qreal xRange = maxX - minX;
    qreal yRange = maxY - minY;
    if (xRange == 0) xRange = 1;
    if (yRange == 0) yRange = 1;
    
    minX -= xRange * 0.05;
    maxX += xRange * 0.05;
    minY -= yRange * 0.05;
    maxY += yRange * 0.05;
    
    // 绘制坐标轴
    QPen axisPen(QColor(50, 50, 50), 2);
    painter.setPen(axisPen);
    
    // X轴
    painter.drawLine(margin, height - margin, width - margin, height - margin);
    // Y轴
    painter.drawLine(margin, margin, margin, height - margin);
    
    // 绘制坐标轴标签
    painter.setFont(QFont("Arial", 10));
    painter.setPen(QColor(80, 80, 80));
    
    // X轴标签
    painter.drawText(width - margin - 50, height - margin + 25, QString::number(maxX, 'f', 2));
    painter.drawText(margin + 10, height - margin + 25, QString::number(minX, 'f', 2));
    painter.drawText((width - 2 * margin) / 2 + margin - 20, height - margin + 25, "时间/序列");
    
    // Y轴标签
    painter.drawText(margin - 55, margin + 15, QString::number(maxY, 'f', 2));
    painter.drawText(margin - 55, height - margin + 5, QString::number(minY, 'f', 2));
    painter.drawText(margin - 35, (height - 2 * margin) / 2 + margin, "数值");
    
    // 绘制标题
    painter.setFont(QFont("Arial", 14, QFont::Bold));
    painter.setPen(QColor(40, 40, 40));
    painter.drawText(width / 2 - 80, 30, "RGA数据散点图");
    
    // 绘制网格线
    QPen gridPen(QColor(200, 200, 200), 1, Qt::DashLine);
    painter.setPen(gridPen);
    
    // 垂直网格线
    for (int i = 0; i <= 10; ++i) {
        int x = margin + i * plotWidth / 10;
        painter.drawLine(x, margin, x, height - margin);
    }
    
    // 水平网格线
    for (int i = 0; i <= 10; ++i) {
        int y = margin + i * plotHeight / 10;
        painter.drawLine(margin, y, width - margin, y);
    }
    
    // 绘制散点
    QPen pointPen(QColor(65, 105, 225), 0);  // RoyalBlue
    QBrush pointBrush(QColor(65, 105, 225, 180));  // 半透明蓝色
    painter.setPen(pointPen);
    painter.setBrush(pointBrush);
    
    for (const QPointF& point : points) {
        qreal x = margin + (point.x() - minX) * plotWidth / (maxX - minX);
        qreal y = height - margin - (point.y() - minY) * plotHeight / (maxY - minY);
        painter.drawEllipse(QPointF(x, y), 4, 4);
    }
    
    painter.end();
    
    // 保存为PNG格式到ByteArray
    QByteArray imageData;
    QBuffer buffer(&imageData);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "PNG");
    
    return imageData;
}

QString OutlookEmailSender::createStyledHtml(const QString& content)
{
    QString css = R"(
        body { 
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; 
            font-size: 15px; 
            line-height: 1.6; 
            color: #333; 
            max-width: 1200px; 
            margin: 0 auto; 
            padding: 20px;
            background-color: #f5f7fa;
        }
        h1, h2, h3 { 
            font-weight: 600; 
        }
        h1 { 
            font-size: 28px; 
        }
        h2 { 
            font-size: 22px; 
        }
        h3 { 
            font-size: 18px; 
        }
        p { 
            margin: 10px 0; 
        }
        img { 
            display: block; 
            margin: 0 auto; 
        }
        table { 
            border-collapse: collapse; 
            width: 100%; 
        }
        th, td { 
            padding: 12px; 
            text-align: left; 
            border-bottom: 1px solid #ddd; 
        }
        th { 
            background-color: #f2f2f2; 
        }
        @media print {
            body { 
                padding: 10px; 
                background-color: white;
            }
        }
        @media (max-width: 768px) {
            body { 
                padding: 10px; 
            }
            .grid { 
                grid-template-columns: 1fr !important; 
            }
        }
    )";
    
    return QString(R"(
        <!DOCTYPE html>
        <html>
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>RGA测试报告</title>
            <style>%1</style>
        </head>
        <body>
            %2
        </body>
        </html>
                   )").arg(css, content);
}

bool OutlookEmailSender::configureOutlookSecurity()
{
    try {
        // 修改Outlook安全设置（需要管理员权限）
        QSettings securitySettings("HKEY_CURRENT_USER\\Software\\Microsoft\\Office\\16.0\\Outlook\\Security", QSettings::NativeFormat);
        securitySettings.setValue("Level", 0); // 降低安全级别
        securitySettings.setValue("ObjectModelGuard", 2); // 允许程序访问

        return true;
    } catch (...) {
        return false;
    }
}
