#include "HtmlReportGenerator.h"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QBuffer>
#include <QByteArray>
#include <QDateTime>
#include <QDebug>

/*
QString HtmlReportGenerator::generateHtmlReport(const QMap<QString, QImage>& imageMap,
                                              const ReportData& data,
                                              const ReportConfig& config)
{
    QString html;
    
    // 生成HTML头部
    html += generateHtmlHeader(config);
    
    // 生成样式
    html += generateHtmlStyles(config);
    
    // 生成主体内容
    html += "<body>\n";
    html += generateReportContent(imageMap, data, config);
    html += "</body>\n";
    
    // 生成HTML尾部
    html += generateHtmlFooter();
    
    return html;
}
*/

QString HtmlReportGenerator::generateHtmlReport(const QMap<QString, QImage>& images,
                                               const ReportData& data,
                                               const ReportConfig& config)
{
    QString html;
    QTextStream stream(&html);

    // HTML头部
    stream << "<!DOCTYPE html>\n";
    stream << "<html>\n<head>\n";
    stream << "<meta charset=\"UTF-8\">\n";
    stream << "<title>" << config.title << "</title>\n";
    stream << "<style>\n";
    stream << "body { font-family: " << config.fontFamily << "; font-size: " << config.fontSize << "px; }\n";
    stream << "h1 { font-size: " << config.titleFontSize << "px; }\n";
    stream << "h2 { font-size: " << config.imageTitleFontSize << "px; }\n";
    stream << ".image-container { margin: 20px 0; text-align: center; }\n";
    stream << ".image-title { font-weight: bold; margin: 10px 0; }\n";
    stream << ".section { margin: 15px 0; }\n";
    stream << "</style>\n";
    stream << "</head>\n<body>\n";

    // 标题和问候语
    stream << "<h1>" << config.title << "</h1>\n";
    stream << "<p>" << config.greeting << " " << data.recipientName << "</p>\n";

    // 介绍部分 - 支持多行
    stream << "<div class=\"section\">\n";
    stream << "<p>" << config.introduction << "</p>\n";

    if (!data.customIntroductionLines.isEmpty()) {
        stream << "<ul>\n";
        for (const QString& line : data.customIntroductionLines) {
            if (!line.isEmpty()) {
                stream << "<li>" << line << "</li>\n";
            }
        }
        stream << "</ul>\n";
    } else if (!data.customIntroduction.isEmpty()) {
        // 兼容原有的单行介绍
        stream << "<p>" << data.customIntroduction << "</p>\n";
    }
    stream << "</div>\n";

    // 图片部分
    for (auto it = images.constBegin(); it != images.constEnd(); ++it) {
        QString imageKey = it.key();
        const QImage& image = it.value();

        // 将图片转换为Base64
        QByteArray byteArray;
        QBuffer buffer(&byteArray);
        buffer.open(QIODevice::WriteOnly);
        image.save(&buffer, "PNG");

        QString base64Image = QString::fromLatin1(byteArray.toBase64().data());

        stream << "<div class=\"image-container\">\n";
        stream << "<div class=\"image-title\">" << imageKey << "</div>\n";
        stream << "<img src=\"data:image/png;base64," << base64Image << "\" alt=\"" << imageKey << "\">\n";
        stream << "</div>\n";
    }

    // 结论部分 - 支持多行
    stream << "<div class=\"section\">\n";
    stream << "<p>" << config.conclusion << "</p>\n";

    if (!data.customConclusionLines.isEmpty()) {
        stream << "<ul>\n";
        for (const QString& line : data.customConclusionLines) {
            if (!line.isEmpty()) {
                stream << "<li>" << line << "</li>\n";
            }
        }
        stream << "</ul>\n";
    } else if (!data.customConclusion.isEmpty()) {
        // 兼容原有的单行结论
        stream << "<p>" << data.customConclusion << "</p>\n";
    }
    stream << "</div>\n";

    // 报告时间
    stream << "<p><i>Report time:" << QDateTime::currentDateTime().toString("yyyyMMdd hh:mm:ss") << "</i></p>\n";

    stream << "</body>\n</html>\n";

    return html;
}

