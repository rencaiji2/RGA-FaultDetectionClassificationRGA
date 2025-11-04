#ifndef EMAILSENDER_H
#define EMAILSENDER_H

#include <QObject>
#include <QSslSocket>
#include <QByteArray>
#include <QMap>
#include <QFile>
#include <QFileInfo>
#include <QDateTime>
#include <QTimer>

#include <QObject>
#include <QSslSocket>
#include <QTimer>
#include <QFile>
#include <QUuid>
#include <QFileInfo>

class EmailSender : public QObject
{
    Q_OBJECT

public:
    // 内网邮件服务器配置
    struct EmailConfig {
        QString smtpServer;
        int port;
        QString username;
        QString password;
        bool useSSL;
    };

    // 邮件内容结构
    struct EmailContent {
        QString subject;                    // 邮件主题
        QString htmlBody;                   // HTML正文
        QString textBody;                   // 纯文本正文
        QString to;                         // 收件人
        QStringList cc;                     // 抄送
        QStringList bcc;                    // 密送
        QStringList attachments;            // 附件路径列表
    };

    explicit EmailSender(QObject *parent = nullptr);

    //测试函数
    void testSendWithTable();
    void testSendWithImages();
    void testSendDebug();
    void testSend();

    //基础配置 - 必须配置，不然邮件无法发送
    void initEmailConfig(EmailConfig i_conf);
    void setReciverConf(const QString& to,const QStringList& cc,const QString& subject);

    // 发送HTML邮件的新函数
    bool sendHtmlMail(const QString& to, const QString& subject, const QString& htmlContent);
    bool sendHtmlMail(const QString& to, const QStringList& cc, const QString& subject, const QString& htmlContent);
    bool sendHtmlMail(const QStringList& toList, const QStringList& cc, const QString& subject, const QString& htmlContent);

private:
    //配置函数【服务相关】
    bool connectToServer();
    bool authenticate();
    bool sendEmail();
    bool sendEnhancedEmail(const EmailContent& content);
    void disconnectFromServer();
    QString encodeBase64(const QString& input);
    QString generateBoundary();
    QByteArray prepareEmailData(const EmailContent& content);
    bool waitForResponse(int timeoutMs = 10000);

    // 功能函数
    bool sendHtmlEmail(const EmailContent& content);
    QString createHtmlTable(const QList<QStringList>& tableData, const QStringList& headers = QStringList());
    QString embedImage(const QString& imagePath);
    QString createStyledHtml(const QString& content, const QString& css = QString());

    QSslSocket* m_socket;
    QTimer* m_timeoutTimer;
    QString m_response;
    QString m_boundary;

    //邮件基础配置
    EmailConfig m_config;
    QString m_to;//接收者
    QStringList m_cc;//抄送
    QString m_subject;//主题
};

#endif // EMAILSENDER_H
