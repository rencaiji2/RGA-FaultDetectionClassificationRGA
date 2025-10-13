#include "sEditSIM.h"


sEditSIM::sEditSIM(QWidget *parent) :
    QWidget(parent)/*,
    ui(new Ui::sEditSIM)*/
{
    ui.setupUi(this);
    ui.UI_TW_EDITSIM->setAlternatingRowColors(true);
    ui.UI_TW_EDITSIM->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    QStringList str_list;
    str_list<<"2"<<"4"<<"8"<<"16"<<"32"<<"64"
           <<"128"<<"256"<<"512"<<"1024"
          <<"2048" <<"4096"<<"8192"<<"16384";
    ui.UI_CB_HOLDTIME_EDITSIM->addItems(str_list);
}

sEditSIM::~sEditSIM()
{
    //delete ui;
}

void sEditSIM::addRow(QString mass, QString name, QString holdTimeMs)
{
    int rowCount = ui.UI_TW_EDITSIM->rowCount();
    for(int i= 0; i< rowCount; ++i){
        int newMass= mass.toInt();
        int tmpMass= ui.UI_TW_EDITSIM->item(i,0)->text().toInt();
        if(newMass==tmpMass){
            return;
        }
        if(newMass< tmpMass){
            rowCount =i;
            break;
        }
    }
    ui.UI_TW_EDITSIM-> insertRow(rowCount);
    QTableWidgetItem *item = new QTableWidgetItem(mass);
    ui.UI_TW_EDITSIM-> setItem(rowCount,0, item);
    item = new QTableWidgetItem(name);
    ui.UI_TW_EDITSIM-> setItem(rowCount,1, item);

    QComboBox* ComboBox=new QComboBox();
    QStringList str_list;
    str_list<<"2"<<"4"<<"8"<<"16"<<"32"<<"64"
           <<"128"<<"256"<<"512"<<"1024"
          <<"2048" <<"4096"<<"8192"<<"16384";
    ComboBox->addItems(str_list);
    ComboBox->setCurrentText(holdTimeMs);
    ui.UI_TW_EDITSIM->setCellWidget(rowCount, 2, ComboBox);
}

void sEditSIM::on_U_PB_ADD_EDITSIM_clicked()
{
    addRow(ui.UI_LE_MASS_EDITSIM->text(), ui.UI_LE_NAME_EDITSIM->text(), ui.UI_CB_HOLDTIME_EDITSIM->currentText());
}

void sEditSIM::on_UI_PB_CHEMISTRY_EDITSIM_clicked()
{

}

void sEditSIM::on_UI_PB_DELETE_EDITSIM_clicked()
{
    ui.UI_TW_EDITSIM-> removeRow(ui.UI_TW_EDITSIM->currentRow());
}

void sEditSIM::on_UI_PB_CLEAR_EDITSIM_clicked()
{
    ui.UI_TW_EDITSIM-> clear();
    QStringList babels;
    babels<< "质量数"<< "物质名称"<< "驻留时间(ms)";
    ui.UI_TW_EDITSIM-> setHorizontalHeaderLabels(babels);
}
