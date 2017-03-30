#include "tableviewdialog.h"
#include "ui_tableviewdialog.h"
#include "QSettings"
#include "QDir"
#include "QCheckBox"
TableViewDialog::TableViewDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TableViewDialog)
{
    ui->setupUi(this);
    ui->tableWidget->setColumnCount(1);
    QStringList columnNames;
    columnNames << "Параметр" << "Отображение";
    ui->tableWidget->setHorizontalHeaderLabels(columnNames);
    settings = new QSettings(QCoreApplication::applicationDirPath()+QDir::separator()+"config.ini",QSettings::IniFormat,this);
    QStringList rowsNames;
    rowsNames = readSettings();
    ui->tableWidget->setRowCount(rowsNames.count());
    ui->tableWidget->setVerticalHeaderLabels(rowsNames);
    QCheckBox *pCheckBox ;
    for (int i=0; i<ui->tableWidget->rowCount();++i)
    {
        pCheckBox = new QCheckBox();
        ui->tableWidget->setCellWidget(i,0,pCheckBox);
        ui->tableWidget->cellWidget(i,0)->setStyleSheet("margin-left:50%;margin-right:50%");

    }

    setCheckedRows(rowsNames);
    connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(okClicked()));
    ui->tableWidget->setTabKeyNavigation(false);
}

TableViewDialog::~TableViewDialog()
{
    delete ui;
}

QStringList TableViewDialog::readSettings()
{
    settings->beginGroup("/WindowsSettings");
    QStringList rowsNames;
    rowsNames << "FFID" << "Source Line" << "Source Point" << "Source X" << "Source Y"<<"Source Z";
    int size = settings->beginReadArray("/Windows");
    for (int i =0; i<size; ++i)
    {
        settings->setArrayIndex(i);
        if (settings->value("/CountAmpl","0").toBool())
        {
            rowsNames<<QString("Abs A#%1").arg(i+1);
        }
        if (settings->value("/CountRms","0").toBool())
        {
            rowsNames<<QString("RMS #%1").arg(i+1);
        }
        if (settings->value("/CountFreq","0").toBool())
        {
            rowsNames<<QString("Frq #%1").arg(i+1);
        }
        if(settings->value("/CountEnergy","0").toBool())
        {
            rowsNames  <<QString("Energy #%1").arg(i+1);
        }
        if(settings->value("/CountDfr","0").toBool())
        {
            rowsNames <<QString("DFR #%1").arg(i+1);
        }
    }
    settings->endArray();
    size = settings->beginReadArray("/Relations");
    for (int i =0; i<size; ++i)
    {
        settings->setArrayIndex(i);
        rowsNames << QString("%1/%2").arg(settings->value("/A1","A1").toString()).arg(settings->value("/A2","A1").toString());
    }
    settings->endArray();
    settings->endGroup();
    settings->beginGroup("/ConvertSettings");
    if (settings->value("/AnalisysAuxes",false).toBool())
    {
        rowsNames<<"Check Aux";
    }
    if (settings->value("/CheckTests",false).toBool())
    {
        rowsNames<<"Check Tests";
    }
    settings->endGroup();
    return rowsNames;

}

void TableViewDialog::okClicked()
{
    QStringList tableForView;
    QCheckBox *pCheckBox;
    for (int i=0; i<ui->tableWidget->rowCount();++i)
    {
        pCheckBox = qobject_cast<QCheckBox*> (ui->tableWidget->cellWidget(i,0));
        if (pCheckBox->isChecked())
        {
            tableForView.append(ui->tableWidget->verticalHeaderItem(i)->text());
        }
    }
    emit sendTableColumns(tableForView);
    saveSettings(tableForView);
    accept();
}

void TableViewDialog::saveSettings(const QStringList checkedRows)
{
    settings->beginGroup("ViewSettings");
    settings->beginWriteArray("/ColumnForView");
    for (int i=0; i<checkedRows.count();++i)
    {
        settings->setArrayIndex(i);
        settings->setValue("/ColumnForView",checkedRows.value(i));
    }
    settings->endArray();
    settings->endGroup();
}

void TableViewDialog::setCheckedRows(QStringList rows)
{
    settings->beginGroup("/ViewSettings");
    int size = settings->beginReadArray("/ColumnForView");
    //ui->tableWidget->setColumnCount(size);
    QCheckBox *pCheckBox; // чекбокс в таблице
    QString checkedRow; // колонка для отметки из настроек
    int row;
    for (int i=0;i<size;i++)
    {
        settings->setArrayIndex(i);
        checkedRow=settings->value("/ColumnForView","").toString();
        row = rows.indexOf(checkedRow);
        if (row!=-1)
        {
            pCheckBox = qobject_cast<QCheckBox*> (ui->tableWidget->cellWidget(row,0));
            pCheckBox->setChecked(true);
        }
    }
    settings->endArray();
    settings->endGroup();
}
