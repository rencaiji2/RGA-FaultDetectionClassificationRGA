#include "UniversalEmailSender.h"
#include <QDesktopServices>
#include <QUrl>
#include <QUrlQuery>
#include <QMessageBox>
#include <QTcpSocket>
#include <QTimer>
#include <QEventLoop>
#include <QSslSocket>
#include <QFile>
#include <QFileInfo>
#include <QTextCodec>
#include <QUuid>
#include <QDateTime>
#include <QSysInfo>

#ifdef Q_OS_WIN
#include <windows.h>
// MAPI相关头文件
#endif

UniversalEmailSender::UniversalEmailSender(QObject *parent)
    : QObject(parent)
#ifdef Q_OS_WIN
    , m_mapiAvailable(false)
#endif
{
    // 初始化默认配置
    m_config.smtpServer = "127.0.0.1";
    m_config.port = 25;
    m_config.useSSL = false;
    m_config.useTLS = false;
    m_config.senderName = "RGA系统";
    m_config.senderEmail = "system@company.com";
    
    // 检测可用的邮件发送方法
    m_availableMethods.clear();
    
    // SMTP方法总是可用（只要有网络）
    m_availableMethods.append(SMTP_METHOD);
    
    // 系统邮件客户端方法
    m_availableMethods.append(SYSTEM_METHOD);
    
#ifdef Q_OS_WIN
    // 初始化MAPI
    initializeMAPI();
    if (m_mapiAvailable) {
        m_availableMethods.append(MAPI_METHOD);
    }
#endif
    
    // Web方法（需要配置）
    // 这里可以根据配置决定是否添加
}

void UniversalEmailSender::setConfig(const EmailConfig& config)
{
    m_config = config;
    
    // 如果配置了有效的SMTP服务器，添加Web方法
    if (!m_config.smtpServer.isEmpty() && m_config.smtpServer != "127.0.0.1") {
        if (!m_availableMethods.contains(WEB_METHOD)) {
            m_availableMethods.append(WEB_METHOD);
        }
    }
}

QList<UniversalEmailSender::EmailMethod> UniversalEmailSender::getAvailableMethods() const
{
    return m_availableMethods;
}

bool UniversalEmailSender::sendTestEmail(const QString& recipient, const QString& subject, const QString& body)
{
    EmailContent content;
    content.to = recipient;
    content.subject = subject;
    content.textBody = body;
    
    return sendEmail(content);
}

bool UniversalEmailSender::sendEmail(const EmailContent& content)
{
    // 按优先级尝试不同的发送方法
    for (EmailMethod method : m_availableMethods) {
        bool success = false;
        
        switch (method) {
        case SMTP_METHOD:
            success = sendViaSMTP(content);
            break;
        case SYSTEM_METHOD:
            success = sendViaSystemMail(content);
            break;
#ifdef Q_OS_WIN
        case MAPI_METHOD:
            if (m_mapiAvailable) {
                success = sendViaMAPI(content);
            }
            break;
#endif
        case WEB_METHOD:
            success = sendViaWebAPI(content);
            break;
        }
        
        if (success) {
            return true;
        }
    }
    
    return false;
}

bool UniversalEmailSender::isSMTPAvailable() const
{
    return m_availableMethods.contains(SMTP_METHOD);
}

bool UniversalEmailSender::isSystemMailAvailable() const
{
    return m_availableMethods.contains(SYSTEM_METHOD);
}

bool UniversalEmailSender::isMAPIAvailable() const
{
#ifdef Q_OS_WIN
    return m_mapiAvailable;
#else
    return false;
#endif
}

bool UniversalEmailSender::isWebMailAvailable() const
{
    return m_availableMethods.contains(WEB_METHOD);
}

bool UniversalEmailSender::sendViaSMTP(const EmailContent& content)
{
    // 这里可以复用你已有的EmailSender类的SMTP功能
    // 或者实现一个简单的SMTP客户端
    
    // 简单检查SMTP服务器是否可达
    QTcpSocket socket;
    socket.connectToHost(m_config.smtpServer, m_config.port);
    
    if (!socket.waitForConnected(5000)) {
        return false;
    }
    
    socket.disconnectFromHost();
    
    // 如果只是发送测试邮件，使用系统邮件客户端可能更简单
    if (content.textBody == "helloworld" && content.subject == "测试邮件") {
        return sendViaSystemMail(content);
    }
    
    // 对于复杂邮件，建议使用你已有的EmailSender类
    return sendViaSystemMail(content);
}

bool UniversalEmailSender::sendViaSystemMail(const EmailContent& content)
{
    // 使用系统默认邮件客户端发送
    QString mailtoUrl = createMailtoUrl(content);
    
    if (mailtoUrl.isEmpty()) {
        return false;
    }
    
    return QDesktopServices::openUrl(QUrl(mailtoUrl));
}

QString UniversalEmailSender::createMailtoUrl(const EmailContent& content) const
{
    QUrl url("mailto:" + content.to);
    QUrlQuery query;
    
    if (!content.subject.isEmpty()) {
        query.addQueryItem("subject", content.subject);
    }
    
    if (!content.textBody.isEmpty()) {
        query.addQueryItem("body", content.textBody);
    }
    
    if (!content.cc.isEmpty()) {
        query.addQueryItem("cc", content.cc.join(","));
    }
    
    if (!content.bcc.isEmpty()) {
        query.addQueryItem("bcc", content.bcc.join(","));
    }
    
    if (!query.isEmpty()) {
        url.setQuery(query);
    }
    
    return url.toString();
}

bool UniversalEmailSender::sendViaWebAPI(const EmailContent& content)
{
    // 对于Web邮件API，需要具体的API端点和认证信息
    // 这里可以实现Office 365 Graph API等
    
    // 目前回退到系统邮件客户端
    return sendViaSystemMail(content);
}

#ifdef Q_OS_WIN
void UniversalEmailSender::initializeMAPI()
{
    // 检查MAPI是否可用
    HMODULE hMapi = LoadLibrary(TEXT("mapi32.dll"));
    if (hMapi) {
        m_mapiAvailable = true;
        FreeLibrary(hMapi);
    } else {
        m_mapiAvailable = false;
    }
}

bool UniversalEmailSender::sendViaMAPI(const EmailContent& content)
{
    // 对于UWP版本的Outlook，MAPI可能无法正常工作
    // 回退到系统邮件客户端
    return sendViaSystemMail(content);
}
#endif

bool UniversalEmailSender::validateEmail(const QString& email) const
{
    // 简单的邮箱格式验证
    return email.contains("@") && email.contains(".");
}