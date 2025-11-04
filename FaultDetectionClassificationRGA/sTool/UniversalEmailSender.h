#ifndef UNIVERSALEMAILSENDER_H
#define UNIVERSALEMAILSENDER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QVariant>

class UniversalEmailSender : public QObject
{
    Q_OBJECT

public:
    explicit UniversalEmailSender(QObject *parent = nullptr);
    
    enum EmailMethod {
        SMTP_METHOD,           // SMTP方式发送
        SYSTEM_METHOD,         // 系统默认邮件客户端
        MAPI_METHOD,           // MAPI方式(传统Outlook)
        WEB_METHOD             // Web方式(Office 365等)
    };
    
    struct EmailConfig {
        QString smtpServer;
        int port;
        QString username;
        QString password;
        bool useSSL;
        bool useTLS;
        QString senderName;
        QString senderEmail;
    };
    
    struct EmailContent {
        QString to;
        QStringList cc;
        QStringList bcc;
        QString subject;
        QString textBody;
        QString htmlBody;
        QStringList attachments;
    };
    
    // 设置配置
    void setConfig(const EmailConfig& config);
    
    // 检测可用的邮件发送方法
    QList<EmailMethod> getAvailableMethods() const;
    
    // 发送测试邮件
    bool sendTestEmail(const QString& recipient, const QString& subject = "测试邮件", const QString& body = "helloworld");
    
    // 发送完整邮件
    bool sendEmail(const EmailContent& content);
    
    // 检查各种邮件方法的可用性
    bool isSMTPAvailable() const;
    bool isSystemMailAvailable() const;
    bool isMAPIAvailable() const;
    bool isWebMailAvailable() const;

private:
    // 内部发送方法
    bool sendViaSMTP(const EmailContent& content);
    bool sendViaSystemMail(const EmailContent& content);
    bool sendViaMAPI(const EmailContent& content);
    bool sendViaWebAPI(const EmailContent& content);
    
    // 辅助方法
    QString createMailtoUrl(const EmailContent& content) const;
    bool validateEmail(const QString& email) const;
    
    EmailConfig m_config;
    QList<EmailMethod> m_availableMethods;
    
#ifdef Q_OS_WIN
    bool m_mapiAvailable;
    void initializeMAPI();
#endif
};

#endif // UNIVERSALEMAILSENDER_H