/*!
 * \brief HtmlReportGenerator::generateHtmlReportV2 创建告警html的函数
 * \param images
 * \param data
 * \param config
 * \param rows
 * \param cols
 * \return
 */
QString HtmlReportGenerator::generateHtmlReportV2(const QMap<QString, QImage>& images,
                                                 const ReportData& data,
                                                 const ReportConfig& config,
                                                 int rows, int cols)
{
    QString html;
    QTextStream stream(&html);

    // HTML头部
    stream << "<!DOCTYPE html>\n";
    stream << "<html>\n<head>\n";
    stream << "<meta charset=\"UTF-8\">\n";
    stream << "<title>" << config.title << "</title>\n";
    stream << "<style>\n";
    stream << "body { font-family: " << config.fontFamily << "; font-size: " << config.fontSize << "px; }\n";
    stream << "h1 { font-size: " << config.titleFontSize << "px; }\n";
    stream << "h2 { font-size: " << config.imageTitleFontSize << "px; }\n";
    stream << ".section { margin: 15px 0; }\n";
    stream << ".image-table { border-collapse: collapse; width: 100%; }\n";
    stream << ".image-table td { border: 1px solid #ddd; padding: 10px; text-align: center; vertical-align: top; }\n";
    stream << ".image-container { margin: 0 auto; }\n";
    stream << ".image-title { font-weight: bold; margin: 5px 0; font-size: " << (config.imageTitleFontSize - 2) << "px; }\n";
    stream << ".image-cell img { max-width: 100%; height: auto; }\n";
    stream << ".highlight { font-weight: bold; color: red; }\n";  // 加粗标红样式
    stream << ".intro-list { font-size: " << config.fontSize << "px; }\n";  // 介绍列表字体大小
    stream << ".intro-list li { line-height: 1.5; }\n";  // 行高调整
    stream << ".conclusion-list { font-size: " << config.fontSize << "px; }\n";  // 结论列表字体大小
    stream << ".conclusion-list li { line-height: 1.5; }\n";  // 行高调整
    stream << "</style>\n";
    stream << "</head>\n<body>\n";

    // 标题和问候语
    stream << "<h1>" << config.title << "</h1>\n";
    stream << "<p>" << config.greeting << " " << data.recipientName << "</p>\n";

    // 介绍部分
    stream << "<div class=\"section\">\n";
    stream << "<p>" << config.introduction << "</p>\n";

    if (!data.customIntroductionLines.isEmpty()) {
        stream << "<ul class=\"intro-list\">\n";
        for (const QString& line : data.customIntroductionLines) {
            if (!line.isEmpty()) {
                // 对具体值进行加粗标红（而不是字段名）
                QString formattedLine = line;

                // 查找并高亮具体的值
                // 例如: "deviceName:ABC123" -> "deviceName:<span class="highlight">ABC123</span>"
                QRegExp deviceRegex("deviceName:([^,]+)");
                formattedLine.replace(deviceRegex, "deviceName:<span class=\"highlight\">\\1</span>");

                QRegExp itemRegex("item:([^,]+)");
                formattedLine.replace(itemRegex, "item:<span class=\"highlight\">\\1</span>");

                QRegExp chamberRegex("chamberID:([^,]+)");
                formattedLine.replace(chamberRegex, "chamberID:<span class=\"highlight\">\\1</span>");

                QRegExp hostRegex("hostID:([^,]+)");
                formattedLine.replace(hostRegex, "hostID:<span class=\"highlight\">\\1</span>");

                QRegExp ratioRegex("ratioName:([^,]+)");
                formattedLine.replace(ratioRegex, "ratioName:<span class=\"highlight\">\\1</span>");

                // 处理其他单独行的值
                if (line.startsWith("chamberID:")) {
                    int colonIndex = line.indexOf(':');
                    if (colonIndex != -1) {
                        QString prefix = line.left(colonIndex + 1);
                        QString value = line.mid(colonIndex + 1);
                        formattedLine = prefix + "<span class=\"highlight\">" + value + "</span>";
                    }
                }

                stream << "<li>" << formattedLine << "</li>\n";
            }
        }
        stream << "</ul>\n";
    } else if (!data.customIntroduction.isEmpty()) {
        // 对特定字段的值加粗标红
        QString formattedIntro = data.customIntroduction;
        stream << "<p>" << formattedIntro << "</p>\n";
    }
    stream << "</div>\n";

    // 图片表格部分
    if (!images.isEmpty()) {
        stream << "<div class=\"section\">\n";
        stream << "<h2>Chart List</h2>\n";
        stream << "<table class=\"image-table\">\n";

        // 将图片转换为列表以便按行列排列
        QList<QString> imageKeys = images.keys();
        int imageCount = imageKeys.size();

        // 按行列生成表格
        for (int row = 0; row < rows; row++) {
            stream << "<tr>\n";
            for (int col = 0; col < cols; col++) {
                int index = row * cols + col;
                stream << "<td>\n";

                if (index < imageCount) {
                    QString imageKey = imageKeys[index];
                    const QImage& image = images.value(imageKey);

                    // 将图片转换为Base64
                    QByteArray byteArray;
                    QBuffer buffer(&byteArray);
                    buffer.open(QIODevice::WriteOnly);
                    image.save(&buffer, "PNG");

                    QString base64Image = QString::fromLatin1(byteArray.toBase64().data());

                    stream << "<div class=\"image-container\">\n";
                    stream << "<div class=\"image-title\">" << imageKey << "</div>\n";
                    stream << "<div class=\"image-cell\">\n";
                    stream << "<img src=\"data:image/png;base64," << base64Image << "\" alt=\"" << imageKey << "\">\n";
                    stream << "</div>\n";
                    stream << "</div>\n";
                }

                stream << "</td>\n";
            }
            stream << "</tr>\n";
        }

        stream << "</table>\n";
        stream << "</div>\n";
    }

    // 结论部分
    stream << "<div class=\"section\">\n";
    stream << "<p>" << config.conclusion << "</p>\n";

    if (!data.customConclusionLines.isEmpty()) {
        stream << "<ul class=\"conclusion-list\">\n";
        for (const QString& line : data.customConclusionLines) {
            if (!line.isEmpty()) {
                stream << "<li>" << line << "</li>\n";
            }
        }
        stream << "</ul>\n";
    } else if (!data.customConclusion.isEmpty()) {
        stream << "<p>" << data.customConclusion << "</p>\n";
    }
    stream << "</div>\n";

    // 报告时间
    stream << "<p><i>create report time:" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "</i></p>\n";

    stream << "</body>\n</html>\n";

    return html;
}

