#include "windowsparametersdialog.h"
#include "ui_windowsparametersdialog.h"
#include <QLineEdit>
#include <QCheckBox>
#include <QDir>
#include <QMessageBox>
#include <mydoublevalidator.h>
#include <attributewindow.h>
#include <QTableWidgetItem>
#include <QMenu>
//#include <QSignalMapper>
//#include <QP
WindowsParametersDialog::WindowsParametersDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WindowsParametersDialog)
{
    ui->setupUi(this);
    QStringList columnNames;
    columnNames << "Мин. Удаление"<<"Макс. Удаление"<<"Мин. Время"<<"Макс. Время"<<"Cр. Ампл" << "Ср. кв. Ампл" << "Частота" <<"Спектр"<<"Энергия спектра" << "Ширина Спектра"<<"Мин. Амплитуда"<<"Мин Ср. Кв. Ампл."<<"Мин. Частота"<<"Мин. ширина спектра"<<"Мин. энергия спектра";
    //mapper = new QSignalMapper(this);
    ui->windowsTableWidget->setColumnCount(15);
    ui->windowsTableWidget->setRowCount(0);
    ui->windowsTableWidget->setHorizontalHeaderLabels(columnNames);
    ui->windowsTableWidget->resizeColumnsToContents();
    ui->windowsTableWidget->setTabKeyNavigation(false);
    this->setMaximumWidth(1850);
    settings = new QSettings(QCoreApplication::applicationDirPath()+QDir::separator()+"config.ini",QSettings::IniFormat,this);
    ui->minRatioLineEdit->setValidator(new QIntValidator(0,9999,ui->minRatioLineEdit));

    ui->relationsTableWidget->setColumnCount(3);
    columnNames.clear();
    columnNames<<"Амплитуда 1"<<"Амплитуда 2"<<"Мин Соотношение";
    ui->relationsTableWidget->setHorizontalHeaderLabels(columnNames);
    ui->relationsTableWidget->resizeColumnsToContents();
    ui->relationsTableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->relationsTableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->relationsTableWidget,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(displayMenu(QPoint)));
    connect(ui->minFreqSpinBox,SIGNAL(valueChanged(int)),this,SLOT(minFrqValueChanged(int)));
    connect(ui->maxFreqSpinBox,SIGNAL(valueChanged(int)),this,SLOT(maxFrqValueChanged(int)));
    readSettings();
    //connect(ui->windowsTableWidget,SIGNAL(cellClicked(int,int)),this,SLOT(windowsTableWidgetItemChanged(int,int)));
    //addRow();
    //addRow();
    /*int i=0;
    for ( ;i<4;i++)
    {
        QLineEdit *lEdit=new QLineEdit;
        lEdit->setValidator(new QIntValidator(0,500,lEdit));
        lEdit->setFrame(false);
        ui->windowsTableWidget->setCellWidget(0,i,lEdit);
    }
    for (;i<10;i++)
    {
        QCheckBox *pCheckBox = new QCheckBox();
        ui->windowsTableWidget->setCellWidget(0,i,pCheckBox);
        ui->windowsTableWidget->cellWidget(0,i)->setStyleSheet("margin-left:50%;margin-right:50%");
    }
    for ( ;i<15;i++)
    {
        QLineEdit *lEdit=new QLineEdit;
        lEdit->setValidator(new QIntValidator(0,500,lEdit));
        lEdit->setFrame(false);
        ui->windowsTableWidget->setCellWidget(0,i,lEdit);
    }
    ui->windowsTableWidget->resizeColumnsToContents();*/
}

WindowsParametersDialog::~WindowsParametersDialog()
{
    delete ui;
}

