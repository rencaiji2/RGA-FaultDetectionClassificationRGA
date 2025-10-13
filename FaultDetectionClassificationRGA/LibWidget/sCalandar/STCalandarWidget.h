#pragma once
#include <qdialog.h>
#include "STDateWidget.h"
#include <QLabel>
#include <QPushButton>
class STCalandarWidget :
    public QWidget
{
    Q_OBJECT
public:
    STCalandarWidget(QWidget* parrent = nullptr);
    ~STCalandarWidget();
    void SetCurrentDate(int year, int month, int day);
    QDate GetCurrentDate();
private:
    void FillCalandar();
    void initLabels();
    void initCalandar();
    void init();
    QString getFormatMonth();
private:
    QLabel *weeklabels[7];
    STDateWidget *datewidgets[42];
    QPushButton *lastYearButton;
    QPushButton *lastMonthButton;
    QPushButton *nextMonthButton;
    QPushButton *nextYearButton;
    QDate currentDate;
    QLabel *cdlabel;
public slots:
    void HaveDateSelect(QDate date);
    void JumpLastYear();
    void JumpLastMonth();
    void JumpNextMonth();
    void JumpNextYear();

signals:
    void DateSelectSignal(QDate date);
};