QString HtmlReportGenerator::generateHtmlReportWithImageFiles(const QMap<QString, QImage>& imageMap,
                                                            const QString& outputDir,
                                                            const ReportData& data,
                                                            const ReportConfig& config)
{
    // 确保输出目录存在
    QDir dir(outputDir);
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qWarning() << "无法创建输出目录:" << outputDir;
            return QString();
        }
    }
    
    // 保存图片文件
    QString imageDir = outputDir + "/images";
    if (!saveImages(imageMap, imageDir)) {
        qWarning() << "保存图片文件失败";
        return QString();
    }
    
    // 创建配置（不嵌入图片）
    ReportConfig localConfig = config;
    localConfig.embedImages = false;
    localConfig.imageDirectory = imageDir;
    
    return generateHtmlReport(imageMap, data, localConfig);
}

bool HtmlReportGenerator::saveImages(const QMap<QString, QImage>& imageMap, const QString& directory)
{
    QDir dir(directory);
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            return false;
        }
    }
    
    bool allSaved = true;
    
    for (auto it = imageMap.constBegin(); it != imageMap.constEnd(); ++it) {
        QString fileName = sanitizeFileName(it.key()) + ".png";
        QString filePath = directory + "/" + fileName;
        
        if (!it.value().save(filePath, "PNG")) {
            qWarning() << "保存图片失败:" << filePath;
            allSaved = false;
        }
    }
    
    return allSaved;
}

