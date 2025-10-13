#include "STCalandarWidget.h"
#include <QDebug>
STCalandarWidget::STCalandarWidget(QWidget* parrent /* = nullptr */) :QWidget(parrent) {
    //this->setStyleSheet(QString::fromLocal8Bit("font:15px ; background-color:rgb(250,250,250)"));
    this->setMinimumSize(580, 450);
    this->setMaximumSize(580, 450);
//    Qt::WindowFlags flags = Qt::Dialog;
//    flags |= Qt::WindowCloseButtonHint;
//    setWindowFlags(flags);
    init();
}

STCalandarWidget::~STCalandarWidget()
{
}

void STCalandarWidget::SetCurrentDate(int year, int month, int day)
{
    currentDate.setDate(year, month, day);
}

QDate STCalandarWidget::GetCurrentDate()
{
    return currentDate;
}

void STCalandarWidget::FillCalandar()
{
    QDate firstDay;
    firstDay.setDate(currentDate.year(), currentDate.month(), 1);
    int firstnum = firstDay.dayOfWeek();
    qDebug() << firstnum;
    QDate firstDayOfMonth = firstDay.addDays(-(firstnum - 1));
    for (int i = 0; i < 42; i++) {
        if (i < firstnum-1 || (firstDayOfMonth.month() != currentDate.month())) {
            datewidgets[i]->SetDate(firstDayOfMonth.year(), firstDayOfMonth.month(), firstDayOfMonth.day(), false);
        }
        else {
            datewidgets[i]->SetDate(firstDayOfMonth.year(), firstDayOfMonth.month(), firstDayOfMonth.day(), true);
        }
        firstDayOfMonth = firstDayOfMonth.addDays(1);
    }
}

void STCalandarWidget::initLabels()
{
    for (int i = 0; i < 7; i++) {
        weeklabels[i] = new QLabel(this);
        weeklabels[i]->setGeometry(35 + i*80, 50, 80, 40);
    }
    weeklabels[0]->setText("Mon");
    weeklabels[1]->setText("Tue");
    weeklabels[2]->setText("Wed");
    weeklabels[3]->setText("Thu");
    weeklabels[4]->setText("Fri");
    weeklabels[5]->setText("Sat");
    weeklabels[6]->setText("Sun");
}

void STCalandarWidget::initCalandar()
{
    for (int i = 0; i < 42; i++) {
        datewidgets[i] = new STDateWidget(this);
        datewidgets[i]->setGeometry(10 + i % 7 * 80, 80 + i / 7 * 60, 80, 60);
        connect(datewidgets[i], SIGNAL(updateCurrentDate(QDate)), this, SLOT(HaveDateSelect(QDate)));
    }
    for (int i = 0; i < 42; i++) {
        if (i / 7 == 0 ) {
            if (i % 7 == 0) {
                datewidgets[i]->AddNeighbor(datewidgets[i + 1], STDateWidget::DIR_RIGHT);
            }
            else if (i % 7 == 6) {
                datewidgets[i]->AddNeighbor(datewidgets[i - 1], STDateWidget::DIR_LEFT);
            }
            else {
                datewidgets[i]->AddNeighbor(datewidgets[i + 1], STDateWidget::DIR_RIGHT);
                datewidgets[i]->AddNeighbor(datewidgets[i - 1], STDateWidget::DIR_LEFT);
            }
            datewidgets[i]->AddNeighbor(datewidgets[i + 7], STDateWidget::DIR_BOTTOM);
        }
        else if (i / 7 == 5) {
            if (i % 7 == 0) {
                datewidgets[i]->AddNeighbor(datewidgets[i + 1], STDateWidget::DIR_RIGHT);
            }
            else if (i % 7 == 6) {
                datewidgets[i]->AddNeighbor(datewidgets[i - 1], STDateWidget::DIR_LEFT);
            }
            else {
                datewidgets[i]->AddNeighbor(datewidgets[i + 1], STDateWidget::DIR_RIGHT);
                datewidgets[i]->AddNeighbor(datewidgets[i - 1], STDateWidget::DIR_LEFT);
            }
            datewidgets[i]->AddNeighbor(datewidgets[i - 7], STDateWidget::DIR_TOP);
        }
        else
        {
            if (i % 7 == 0) {
                datewidgets[i]->AddNeighbor(datewidgets[i + 1], STDateWidget::DIR_RIGHT);
            }
            else if (i % 7 == 6) {
                datewidgets[i]->AddNeighbor(datewidgets[i - 1], STDateWidget::DIR_LEFT);
            }
            else {
                datewidgets[i]->AddNeighbor(datewidgets[i + 1], STDateWidget::DIR_RIGHT);
                datewidgets[i]->AddNeighbor(datewidgets[i - 1], STDateWidget::DIR_LEFT);
            }
            datewidgets[i]->AddNeighbor(datewidgets[i - 7], STDateWidget::DIR_TOP);
            datewidgets[i]->AddNeighbor(datewidgets[i + 7], STDateWidget::DIR_BOTTOM);
        }
    }
    FillCalandar();
}

void STCalandarWidget::init()
{
    currentDate = QDate::currentDate();
    lastYearButton = new QPushButton(this);
    lastYearButton->setGeometry(10, 10, 100, 30);
    lastYearButton->setText("<<");

    lastMonthButton = new QPushButton(this);
    lastMonthButton->setGeometry(120, 10, 100, 30);
    lastMonthButton->setText("<");

    cdlabel = new QLabel(this);
    cdlabel->setGeometry(255, 10, 100, 40);
    cdlabel->setText(getFormatMonth());

    nextMonthButton = new QPushButton(this);
    nextMonthButton->setGeometry(360, 10, 100, 30);
    nextMonthButton->setText(">");

    nextYearButton = new QPushButton(this);
    nextYearButton->setGeometry(470, 10, 100, 30);
    nextYearButton->setText(">>");

    connect(lastYearButton, SIGNAL(clicked()), this, SLOT(JumpLastYear()));
    connect(lastMonthButton, SIGNAL(clicked()), this, SLOT(JumpLastMonth()));
    connect(nextMonthButton, SIGNAL(clicked()), this, SLOT(JumpNextMonth()));
    connect(nextYearButton, SIGNAL(clicked()), this, SLOT(JumpNextYear()));
    initLabels();
    initCalandar();
}
QString STCalandarWidget::getFormatMonth()
{
    QString ans = QString::number(currentDate.month());
    ans += QString::fromLocal8Bit(" / ");
    ans += QString::number(currentDate.year());
    //ans += QString::fromLocal8Bit(" Moon ");
    return ans;
}
void STCalandarWidget::HaveDateSelect(QDate date)
{
    qDebug() << date;
    emit DateSelectSignal(date);
}
void STCalandarWidget::JumpLastYear()
{
    currentDate = currentDate.addYears(-1);
    FillCalandar();
    cdlabel->setText(getFormatMonth());
}

void STCalandarWidget::JumpLastMonth()
{
    currentDate = currentDate.addMonths(-1);
    FillCalandar();
    cdlabel->setText(getFormatMonth());
}

void STCalandarWidget::JumpNextMonth()
{
    currentDate = currentDate.addMonths(1);
    FillCalandar();
    cdlabel->setText(getFormatMonth());
}

void STCalandarWidget::JumpNextYear()
{
    currentDate = currentDate.addYears(1);
    FillCalandar();
    cdlabel->setText(getFormatMonth());
}