void WindowsParametersDialog::addRow()
{
    ui->windowsTableWidget->setRowCount(ui->windowsTableWidget->rowCount()+1);
    int i=0;
    QLineEdit *lEdit;
    QCheckBox *pCheckBox;
    for ( ;i<4;i++)
    {
        lEdit=new QLineEdit;
        lEdit->setValidator(new QIntValidator(0,9999,lEdit));
        lEdit->setFrame(false);
        ui->windowsTableWidget->setCellWidget(ui->windowsTableWidget->rowCount()-1,i,lEdit);
        lEdit->setAlignment(Qt::AlignHCenter);
        //ui->windowsTableWidget->cellWidget(ui->windowsTableWidget->rowCount()-1,i)->setStyleSheet("margin-left:50%;margin-right:50%");
    }

    pCheckBox = new QCheckBox();
    //pCheckBox->set
    ui->windowsTableWidget->setCellWidget(ui->windowsTableWidget->rowCount()-1,4,pCheckBox);
    ui->windowsTableWidget->cellWidget(ui->windowsTableWidget->rowCount()-1,4)->setStyleSheet("margin-left:50%;margin-right:50%");
    connect(pCheckBox,SIGNAL(stateChanged(int)),this,SLOT(countAmplCheckBoxStateChanged(int)));
    //connect(pCheckBox,SIGNAL())
    //connect(pCheckBox,SIGNAL(stateChanged(int)),mapper,SLOT(map()));
    //mapper->setMapping(pCheckBox,QString("A%1").arg(ui->windowsTableWidget->rowCount()));
    pCheckBox = new QCheckBox();
    ui->windowsTableWidget->setCellWidget(ui->windowsTableWidget->rowCount()-1,5,pCheckBox);
    ui->windowsTableWidget->cellWidget(ui->windowsTableWidget->rowCount()-1,5)->setStyleSheet("margin-left:50%;margin-right:50%");
    connect(pCheckBox,SIGNAL(stateChanged(int)),this,SLOT(countAmplCheckBoxStateChanged(int)));
    //connect(pCheckBox,SIGNAL(stateChanged(int)),mapper,SLOT(map()));
    //mapper->setMapping(pCheckBox,QString("R%1").arg(ui->windowsTableWidget->rowCount()));
    for (i=6;i<10;i++)
    {
        pCheckBox = new QCheckBox();
        ui->windowsTableWidget->setCellWidget(ui->windowsTableWidget->rowCount()-1,i,pCheckBox);
        ui->windowsTableWidget->cellWidget(ui->windowsTableWidget->rowCount()-1,i)->setStyleSheet("margin-left:50%;margin-right:50%");
        //connect(pCheckBox,SIGNAL(stateChanged(int)),this,SLOT(countAmplCheckBoxStateChanged(int)));
    }
    for ( ;i<15;i++)
    {
        QLineEdit *lEdit=new QLineEdit;
        lEdit->setValidator(new MyDoubleValidator(0,999,5,lEdit));
        lEdit->setFrame(false);
        ui->windowsTableWidget->setCellWidget(ui->windowsTableWidget->rowCount()-1,i,lEdit);
        lEdit->setAlignment(Qt::AlignHCenter);
        //ui->windowsTableWidget->cellWidget(ui->windowsTableWidget->rowCount()-1,i)->setStyleSheet("margin-left:50%;margin-right:50%");
    }
    ui->windowsTableWidget->resizeColumnsToContents();
}
//сохраняем параметры
void WindowsParametersDialog::saveSettings()
{
    //QVector<AttributeWindow*> windows;
    //AttributeWindow *windowAttr;
    int minOffset,maxOffset,minTime,maxTime;
    QLineEdit *pLineE;
    QCheckBox *pCheckB;
    for (int i=0; i<ui->windowsTableWidget->rowCount();++i)
    {
        //windowAttr = new AttributeWindow();
        pLineE=(qobject_cast<QLineEdit*>(ui->windowsTableWidget->cellWidget(i,0)));
        minOffset= pLineE->text().toInt();
        pLineE=(qobject_cast<QLineEdit*>(ui->windowsTableWidget->cellWidget(i,1)));
        maxOffset=pLineE->text().toInt();
        pLineE=(qobject_cast<QLineEdit*>(ui->windowsTableWidget->cellWidget(i,2)));
        minTime = pLineE->text().toInt();
        pLineE=(qobject_cast<QLineEdit*>(ui->windowsTableWidget->cellWidget(i,3)));
        maxTime=pLineE->text().toInt();
        if (maxOffset<=minOffset || maxTime<=minTime)
        {
            QMessageBox::warning(0, "Параметры окна", QString("Задайте корректные параметры окна # %1").arg(i+1));
            return;
        }
    }

    settings->beginGroup("/WindowsSettings");
    settings->setValue("/Width",this->width());
    settings->setValue("/Height",this->height());
    settings->setValue("/NbWindows",ui->nbWindowsSpinBox->value());
    settings->setValue("/MutedTraces",ui->mutedCheckBox->isChecked());
    settings->setValue("/CheckTests",ui->checkTestsCheckBox->isChecked());
    settings->setValue("/MinAmplValue",ui->minAmplDoubleSpinBox->value());
    //settings->setValue("/Relations",ui->relationsLineEdit->text());
    settings->beginWriteArray("/Windows");
    for(int j =0; j<ui->windowsTableWidget->rowCount();++j)
    {
        settings->setArrayIndex(j);
        pLineE=(qobject_cast<QLineEdit*>(ui->windowsTableWidget->cellWidget(j,0)));
        settings->setValue("/MinOffset",pLineE->text());
        pLineE=(qobject_cast<QLineEdit*>(ui->windowsTableWidget->cellWidget(j,1)));
        settings->setValue("/MaxOffset",pLineE->text());
        pLineE=(qobject_cast<QLineEdit*>(ui->windowsTableWidget->cellWidget(j,2)));
        settings->setValue("/MinTime",pLineE->text());
        pLineE=(qobject_cast<QLineEdit*>(ui->windowsTableWidget->cellWidget(j,3)));
        settings->setValue("/MaxTime",pLineE->text());
        pLineE=(qobject_cast<QLineEdit*>(ui->windowsTableWidget->cellWidget(j,10)));
        settings->setValue("/MinAmpl",pLineE->text().replace(locale().decimalPoint(),'.'));
        pLineE=(qobject_cast<QLineEdit*>(ui->windowsTableWidget->cellWidget(j,11)));
        settings->setValue("/MinRms",pLineE->text().replace(locale().decimalPoint(),'.'));
        pLineE=(qobject_cast<QLineEdit*>(ui->windowsTableWidget->cellWidget(j,12)));
        settings->setValue("/MinFreq",pLineE->text().replace(locale().decimalPoint(),'.'));
        pLineE=(qobject_cast<QLineEdit*>(ui->windowsTableWidget->cellWidget(j,13)));
        settings->setValue("/MinDfr",pLineE->text().replace(locale().decimalPoint(),'.'));
        pLineE=(qobject_cast<QLineEdit*>(ui->windowsTableWidget->cellWidget(j,14)));
        settings->setValue("/MinEnergy",pLineE->text().replace(locale().decimalPoint(),'.'));
        pCheckB=(qobject_cast<QCheckBox*>(ui->windowsTableWidget->cellWidget(j,4)));
        settings->setValue("/CountAmpl",pCheckB->isChecked());
        pCheckB=(qobject_cast<QCheckBox*>(ui->windowsTableWidget->cellWidget(j,5)));
        settings->setValue("/CountRms",pCheckB->isChecked());
        pCheckB=(qobject_cast<QCheckBox*>(ui->windowsTableWidget->cellWidget(j,6)));
        settings->setValue("/CountFreq",pCheckB->isChecked());
        pCheckB=(qobject_cast<QCheckBox*>(ui->windowsTableWidget->cellWidget(j,7)));
        settings->setValue("/WriteSpectrum",pCheckB->isChecked());
        //settings->setValue("/CountDfr",pCheckB->isChecked());
        pCheckB=(qobject_cast<QCheckBox*>(ui->windowsTableWidget->cellWidget(j,8)));
        settings->setValue("/CountDfr",pCheckB->isChecked());
        pCheckB=(qobject_cast<QCheckBox*>(ui->windowsTableWidget->cellWidget(j,9)));
        settings->setValue("/CountEnergy",pCheckB->isChecked());
    }

    settings->endArray();
    settings->beginWriteArray("/Relations");
    for (int j=0;j<ui->relationsTableWidget->rowCount();++j)
    {
        settings->setArrayIndex(j);
        settings->setValue("/A1",ui->relationsTableWidget->item(j,0)->text());
        settings->setValue("/A2",ui->relationsTableWidget->item(j,1)->text());
        settings->setValue("/ratio",ui->relationsTableWidget->item(j,2)->text());
    }
    settings->endArray();
    settings->setValue("/RmsSpectrum",ui->rmsSpectrumCheckBox->isChecked());
    settings->setValue("/EnergyMinFreq",ui->minFreqSpinBox->value());
    settings->setValue("/EnergyMaxFreq",ui->maxFreqSpinBox->value());
    settings->setValue("/WidthEnergy",ui->energyRadioButton->isChecked());
    settings->setValue("/FreqLevel",ui->frqLvlSpinBox->value());

    settings->endGroup();
    accept();
}
void WindowsParametersDialog::readSettings()
{
    settings->beginGroup("/WindowsSettings");
    this->resize(settings->value("/Width",1000).toInt(),settings->value("/Height",300).toInt());
    ui->mutedCheckBox->setChecked(settings->value( "/MutedTraces",true).toBool());
    ui->checkTestsCheckBox->setChecked(settings->value("/CheckTests",false).toBool());
    ui->minAmplDoubleSpinBox->setValue(settings->value("/MinAmplValue",0.0).toDouble());
    ui->nbWindowsSpinBox->setValue(settings->value("/NbWindows",0).toInt());
    //ui->relationsLineEdit->setText(settings->value("/Relations","").toString());
    int size = settings->beginReadArray("/Windows");
    for (int i=0; i<size; ++i)
    {
        settings->setArrayIndex(i);
        QLineEdit *pLineE(qobject_cast<QLineEdit*>(ui->windowsTableWidget->cellWidget(i,0)));
        pLineE->setText(settings->value("/MinOffset","0").toString());
        pLineE=(qobject_cast<QLineEdit*>(ui->windowsTableWidget->cellWidget(i,1)));
        pLineE->setText(settings->value("/MaxOffset","0").toString());
        pLineE=(qobject_cast<QLineEdit*>(ui->windowsTableWidget->cellWidget(i,2)));
        pLineE->setText(settings->value("/MinTime","0").toString());
        pLineE=(qobject_cast<QLineEdit*>(ui->windowsTableWidget->cellWidget(i,3)));
        pLineE->setText(settings->value("/MaxTime","0").toString());
        pLineE=(qobject_cast<QLineEdit*>(ui->windowsTableWidget->cellWidget(i,10)));
        pLineE->setText(settings->value("/MinAmpl","0").toString().replace('.',locale().decimalPoint()));
        pLineE=(qobject_cast<QLineEdit*>(ui->windowsTableWidget->cellWidget(i,11)));
        pLineE->setText(settings->value("/MinRms","0").toString().replace('.',locale().decimalPoint()));
        pLineE=(qobject_cast<QLineEdit*>(ui->windowsTableWidget->cellWidget(i,12)));
        pLineE->setText(settings->value("/MinFreq","0").toString().replace('.',locale().decimalPoint()));
        pLineE=(qobject_cast<QLineEdit*>(ui->windowsTableWidget->cellWidget(i,13)));
        pLineE->setText(settings->value("/MinDfr","0").toString().replace('.',locale().decimalPoint()));
        pLineE=(qobject_cast<QLineEdit*>(ui->windowsTableWidget->cellWidget(i,14)));
        pLineE->setText(settings->value("/MinEnergy","0").toString().replace('.',locale().decimalPoint()));
        QCheckBox *pCheckB(qobject_cast<QCheckBox*>(ui->windowsTableWidget->cellWidget(i,4)));
        pCheckB->setChecked(settings->value("/CountAmpl","0").toBool());
        pCheckB=(qobject_cast<QCheckBox*>(ui->windowsTableWidget->cellWidget(i,5)));
        pCheckB->setChecked(settings->value("/CountRms","0").toBool());
        pCheckB=(qobject_cast<QCheckBox*>(ui->windowsTableWidget->cellWidget(i,6)));
        pCheckB->setChecked(settings->value("/CountFreq","0").toBool());
        pCheckB=(qobject_cast<QCheckBox*>(ui->windowsTableWidget->cellWidget(i,7)));
        pCheckB->setChecked(settings->value("/WriteSpectrum","0").toBool());
        pCheckB=(qobject_cast<QCheckBox*>(ui->windowsTableWidget->cellWidget(i,8)));
        pCheckB->setChecked(settings->value("/CountEnergy","0").toBool());
        pCheckB=(qobject_cast<QCheckBox*>(ui->windowsTableWidget->cellWidget(i,9)));
        pCheckB->setChecked(settings->value("/CountDfr","0").toBool());
    }

    settings->endArray();
    size=settings->beginReadArray("/Relations");
    for (int i=0;i<size;++i)
    {
        addRatioRow();
        settings->setArrayIndex(i);
        ui->relationsTableWidget->item(i,0)->setText(settings->value("/A1","A1").toString());
        ui->relationsTableWidget->item(i,1)->setText(settings->value("/A2","A1").toString());
        ui->relationsTableWidget->item(i,2)->setText(settings->value("/ratio","0").toString());
    }
    settings->endArray();
    ui->rmsSpectrumCheckBox->setChecked(settings->value("/RmsSpectrum",false).toBool());
    ui->minFreqSpinBox->setValue(settings->value("/EnergyMinFreq",1).toUInt());
    ui->maxFreqSpinBox->setValue(settings->value("/EnergyMaxFreq",125).toUInt());
    ui->energyRadioButton->setChecked(settings->value("/WidthEnergy",false).toBool());
    ui->frqLvlSpinBox->setValue(settings->value("/FreqLevel",15).toUInt());
    ui->maxFreqRadioButton->setChecked(!ui->energyRadioButton->isChecked());
    ui->frqLvlSpinBox->setEnabled(!ui->energyRadioButton->isChecked());
    settings->endGroup();
}
void WindowsParametersDialog::nbWindowsSpinBoxValueChanged(int val)
{
    if (val < ui->windowsTableWidget->rowCount())
    {
        ui->windowsTableWidget->setRowCount(val);
    }
    else
    {
        int i = ui->windowsTableWidget->rowCount();
        for ( ; i<val;++i)
        {
            addRow();
        }
    }
}
void WindowsParametersDialog::countAmplCheckBoxStateChanged(int state)
{
    Q_UNUSED(state);
    ui->ratio1ComboBox->clear();
    ui->ratio2comboBox->clear();
    QStringList ampls;
    QCheckBox *pCheckB;
    for (int i=0;i<ui->windowsTableWidget->rowCount();++i)
    {
        pCheckB = qobject_cast<QCheckBox*>(ui->windowsTableWidget->cellWidget(i,4));
        if (pCheckB->isChecked())
        {
            ampls.append(QString("A%1").arg(i+1));
        }
        pCheckB = qobject_cast<QCheckBox*>(ui->windowsTableWidget->cellWidget(i,5));
        if (pCheckB->isChecked())
        {
            ampls.append(QString("R%1").arg(i+1));
        }
    }

    ui->ratio1ComboBox->addItems(ampls);
    ui->ratio2comboBox->addItems(ampls);

}
void WindowsParametersDialog::addRatioRow()
{
    ui->relationsTableWidget->setRowCount(ui->relationsTableWidget->rowCount()+1);
    for (int i =0; i<3; ++i )
    {
        QTableWidgetItem *item = new QTableWidgetItem();
        item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        item->setTextAlignment(Qt::AlignCenter);
        ui->relationsTableWidget->setItem(ui->relationsTableWidget->rowCount()-1,i,item);
    }
}
void WindowsParametersDialog::addRatioSlot()
{
    /*ui->relationsTableWidget->setRowCount(ui->relationsTableWidget->rowCount()+1);
    for (int i =0; i<3; ++i )
    {
        QTableWidgetItem *item = new QTableWidgetItem();
        item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        ui->relationsTableWidget->setItem(ui->relationsTableWidget->rowCount()-1,i,item);
    }*/

    QStringList currentRelations;

    for (int i=0; i<ui->relationsTableWidget->rowCount();++i)
    {
        currentRelations.append(ui->relationsTableWidget->item(i,0)->text()+ui->relationsTableWidget->item(i,1)->text());
    }
    QString tmp = ui->ratio1ComboBox->currentText()+ui->ratio2comboBox->currentText();
    if (currentRelations.indexOf(tmp)==-1)
    {
        addRatioRow();

        ui->relationsTableWidget->item(ui->relationsTableWidget->rowCount()-1,0)->setText(ui->ratio1ComboBox->currentText());
        ui->relationsTableWidget->item(ui->relationsTableWidget->rowCount()-1,1)->setText(ui->ratio2comboBox->currentText());
        if (ui->minRatioLineEdit->text().isEmpty())
        {
            ui->relationsTableWidget->item(ui->relationsTableWidget->rowCount()-1,2)->setText("0");
        }
        else
        {
            ui->relationsTableWidget->item(ui->relationsTableWidget->rowCount()-1,2)->setText(ui->minRatioLineEdit->text());
        }
    }
    else
    {
        QMessageBox::warning(0, "Cоотношения", QString("Указанное соотношение уже есть в списке"));
    }
}


