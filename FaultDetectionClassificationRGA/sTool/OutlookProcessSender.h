#ifndef OUTLOOKPROCESSSENDER_H
#define OUTLOOKPROCESSSENDER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QProcess>
#include <QTemporaryFile>
#include <QFile>

class OutlookProcessSender : public QObject
{
    Q_OBJECT

public:
    explicit OutlookProcessSender(QObject *parent = nullptr);
    
    struct EmailContent {
        QString to;
        QStringList cc;
        QStringList bcc;
        QString subject;
        QString body;
        QStringList attachments;
    };
    
    // 发送测试邮件
    bool sendTestEmail(const QString& recipient, const QString& subject = "测试邮件", const QString& body = "helloworld");
    
    // 发送完整邮件
    bool sendEmail(const EmailContent& content);
    
    // 检查Outlook是否可用
    bool isOutlookAvailable() const;
    
    // 创建邮件但不发送（草稿）
    bool createDraftEmail(const EmailContent& content);
    
private:
    // 内部方法
    QString createMailtoUrl(const EmailContent& content) const;
    bool validateEmail(const QString& email) const;
    QString createTempHtmlFile(const QString& htmlContent) const;
    bool launchOutlookWithArgs(const QStringList& arguments) const;
    
    // Outlook可执行文件路径
    QString m_outlookPath;
    
    // 检测Outlook安装位置
    void detectOutlookInstallation();
    
    // 不同版本Outlook的处理
    enum OutlookVersion {
        OUTLOOK_DESKTOP,    // 传统桌面版
        OUTLOOK_UWP,        // Windows Store版(UWP)
        OUTLOOK_WEB,        // Web版
        OUTLOOK_UNKNOWN
    };
    
    OutlookVersion m_outlookVersion;
};

#endif // OUTLOOKPROCESSSENDER_H