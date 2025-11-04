#include "EmailSender.h"
#include <QTextCodec>
#include <QDateTime>
#include <QFileInfo>
#include <QFile>
#include <QUrl>
#include <QUuid>
#include <QRegularExpression>

#include <QDebug>
#include <QHostInfo>
#include <QDateTime>
#include <QTextCodec>

EmailSender::EmailSender(QObject* parent)
    : QObject(parent)
    , m_socket(new QSslSocket(this))
    , m_timeoutTimer(new QTimer(this))
    , m_boundary(generateBoundary())
{
    // 配置内网邮件服务器
    m_config.smtpServer = "smtp.163.com";
    m_config.port = 25;
    m_config.username = "shenxiaoxiang2000@163.com";
    m_config.password = "TTWjLNN9LySxZvZt";
    m_config.useSSL = false;

    m_timeoutTimer->setSingleShot(true);
    connect(m_timeoutTimer, &QTimer::timeout, this, [this]() {
        qDebug() << "连接超时";
        m_socket->abort();
    });

    connect(m_socket, &QSslSocket::connected, this, []() {
        qDebug() << "SMTP服务器连接成功";
    });

    connect(m_socket, &QSslSocket::disconnected, this, []() {
        qDebug() << "SMTP服务器断开连接";
    });

    connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QSslSocket::error),
        this, [this](QAbstractSocket::SocketError error) {
            qDebug() << "Socket错误:" << error << m_socket->errorString();
    });
}

void EmailSender::testSendWithTable()
{
    EmailContent content;
    content.subject = "测试结果报告";
    content.to = "170176354@qq.com";
    content.cc = QStringList() << "shenxiaoxiang2000@163.com";

    // 创建测试数据表格
    QList<QStringList> testData;
    testData << (QStringList() << "HV测试1" << "通过" << "200V" << "50ms");
    testData << (QStringList() << "HV测试2" << "失败" << "300V" << "65ms");
    testData << (QStringList() << "HV测试3" << "通过" << "250V" << "55ms");

    QStringList headers = {"测试项目", "状态", "电压", "耗时"};

    QString tableHtml = createHtmlTable(testData, headers);

    QString htmlBody = QString(R"(
        <h1>高压测试报告</h1>
        <p><strong>测试时间:</strong> %1</p>
        <h2>测试结果详情</h2>
        %2
        <p>请查看附件中的详细测试数据。</p>
    )").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"), tableHtml);

    content.htmlBody = createStyledHtml(htmlBody);
    content.attachments << "hv_test_data.csv" << "hv_test_log.txt";

    if (connectToServer() && authenticate() && sendEnhancedEmail(content)) {
        qDebug() << "带表格的邮件发送成功";
    }
    disconnectFromServer();
}

void EmailSender::testSendWithImages()
{
    EmailContent content;
    content.subject = "测试图表报告";
    content.to = "test2@ydmail.com";
    content.cc = QStringList() << "test2@ydmail.com";

    QString htmlBody = QString(R"(
        <h1>测试图表报告</h1>
        <p>以下是测试结果的图表：</p>
        <div style="text-align: center;">
            <img src="cid:chart1" alt="测试图表1" style="max-width: 500px; height: auto; margin: 10px;">
            <img src="cid:chart2" alt="测试图表2" style="max-width: 500px; height: auto; margin: 10px;">
        </div>
        <p>图表说明：...</p>
    )");

    content.htmlBody = createStyledHtml(htmlBody);
    // 注意：真正的图片嵌入需要在MIME部分中处理，这里简化处理
    content.attachments << "chart1.png" << "chart2.png";

    if (connectToServer() && authenticate() && sendEnhancedEmail(content)) {
        qDebug() << "带图片的邮件发送成功";
    }
    disconnectFromServer();
}

void EmailSender::testSendDebug()
{
    qDebug() << "=== 邮件发送测试开始 ===";
    qDebug() << "服务器地址:" << m_config.smtpServer;
    qDebug() << "端口:" << m_config.port;
    qDebug() << "用户名:" << m_config.username;
    qDebug() << "使用SSL:" << m_config.useSSL;

    // 检查网络连接
    QHostInfo hostInfo = QHostInfo::fromName(m_config.smtpServer);
    if (hostInfo.error() != QHostInfo::NoError) {
        qDebug() << "DNS解析失败:" << hostInfo.errorString();
        return;
    }
    qDebug() << "DNS解析成功，IP地址:" << hostInfo.addresses().first().toString();

    testSend();
}

