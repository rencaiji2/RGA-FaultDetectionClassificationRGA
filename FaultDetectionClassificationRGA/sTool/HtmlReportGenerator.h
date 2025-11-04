#ifndef HTMLREPORTGENERATOR_H
#define HTMLREPORTGENERATOR_H

#include <QMap>
#include <QImage>
#include <QString>
#include <QDateTime>
#include <QFileInfo>

class HtmlReportGenerator
{
public:
    struct ReportConfig {
        ReportConfig(){}
        QString title = "报警信息报告";
        QString greeting = "您好：";
        QString introduction = "报警信息如下：";
        QString conclusion = "请关注以上报警信息，及时处理相关问题。";
        QString fontFamily = "Arial, 'Microsoft Yahei', sans-serif";
        int fontSize = 14;
        int titleFontSize = 18;
        int imageTitleFontSize = 16;
        QString backgroundColor = "#ffffff";
        QString textColor = "#333333";
        int imageWidth = 800;  // 图片显示宽度
        int imageHeight = 600; // 图片显示高度
        bool embedImages = true; // 是否嵌入图片（base64）或使用文件路径
        QString imageDirectory = "./images"; // 图片保存目录（当不嵌入时使用）
    };
    
    struct ReportData {
        QString recipientName;
        QDateTime reportTime;
        QString customIntroduction;
        QString customConclusion;
        QStringList customIntroductionLines;  // 新增：多行介绍
        QStringList customConclusionLines;    // 新增：多行结论
    };
    
    // 生成完整的HTML报告
    static QString generateHtmlReport(const QMap<QString, QImage>& imageMap,
                                    const ReportData& data = ReportData(),
                                    const ReportConfig& config = ReportConfig());

    static QString generateHtmlReportV2(const QMap<QString, QImage>& images,
                                                     const ReportData& data,
                                                     const ReportConfig& config,
                                                     int rows, int cols);
    
    // 保存图片并生成HTML（不嵌入图片）
    static QString generateHtmlReportWithImageFiles(const QMap<QString, QImage>& imageMap,
                                                  const QString& outputDir,
                                                  const ReportData& data = ReportData(),
                                                  const ReportConfig& config = ReportConfig());
    
    // 保存图片到指定目录
    static bool saveImages(const QMap<QString, QImage>& imageMap, const QString& directory);
    
    // 将图片转换为Base64编码
    static QString imageToBase64(const QImage& image, const QString& format = "PNG");
    
    // 清理文件名中的非法字符
    static QString sanitizeFileName(const QString& fileName);

private:
    // 生成HTML头部
    static QString generateHtmlHeader(const ReportConfig& config);
    
    // 生成HTML样式
    static QString generateHtmlStyles(const ReportConfig& config);
    
    // 生成报告主体内容
    static QString generateReportContent(const QMap<QString, QImage>& imageMap,
                                       const ReportData& data,
                                       const ReportConfig& config);
    
    // 生成问候语
    static QString generateGreeting(const ReportData& data, const ReportConfig& config);
    
    // 生成介绍语
    static QString generateIntroduction(const ReportData& data, const ReportConfig& config);
    
    // 生成图片部分
    static QString generateImageSection(const QMap<QString, QImage>& imageMap,
                                      const ReportConfig& config);
    
    // 生成结束语
    static QString generateConclusion(const ReportData& data, const ReportConfig& config);
    
    // 生成HTML尾部
    static QString generateHtmlFooter();
    
    // 生成单个图片项
    static QString generateImageItem(const QString& title, 
                                   const QImage& image,
                                   const ReportConfig& config,
                                   int index = 0);
};

#endif // HTMLREPORTGENERATOR_H
