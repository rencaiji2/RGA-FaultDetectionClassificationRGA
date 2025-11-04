#ifndef OUTLOOKEMAILSENDER_H
#define OUTLOOKEMAILSENDER_H

#include <QObject>
#include <QList>
#include <QPointF>
#include <QDateTime>
#include <QVariant>
#include <QDebug>
#include <QTimer>
#include <QSettings>

#ifdef Q_OS_WIN
#include <QAxObject>
#endif

class OutlookEmailSender : public QObject
{
    Q_OBJECT

public:
    explicit OutlookEmailSender(QObject *parent = nullptr);
    ~OutlookEmailSender();
    
    // 测试数据结构
    struct TestPointData {
        QString seriesName;
        QList<QPointF> points;
    };
    
    // 测试结果数据
    struct TestResultData {
        QString testName;
        QString equipmentName;
        QString chamberID;
        QString recipeName;
        QString testTime;
        int totalPoints;
        double avgValue;
        double maxValue;
        double minValue;
        QString status;
    };
    
    // Outlook配置
    struct OutlookConfig {
        bool useExistingOutlook;    // 使用已登录的Outlook
        QString profileName;        // 配置文件名（如果需要指定）
    };
    
    // 设置配置
    void setConfig(const OutlookConfig& config);
    
    // 检查Outlook是否可用
    bool isOutlookAvailable() const;
    
    // 发送测试报告邮件
    bool sendTestReport(const TestResultData& testData, 
                       const QList<TestPointData>& chartData,
                       const QString& summary = QString(),
                       const QStringList& recipients = QStringList(),
                       const QStringList& ccRecipients = QStringList());

    bool sendTestEmailWithRetry(const QString& recipient, const QString& subject, const QString& body, int retries = 3);
    // 简单的测试邮件发送功能 - 新增
    bool sendTestEmail(const QString& recipient, const QString& subject = "测试邮件", const QString& body = "helloworld");

    // 创建散点图Base64数据（用于嵌入HTML）
    QString createScatterPlotBase64(const QList<QPointF>& points, int width = 800, int height = 600);
    
    // 生成完整的HTML报告
    QString generateHtmlReport(const TestResultData& testData, 
                              const QList<TestPointData>& chartData,
                              const QString& summary = QString());

    bool configureOutlookSecurity();

private slots:
    void onOutlookException(int code, const QString& source, const QString& desc, const QString& help);
    
private:
    // 内部函数
    bool initializeOutlook();
    void cleanupOutlook();
    QByteArray createScatterPlotImage(const QList<QPointF>& points, int width, int height);
    QString createStyledHtml(const QString& content);


    
#ifdef Q_OS_WIN
    QAxObject* m_outlookApp;
    QAxObject* m_outlookNamespace;
    bool m_isInitialized;
#endif
    
    OutlookConfig m_config;
};

#endif // OUTLOOKEMAILSENDER_H
