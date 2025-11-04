#include "OutlookProcessSender.h"
#include <QProcess>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QTemporaryFile>
#include <QUrl>
#include <QUrlQuery>
#include <QDesktopServices>
#include <QSettings>
#include <QSysInfo>
#include <QRegularExpression>
#include <QApplication>
#include <QMessageBox>

OutlookProcessSender::OutlookProcessSender(QObject *parent)
    : QObject(parent)
    , m_outlookVersion(OUTLOOK_UNKNOWN)
{
    detectOutlookInstallation();
}

void OutlookProcessSender::detectOutlookInstallation()
{
    // 检测不同版本的Outlook
    
    // 1. 检测UWP版本 (Windows Store版)
    QString uwpPath = "C:/Program Files/WindowsApps/";
    QDir uwpDir(uwpPath);
    if (uwpDir.exists()) {
        QStringList uwpOutlookDirs = uwpDir.entryList(QStringList("*Outlook*"), QDir::Dirs);
        if (!uwpOutlookDirs.isEmpty()) {
            m_outlookVersion = OUTLOOK_UWP;
            // UWP应用通常通过协议启动
            return;
        }
    }
    
    // 2. 检测传统桌面版
    QStringList outlookPaths = {
        "C:/Program Files/Microsoft Office/root/Office16/OUTLOOK.EXE",
        "C:/Program Files (x86)/Microsoft Office/root/Office16/OUTLOOK.EXE",
        "C:/Program Files/Microsoft Office/Office16/OUTLOOK.EXE",
        "C:/Program Files (x86)/Microsoft Office/Office16/OUTLOOK.EXE",
        "C:/Program Files/Microsoft Office/Office15/OUTLOOK.EXE",
        "C:/Program Files (x86)/Microsoft Office/Office15/OUTLOOK.EXE"
    };
    
    for (const QString& path : outlookPaths) {
        if (QFile::exists(path)) {
            m_outlookPath = path;
            m_outlookVersion = OUTLOOK_DESKTOP;
            return;
        }
    }
    
    // 3. 通过注册表检测
    QSettings registry("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\OUTLOOK.EXE", QSettings::NativeFormat);
    QString registryPath = registry.value("Path").toString();
    if (!registryPath.isEmpty() && QFile::exists(registryPath + "/OUTLOOK.EXE")) {
        m_outlookPath = registryPath + "/OUTLOOK.EXE";
        m_outlookVersion = OUTLOOK_DESKTOP;
        return;
    }
    
    // 4. 检测是否可以通过协议启动
    // Outlook通常注册了mailto协议
    m_outlookVersion = OUTLOOK_UNKNOWN;
}

bool OutlookProcessSender::isOutlookAvailable() const
{
    if (m_outlookVersion == OUTLOOK_UWP || m_outlookVersion == OUTLOOK_DESKTOP) {
        return true;
    }
    
    // 尝试通过系统检测
    return true; // 系统邮件客户端通常总是可用的
}

bool OutlookProcessSender::sendTestEmail(const QString& recipient, const QString& subject, const QString& body)
{
    EmailContent content;
    content.to = recipient;
    content.subject = subject;
    content.body = body;
    
    return sendEmail(content);
}

bool OutlookProcessSender::sendEmail(const EmailContent& content)
{
    // 使用mailto协议发送邮件
    QString mailtoUrl = createMailtoUrl(content);
    
    if (mailtoUrl.isEmpty()) {
        return false;
    }
    
    // 通过QDesktopServices启动默认邮件客户端
    bool result = QDesktopServices::openUrl(QUrl(mailtoUrl));
    
    if (!result) {
        // 如果QDesktopServices失败，尝试直接启动Outlook进程
        if (m_outlookVersion == OUTLOOK_DESKTOP && !m_outlookPath.isEmpty()) {
            QStringList args;
            args << "/c" << "start" << "mailto:" + content.to;
            if (!content.subject.isEmpty()) {
                args << "?subject=" + QUrl::toPercentEncoding(content.subject);
            }
            
            QProcess process;
            return process.startDetached("cmd", args);
        }
    }
    
    return result;
}

bool OutlookProcessSender::createDraftEmail(const EmailContent& content)
{
    // 创建草稿邮件（不自动发送）
    QString mailtoUrl = createMailtoUrl(content);
    
    if (mailtoUrl.isEmpty()) {
        return false;
    }
    
    // 添加参数表示创建草稿
    if (mailtoUrl.contains("?")) {
        mailtoUrl += "&";
    } else {
        mailtoUrl += "?";
    }
    mailtoUrl += "action=new";
    
    return QDesktopServices::openUrl(QUrl(mailtoUrl));
}

QString OutlookProcessSender::createMailtoUrl(const EmailContent& content) const
{
    if (content.to.isEmpty()) {
        return QString();
    }
    
    QUrl url("mailto:" + content.to);
    QUrlQuery query;
    
    if (!content.subject.isEmpty()) {
        query.addQueryItem("subject", content.subject);
    }
    
    if (!content.body.isEmpty()) {
        query.addQueryItem("body", content.body);
    }
    
    if (!content.cc.isEmpty()) {
        query.addQueryItem("cc", content.cc.join(","));
    }
    
    if (!content.bcc.isEmpty()) {
        query.addQueryItem("bcc", content.bcc.join(","));
    }
    
    // 注意：附件在mailto协议中支持有限，通常不建议使用
    
    if (!query.isEmpty()) {
        url.setQuery(query);
    }
    
    return url.toString();
}

bool OutlookProcessSender::validateEmail(const QString& email) const
{
    // 简单的邮箱格式验证
    static QRegularExpression emailRegex("^[\\w\\.\\-]+@([\\w\\-]+\\.)+[\\w\\-]{2,}$");
    return emailRegex.match(email).hasMatch();
}

bool OutlookProcessSender::launchOutlookWithArgs(const QStringList& arguments) const
{
    if (m_outlookVersion == OUTLOOK_DESKTOP && !m_outlookPath.isEmpty()) {
        QProcess process;
        return process.startDetached(m_outlookPath, arguments);
    } else {
        // 对于UWP版本，使用协议启动
        QString command = "outlook:";
        if (!arguments.isEmpty()) {
            command += arguments.join(" ");
        }
        
        return QProcess::startDetached("cmd", QStringList() << "/c" << "start" << command);
    }
}