void EmailSender::testSend()
{
    qDebug() << "开始测试邮件发送...";

    // 创建丰富的HTML邮件内容
    EmailContent content;
    content.subject = QString("测试报告 - %1").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm"));
    content.to = "170176354@qq.com";
    content.cc << "170176354@qq.com";

    // 创建测试结果表格数据
    QList<QStringList> tableData;
    tableData << (QStringList() << "测试项1" << "通过" << "100ms");
    tableData << (QStringList() << "测试项2" << "失败" << "150ms");
    tableData << (QStringList() << "测试项3" << "通过" << "80ms");
    tableData << (QStringList() << "测试项4" << "通过" << "120ms");

    QStringList headers;
    headers << "测试项目" << "状态" << "耗时";

    QString tableHtml = createHtmlTable(tableData, headers);

    // 创建HTML内容
    QString htmlContent = QString(R"(
        <h1 style="color: #2c3e50; border-bottom: 2px solid #3498db;">自动化测试报告</h1>
        <p><strong>测试时间:</strong> %1</p>
        <p><strong>测试结果:</strong> 4个测试项，3个通过，1个失败</p>

        <h2 style="color: #34495e;">详细结果</h2>
        %2

        <h2 style="color: #34495e;">统计信息</h2>
        <ul style="line-height: 1.6;">
            <li>总测试数: 4</li>
            <li>通过率: 75%</li>
            <li>平均耗时: 112.5ms</li>
            <li>执行人: 测试系统</li>
        </ul>

        <p style="color: #7f8c8d; font-size: 12px;">如有疑问，请联系测试团队。</p>
    )").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"), tableHtml);

    // 添加样式
    QString css = R"(
        body { font-family: Arial, sans-serif; font-size: 14px; }
        table { border-collapse: collapse; width: 100%; margin: 20px 0; }
        th { background-color: #3498db; color: white; font-weight: bold; padding: 12px; text-align: left; }
        td { padding: 12px; text-align: left; border: 1px solid #bdc3c7; }
        tr:nth-child(even) { background-color: #ecf0f1; }
        ul { line-height: 1.6; }
        h1, h2 { margin-top: 20px; margin-bottom: 10px; }
    )";

    content.htmlBody = createStyledHtml(htmlContent, css);

    // 添加纯文本版本（可选）
    content.textBody = QString(R"(
        自动化测试报告
        测试时间: %1
        测试结果: 4个测试项，3个通过，1个失败

        详细结果:
        测试项目    状态    耗时
        测试项1     通过    100ms
        测试项2     失败    150ms
        测试项3     通过    80ms
        测试项4     通过    120ms

        统计信息:
        - 总测试数: 4
        - 通过率: 75%
        - 平均耗时: 112.5ms
        - 执行人: 测试系统

        如有疑问，请联系测试团队。
            )").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

    // 添加附件（如果需要）
    // content.attachments << "test_results.csv" << "test_log.txt";

    if (connectToServer()) {
        qDebug() << "连接服务器成功";

        if (authenticate()) {
            qDebug() << "认证成功";

            if (sendEnhancedEmail(content)) {
                qDebug() << "HTML邮件发送成功";
            } else {
                qDebug() << "HTML邮件发送失败";
            }
        } else {
            qDebug() << "认证失败";
        }

        disconnectFromServer();
    } else {
        qDebug() << "连接服务器失败";
    }
}

/*!
 * \brief EmailSender::initEmailConfig
 * \param i_conf 邮件的基础配置，包含服务器配置与发送人配置
 */
void EmailSender::initEmailConfig(EmailSender::EmailConfig i_conf)
{
    m_config = i_conf;
}

/*!
 * \brief EmailSender::setReciverConf 设置接收者信息
 * \param to 接收者
 * \param cc 抄送
 * \param subject 主题
 */
void EmailSender::setReciverConf(const QString &to, const QStringList &cc, const QString &subject)
{
    m_to = to;
    m_cc = cc;
    m_subject = subject;
}

// 在 EmailSender.cpp 中添加以下实现

bool EmailSender::sendHtmlMail(const QString& to, const QString& subject, const QString& htmlContent)
{
    bool ok = false;
    EmailContent content;
    content.to = to;
    content.subject = subject;
    content.htmlBody = htmlContent;

    if (connectToServer()) {
        qDebug() << "连接服务器成功";

        if (authenticate()) {
            qDebug() << "认证成功";

            if (sendEnhancedEmail(content)) {
                ok = true;
                qDebug() << "HTML邮件发送成功";
            } else {
                qDebug() << "HTML邮件发送失败";
            }
        } else {
            qDebug() << "认证失败";
        }

        disconnectFromServer();
    } else {
        qDebug() << "连接服务器失败";
    }

    return ok;
}

bool EmailSender::sendHtmlMail(const QString& to, const QStringList& cc, const QString& subject, const QString& htmlContent)
{
    bool ok = false;
    EmailContent content;
    content.to = to;
    content.cc = cc;
    content.subject = subject;
    content.htmlBody = htmlContent;

    if (connectToServer()) {
        qDebug() << "连接服务器成功";

        if (authenticate()) {
            qDebug() << "认证成功";

            if (sendEnhancedEmail(content)) {
                ok = true;
                qDebug() << "HTML邮件发送成功";
            } else {
                qDebug() << "HTML邮件发送失败";
            }
        } else {
            qDebug() << "认证失败";
        }

        disconnectFromServer();
    } else {
        qDebug() << "连接服务器失败";
    }

    return ok;
}

bool EmailSender::sendHtmlMail(const QStringList& toList, const QStringList& cc, const QString& subject, const QString& htmlContent)
{
    bool ok = false;
    EmailContent content;
    content.to = toList.join(",");
    content.cc = cc;
    content.subject = subject;
    content.htmlBody = htmlContent;

    if (connectToServer()) {
        qDebug() << "连接服务器成功";

        if (authenticate()) {
            qDebug() << "认证成功";

            if (sendEnhancedEmail(content)) {
                ok = true;
                qDebug() << "HTML邮件发送成功";
            } else {
                qDebug() << "HTML邮件发送失败";
            }
        } else {
            qDebug() << "认证失败";
        }

        disconnectFromServer();
    } else {
        qDebug() << "连接服务器失败";
    }
    return ok;
}

// 发送增强版邮件（支持HTML、附件等）
bool EmailSender::sendEnhancedEmail(const EmailContent& content)
{
    // 发送MAIL FROM命令
    QString mailFrom = QString("MAIL FROM:<%1>\r\n").arg(m_config.username);
    m_socket->write(mailFrom.toUtf8());
    if (!waitForResponse()) {
        qDebug() << "MAIL FROM命令失败";
        return false;
    }
    qDebug() << "MAIL FROM响应:" << m_response.trimmed();

    // 发送RCPT TO命令（收件人）
    QString rcptTo = QString("RCPT TO:<%1>\r\n").arg(content.to);
    m_socket->write(rcptTo.toUtf8());
    if (!waitForResponse()) {
        qDebug() << "RCPT TO命令失败";
        return false;
    }
    qDebug() << "RCPT TO响应:" << m_response.trimmed();

    // 发送抄送
    for (const QString& ccAddr : content.cc) {
        QString ccCmd = QString("RCPT TO:<%1>\r\n").arg(ccAddr);
        m_socket->write(ccCmd.toUtf8());
        if (!waitForResponse()) {
            qDebug() << "CC命令失败";
            return false;
        }
        qDebug() << "CC响应:" << m_response.trimmed();
    }

    // 发送DATA命令
    m_socket->write("DATA\r\n");
    if (!waitForResponse()) {
        qDebug() << "DATA命令失败";
        return false;
    }
    qDebug() << "DATA响应:" << m_response.trimmed();

    // 发送邮件内容
    QByteArray emailData = prepareEmailData(content);
    m_socket->write(emailData);
    if (!waitForResponse(60000)) {  // 邮件发送可能需要更长时间
        qDebug() << "邮件内容发送失败";
        return false;
    }
    qDebug() << "邮件发送响应:" << m_response.trimmed();

    return m_response.startsWith("250");  // 250表示发送成功
}

// 准备邮件数据（支持多部分MIME）
QByteArray EmailSender::prepareEmailData(const EmailContent& content)
{
    QString emailContent;

    // 邮件头部
    emailContent += QString("From: %1\r\n").arg(m_config.username);
    emailContent += QString("To: %1\r\n").arg(content.to);

    if (!content.cc.isEmpty()) {
        emailContent += QString("Cc: %1\r\n").arg(content.cc.join(", "));
    }

    emailContent += QString("Subject: %1\r\n").arg(content.subject);
    emailContent += "Date: " + QDateTime::currentDateTime().toString(Qt::RFC2822Date) + "\r\n";
    emailContent += "MIME-Version: 1.0\r\n";

    // 处理多部分邮件（HTML + 附件）
    if (!content.attachments.isEmpty() || !content.htmlBody.isEmpty()) {
        emailContent += QString("Content-Type: multipart/mixed; boundary=\"%1\"\r\n\r\n").arg(m_boundary);

        // HTML正文部分
        if (!content.htmlBody.isEmpty()) {
            emailContent += QString("--%1\r\n").arg(m_boundary);
            emailContent += "Content-Type: text/html; charset=UTF-8\r\n";
            emailContent += "Content-Transfer-Encoding: 8bit\r\n\r\n";
            emailContent += content.htmlBody + "\r\n\r\n";
        }

        // 纯文本部分
        if (!content.textBody.isEmpty()) {
            emailContent += QString("--%1\r\n").arg(m_boundary);
            emailContent += "Content-Type: text/plain; charset=UTF-8\r\n";
            emailContent += "Content-Transfer-Encoding: 8bit\r\n\r\n";
            emailContent += content.textBody + "\r\n\r\n";
        }

        // 附件部分
        for (const QString& attachmentPath : content.attachments) {
            QFile file(attachmentPath);
            if (file.open(QIODevice::ReadOnly)) {
                QFileInfo fileInfo(attachmentPath);
                QByteArray fileData = file.readAll();
                file.close();

                emailContent += QString("--%1\r\n").arg(m_boundary);
                emailContent += QString("Content-Type: application/octet-stream; name=\"%1\"\r\n").arg(fileInfo.fileName());
                emailContent += "Content-Transfer-Encoding: base64\r\n";
                emailContent += QString("Content-Disposition: attachment; filename=\"%1\"\r\n\r\n").arg(fileInfo.fileName());

                // 分块编码以避免一行过长
                QByteArray base64Data = fileData.toBase64();
                for (int i = 0; i < base64Data.length(); i += 76) {
                    emailContent += base64Data.mid(i, 76) + "\r\n";
                }
                emailContent += "\r\n";
            }
        }

        emailContent += QString("--%1--\r\n").arg(m_boundary);
    } else {
        // 简单HTML邮件
        if (!content.htmlBody.isEmpty()) {
            emailContent += "Content-Type: text/html; charset=UTF-8\r\n";
            emailContent += "Content-Transfer-Encoding: 8bit\r\n\r\n";
            emailContent += content.htmlBody;
        } else if (!content.textBody.isEmpty()) {
            emailContent += "Content-Type: text/plain; charset=UTF-8\r\n";
            emailContent += "Content-Transfer-Encoding: 8bit\r\n\r\n";
            emailContent += content.textBody;
        }
    }

    emailContent += "\r\n.\r\n";
    return emailContent.toUtf8();
}

// 创建HTML表格
QString EmailSender::createHtmlTable(const QList<QStringList>& tableData, const QStringList& headers)
{
    QString html = "<table border='1' cellpadding='5' cellspacing='0' style='border-collapse: collapse; width: 100%;'>";

    // 添加表头
    if (!headers.isEmpty()) {
        html += "<thead><tr>";
        for (const QString& header : headers) {
            html += QString("<th style='background-color: #3498db; color: white; font-weight: bold;'>%1</th>").arg(header);
        }
        html += "</tr></thead>";
    }

    // 添加表格数据
    html += "<tbody>";
    for (int i = 0; i < tableData.size(); ++i) {
        const QStringList& row = tableData[i];
        QString rowStyle = (i % 2 == 0) ? "background-color: #f8f9fa;" : "background-color: #ffffff;";
        html += QString("<tr style='%1'>").arg(rowStyle);
        for (const QString& cell : row) {
            html += QString("<td style='padding: 8px; border: 1px solid #dee2e6;'>%1</td>").arg(cell);
        }
        html += "</tr>";
    }
    html += "</tbody></table>";

    return html;
}

// 嵌入图片（返回HTML img标签）
QString EmailSender::embedImage(const QString& imagePath)
{
    QString imageCid = QUuid::createUuid().toString().remove("{").remove("}");
    return QString("<img src='cid:%1' alt='Embedded Image' style='max-width: 100%; height: auto;' />").arg(imageCid);
}

// 创建带样式的HTML
QString EmailSender::createStyledHtml(const QString& content, const QString& css)
{
    QString defaultCss = R"(
        body { font-family: Arial, sans-serif; font-size: 14px; line-height: 1.6; color: #333; }
        h1 { color: #2c3e50; border-bottom: 2px solid #3498db; padding-bottom: 10px; }
        h2 { color: #34495e; margin-top: 20px; }
        table { border-collapse: collapse; width: 100%; margin: 20px 0; }
        th { background-color: #3498db; color: white; font-weight: bold; padding: 12px; text-align: left; }
        td { padding: 12px; text-align: left; border: 1px solid #bdc3c7; }
        tr:nth-child(even) { background-color: #ecf0f1; }
        ul, ol { margin: 10px 0; padding-left: 20px; }
        li { margin: 5px 0; }
        p { margin: 10px 0; }
    )";

    QString finalCss = css.isEmpty() ? defaultCss : css;

    return QString(R"(
        <!DOCTYPE html>
        <html>
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <style>%1</style>
        </head>
        <body>
            %2
        </body>
        </html>
    )").arg(finalCss, content);
}

// 生成MIME边界
QString EmailSender::generateBoundary()
{
    return "boundary_" + QUuid::createUuid().toString().remove("{").remove("}");
}

// 以下函数保持不变...
bool EmailSender::connectToServer()
{
    qDebug() << "正在连接到服务器:" << m_config.smtpServer << ":" << m_config.port;

    m_socket->connectToHost(m_config.smtpServer, m_config.port);

    if (!m_socket->waitForConnected(10000)) {
        qDebug() << "连接超时或失败:" << m_socket->errorString();
        return false;
    }

    // 等待服务器欢迎信息
    if (!waitForResponse()) {
        qDebug() << "等待服务器响应超时";
        return false;
    }

    qDebug() << "服务器响应:" << m_response.trimmed();
    return true;
}

bool EmailSender::authenticate()
{
    // 发送EHLO命令
    m_socket->write("EHLO localhost\r\n");
    if (!waitForResponse()) {
        qDebug() << "EHLO命令失败";
        return false;
    }
    qDebug() << "EHLO响应:" << m_response.trimmed();

    // 发送AUTH LOGIN命令（如果服务器支持）
    m_socket->write("AUTH LOGIN\r\n");
    if (!waitForResponse()) {
        qDebug() << "AUTH LOGIN命令失败，尝试直接发送邮件";
        return true;  // 有些内网服务器不需要认证
    }
    qDebug() << "AUTH LOGIN响应:" << m_response.trimmed();

    // 发送用户名（Base64编码）
    QString encodedUsername = encodeBase64(m_config.username);
    m_socket->write(encodedUsername.toUtf8() + "\r\n");
    if (!waitForResponse()) {
        qDebug() << "用户名发送失败";
        return false;
    }
    qDebug() << "用户名响应:" << m_response.trimmed();

    // 发送密码（Base64编码）
    QString encodedPassword = encodeBase64(m_config.password);
    m_socket->write(encodedPassword.toUtf8() + "\r\n");
    if (!waitForResponse()) {
        qDebug() << "密码发送失败";
        return false;
    }
    qDebug() << "密码响应:" << m_response.trimmed();

    return m_response.startsWith("235");  // 235表示认证成功
}

void EmailSender::disconnectFromServer()
{
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        m_socket->write("QUIT\r\n");
        m_socket->waitForBytesWritten();
        m_socket->disconnectFromHost();
    }
}

QString EmailSender::encodeBase64(const QString& input)
{
    return input.toUtf8().toBase64();
}

bool EmailSender::waitForResponse(int timeoutMs)
{
    m_timeoutTimer->start(timeoutMs);
    m_response.clear();

    while (m_socket->canReadLine() || m_socket->waitForReadyRead(timeoutMs)) {
        while (m_socket->canReadLine()) {
            QString line = m_socket->readLine();
            m_response += line;

            // 检查是否是最终响应
            if (line.length() >= 4 && line[3] != '-') {
                m_timeoutTimer->stop();
                return true;
            }
        }
    }

    m_timeoutTimer->stop();
    return false;
}