void WindowsParametersDialog::displayMenu(QPoint pos)
{
    QMenu menu(this);
    QAction *act=menu.addAction("Удалить");
    //QAction *paste = menu.addAction("Вставить глубины");
    QAction *a=menu.exec(ui->relationsTableWidget->viewport()->mapToGlobal(pos));
    if (act==a)
    {
        //int ffid = ui->segyBreaksTableWidget->item(ui->segyBreaksTableWidget->currentRow(),0)->text().toInt();
        ui->relationsTableWidget->removeRow(ui->relationsTableWidget->currentRow());
        /*QString itemText;

        for (int i=0; i<ui->segyListWidget->count(); ++i)
        {
            itemText = ui->segyListWidget->item(i)->text();
            if (itemText.left(itemText.indexOf('.')).toInt()==ffid)
            {
                ui->segyListWidget->takeItem(i);
                break;
            }
        }*/
        //QList<QListWidgetItem*> listItem = ui->segyBreaksTableWidget->
        //ui->segyListWidget-> ffid.leftJustified()
        //this->setFixedSize(ui->segyBreaksTableWidget->horizontalHeader()->length()+125,ui->segyBreaksTableWidget->verticalHeader()->length()+75);
    }
    /*if (paste == a)
    {
        QString str = qApp->clipboard()->text();
        QStringList strLst = str.split("\n");
        QRegExp reg("(\\s)");
        for (int i =0; i<ui->segyBreaksTableWidget->rowCount() && i<strLst.count();i++ )
        {
            QStringList strLst1 = strLst[i].split(reg);
            ui->segyBreaksTableWidget->item(i,1)->setText(strLst1[0]);
        }

    }*/
}

void WindowsParametersDialog::minFrqValueChanged(int val)
{
    ui->maxFreqSpinBox->setMinimum(val+10);
}
void WindowsParametersDialog::maxFrqValueChanged(int val)
{
    ui->minFreqSpinBox->setMaximum(val-10);
}
