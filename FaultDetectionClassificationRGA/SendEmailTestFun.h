#ifndef SENDEMAILTESTFUN_H
#define SENDEMAILTESTFUN_H

#include <QObject>
#include <QSslSocket>
#include <QTimer>
#include <QFile>
#include <QUuid>
#include <QFileInfo>

class SendEmailTestFun : public QObject
{
    Q_OBJECT

public:
    explicit SendEmailTestFun(QObject *parent = nullptr);

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

    void testSendWithTable();
    void testSendWithImages();

    void testSendDebug();
    void testSend();

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

    // 内网邮件服务器配置
    struct EmailConfig {
        QString smtpServer;
        int port;
        QString username;
        QString password;
        bool useSSL;
    } m_config;
};


#endif // SENDEMAILTESTFUN_H