QString HtmlReportGenerator::imageToBase64(const QImage& image, const QString& format)
{
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, format.toLocal8Bit().data());
    return byteArray.toBase64();
}

QString HtmlReportGenerator::sanitizeFileName(const QString& fileName)
{
    QString cleanName = fileName;
    cleanName.replace("/", "_").replace("\\", "_").replace(":", "_");
    cleanName.replace("*", "_").replace("?", "_").replace("\"", "_");
    cleanName.replace("<", "_").replace(">", "_").replace("|", "_");
    return cleanName;
}

QString HtmlReportGenerator::generateHtmlHeader(const ReportConfig& config)
{
    QString header = QString(
        "<!DOCTYPE html>\n"
        "<html lang=\"zh-CN\">\n"
        "<head>\n"
        "    <meta charset=\"utf-8\">\n"
        "    <meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n"
        "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
        "    <title>%1</title>\n"
        "</head>\n"
    ).arg(config.title);

    return header;
}

QString HtmlReportGenerator::generateHtmlStyles(const ReportConfig& config)
{
    QString styles = QString(
        "<style>\n"
        "    body {\n"
        "        font-family: %2;\n"
        "        font-size: %3px;\n"
        "        background-color: %4;\n"
        "        color: %5;\n"
        "        margin: 20px;\n"
        "        line-height: 1.6;\n"
        "    }\n"
        "    h1 {\n"
        "        font-size: %6px;\n"
        "        color: #2c3e50;\n"
        "        border-bottom: 2px solid #3498db;\n"
        "        padding-bottom: 10px;\n"
        "        margin-bottom: 20px;\n"
        "    }\n"
        "    h2 {\n"
        "        font-size: %7px;\n"
        "        color: #34495e;\n"
        "        margin-top: 30px;\n"
        "        margin-bottom: 15px;\n"
        "    }\n"
        "    .greeting {\n"
        "        font-weight: bold;\n"
        "        font-size: %3px;\n"
        "        margin-bottom: 15px;\n"
        "    }\n"
        "    .introduction {\n"
        "        margin-bottom: 25px;\n"
        "        background-color: #f8f9fa;\n"
        "        padding: 15px;\n"
        "        border-left: 4px solid #007bff;\n"
        "    }\n"
        "    .image-container {\n"
        "        margin: 20px 0;\n"
        "        text-align: center;\n"
        "    }\n"
        "    .image-title {\n"
        "        font-weight: bold;\n"
        "        margin: 10px 0;\n"
        "        color: #555;\n"
        "    }\n"
        "    .image-item {\n"
        "        max-width: %8px;\n"
        "        height: auto;\n"
        "        border: 1px solid #ddd;\n"
        "        border-radius: 4px;\n"
        "        box-shadow: 0 2px 4px rgba(0,0,0,0.1);\n"
        "    }\n"
        "    .conclusion {\n"
        "        margin-top: 30px;\n"
        "        padding: 15px;\n"
        "        background-color: #e9f7ef;\n"
        "        border-left: 4px solid #27ae60;\n"
        "        font-style: italic;\n"
        "    }\n"
        "    .report-info {\n"
        "        font-size: %9px;\n"
        "        color: #666;\n"
        "        text-align: right;\n"
        "        margin-bottom: 20px;\n"
        "    }\n"
        "</style>\n"
    ).arg(config.fontFamily)
     .arg(config.fontSize)
     .arg(config.backgroundColor)
     .arg(config.textColor)
     .arg(config.titleFontSize)
     .arg(config.imageTitleFontSize)
     .arg(config.imageWidth)
     .arg(config.fontSize - 2);
    
    return styles;
}

QString HtmlReportGenerator::generateReportContent(const QMap<QString, QImage>& imageMap,
                                                 const ReportData& data,
                                                 const ReportConfig& config)
{
    QString content;
    
    // 标题
    content += QString("<h1>%1</h1>\n").arg(config.title);
    
    // 报告信息
    QString reportTime = data.reportTime.isValid() ? 
        data.reportTime.toString("yyyyMMdd hh:mm:ss") :
        QDateTime::currentDateTime().toString("yyyyMMdd hh:mm:ss");
    content += QString("<div class=\"report-info\">create report time:%1</div>\n").arg(reportTime);
    
    // 问候语
    content += generateGreeting(data, config);
    
    // 介绍语
    content += generateIntroduction(data, config);
    
    // 图片部分
    content += generateImageSection(imageMap, config);
    
    // 结束语
    content += generateConclusion(data, config);
    
    return content;
}

QString HtmlReportGenerator::generateGreeting(const ReportData& data, const ReportConfig& config)
{
    QString greeting = data.recipientName.isEmpty() ? "userName" : data.recipientName;
    return QString("<div class=\"greeting\">%1%2</div>\n").arg(config.greeting,greeting);
}

QString HtmlReportGenerator::generateIntroduction(const ReportData& data, const ReportConfig& config)
{
    QString introText = data.customIntroduction.isEmpty() ? config.introduction : data.customIntroduction;
    QString timeText = data.reportTime.isValid() ? 
        data.reportTime.toString("yyyyMMdd hh:mm") :
        QDateTime::currentDateTime().toString("yyyyMMdd hh:mm");
    
    return QString(
        "<div class=\"introduction\">\n"
        "    <p>%1%2</p>\n"
        "</div>\n"
    ).arg(timeText, introText);
}

QString HtmlReportGenerator::generateImageSection(const QMap<QString, QImage>& imageMap,
                                                const ReportConfig& config)
{
    QString section;
    int index = 1;
    
    for (auto it = imageMap.constBegin(); it != imageMap.constEnd(); ++it) {
        section += generateImageItem(it.key(), it.value(), config, index++);
    }
    
    return section;
}

QString HtmlReportGenerator::generateImageItem(const QString& title, 
                                             const QImage& image,
                                             const ReportConfig& config,
                                             int index)
{
    QString item;
    
    // 图片标题
    QString displayTitle = title.isEmpty() ? QString("图片%1").arg(index) : title;
    item += QString("<h2 class=\"image-title\">%1</h2>\n").arg(displayTitle);
    
    // 图片
    item += "<div class=\"image-container\">\n";
    
    if (config.embedImages) {
        // 嵌入Base64图片
        QString base64Data = imageToBase64(image, "PNG");
        item += QString(
            "    <img src=\"data:image/png;base64,%1\" \n"
            "         alt=\"%2\" \n"
            "         class=\"image-item\" \n"
            "         style=\"max-width: %3px; max-height: %4px;\">\n"
        ).arg(base64Data)
         .arg(displayTitle)
         .arg(config.imageWidth)
         .arg(config.imageHeight);
    } else {
        // 使用文件路径
        QString fileName = sanitizeFileName(title) + ".png";
        QString imagePath = config.imageDirectory + "/" + fileName;
        item += QString(
            "    <img src=\"%1\" \n"
            "         alt=\"%2\" \n"
            "         class=\"image-item\" \n"
            "         style=\"max-width: %3px; max-height: %4px;\">\n"
        ).arg(imagePath)
         .arg(displayTitle)
         .arg(config.imageWidth)
         .arg(config.imageHeight);
    }
    
    item += "</div>\n";
    
    return item;
}

QString HtmlReportGenerator::generateConclusion(const ReportData& data, const ReportConfig& config)
{
    QString conclusionText = data.customConclusion.isEmpty() ? config.conclusion : data.customConclusion;
    return QString(
        "<div class=\"conclusion\">\n"
        "    <p>%1</p>\n"
        "</div>\n"
    ).arg(conclusionText);
}

QString HtmlReportGenerator::generateHtmlFooter()
{
    return "</html>\n";
}
