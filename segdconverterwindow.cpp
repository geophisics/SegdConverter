#include "segdconverterwindow.h"
#include "ui_segdconverterwindow.h"
#include "convertparametersdialog.h"
#include "binningparametersdialog.h"
#include "windowsparametersdialog.h"
#include "tableviewdialog.h"
#include <QDir>
#include <QFileDialog>
#include <Segd/segdfile.h>
#include <Cst/cstfile.h>
#include <Segy/segyfile.h>
#include <QHeaderView>
#include "Workers/segyworkeronline.h"
#include "Workers/cstworkeronline.h"
#include "QMessageBox"
#include "auxesdialog.h"
#include "auxviewdialog.h"
#include "SUB/general.h"


SegdConverterWindow::SegdConverterWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SegdConverterWindow)
{
    ui->setupUi(this);
    settings = new QSettings(QCoreApplication::applicationDirPath()+QDir::separator()+"config.ini",QSettings::IniFormat,this);
    ui->actionStop->setEnabled(false);
    readSettings();
    readConvertParamsSettings();
//    qRegisterMetaType<FfidData>();
//    qRegisterMetaType<SeisAttributes>();
    attr_model=new AttributesModel(this);

    ui->attributesTableView->setModel(attr_model);
   // ui->attributesTableView->horizontalHeader()->setVisible(true);
   // ui->attributesTableView->verticalHeader()->setVisible(true);
    attr_model->setHeaders();
    //ui->attributesTableView->horizontalHeader()->setSectionsMovable(true);
    connect(ui->actionOpenSegd,SIGNAL(triggered(bool)),this,SLOT(openDataSlot()));
    connect(ui->actionSaveSeisData,SIGNAL(triggered(bool)),this,SLOT(saveDataSlot()));
    connect(ui->actionSaveAttributes,SIGNAL(triggered(bool)),this,SLOT(saveAttributesFileSlot()));
    connect(ui->actionAttributeTable,SIGNAL(triggered(bool)),this,SLOT(openTableViewParametersDialog()));
    connect(ui->actionOpenRPS,SIGNAL(triggered(bool)),this,SLOT(selectRpsFile(bool)));
    connect(ui->actionOpenSPS,SIGNAL(triggered(bool)),this,SLOT(selectSpsFile(bool)));
    connect(ui->actionOpenXPS,SIGNAL(triggered(bool)),this,SLOT(selectXpsFile(bool)));
    connect(ui->actionRun,SIGNAL(triggered(bool)),this,SLOT(runActionSlot()));
    connect(ui->actionAboutQt,SIGNAL(triggered(bool)),this,SLOT(aboutQtSlot()));
    connect(ui->actionAbout,SIGNAL(triggered(bool)),this,SLOT(aboutSlot()));
    connect(ui->actionAuxes,SIGNAL(triggered(bool)),this,SLOT(openAuxParametersDialog()));
}

SegdConverterWindow::~SegdConverterWindow()
{
    saveSettings();
    delete ui;
}

//вызываем диалог параметров конвертации
void SegdConverterWindow::openConvertParametersDialog()
{
    ConvertParametersDialog *dialog = new ConvertParametersDialog();
    int dialogResult=dialog->exec();
    if (dialogResult==1)
    {
        readConvertParamsSettings();
        attr_model->setHeaders();
    }
}
//вызываем диалог параметров бинирования
void SegdConverterWindow::openBinningParametersDialog()
{
    BinningParametersDialog *dialog = new BinningParametersDialog();
    dialog->exec();
}
//вызываем диалог параметров окон атрибутов
void SegdConverterWindow::openWindowsParametersDialog()
{
    WindowsParametersDialog *dialog = new WindowsParametersDialog();
    int dialogResult = dialog->exec();
    if (dialogResult==1)
    {
        attr_model->setHeaders();
    }
}
void SegdConverterWindow::openTableViewParametersDialog()
{
    TableViewDialog *dialog = new TableViewDialog();
    connect(dialog,SIGNAL(sendTableColumns(QStringList)),this,SLOT(setColumnsForView(QStringList)));
    dialog->exec();
}
//загружаем настройки
void SegdConverterWindow::readSettings()
{
    settings->beginGroup("/MainSettings");
    ui->segdLineEdit->setText(settings->value("/SegdPath","").toString());
    ui->outFileLineEdit->setText(settings->value("/OutFile","").toString());
    ui->attrFileLineEdit->setText(settings->value("AttrPath","").toString());
    WorkDir = settings->value("/WorkDir","").toString();
    QByteArray MyArray = settings->value("/TableState","").toByteArray();
    settings->endGroup();
    settings->beginGroup("/ViewSettings");
    int size = settings->beginReadArray("/ColumnForView");
    //ui->attributesTableView->setColumnCount(size);
    int i=0;
    for ( ;i<size;i++)
    {
        settings->setArrayIndex(i);
        AttributeColumns<<settings->value("/ColumnForView","").toString();
    }
    settings->endArray();


    settings->endGroup();
    settings->beginGroup("/ConvertSettings");
    online = settings->value("/OnLine",false).toBool();
    settings->endGroup();
    //ui->attributesTableView->setHorizontalHeaderLabels(AttributeColumns);
    ui->attributesTableView->horizontalHeader()->restoreState(MyArray);
    //ui->attributesTableView->setHorizontalHeader();
}

//сохраняем настройки
void SegdConverterWindow::saveSettings()
{
    settings->beginGroup("/MainSettings");
    settings->setValue("/SegdPath",ui->segdLineEdit->text());
    settings->setValue("/OutFile",ui->outFileLineEdit->text());
    settings->setValue("/AttrPath",ui->attrFileLineEdit->text());
    settings->setValue("/WorkDir",WorkDir);
    /*settings->beginWriteArray("/columns");
    for (int i=0; i< ui->attributesTableView->columnCount();++i)
    {
        settings->setArrayIndex(i);
        settings->setValue("/columnNb",ui->attributesTableView->visualColumn(i));
    }
    settings->endArray();*/
    QByteArray MyArray = ui->attributesTableView->horizontalHeader()->saveState();
    settings->setValue("/TableState",MyArray);
    //ui->attributesTableView->columnViewportPosition()
    settings->endGroup();

}

//читаем настройки конвертирования
void SegdConverterWindow::readConvertParamsSettings()
{
    settings->beginGroup("/ConvertSettings");
    bool b = settings->value("/ReadMode",1).toBool();
    if (b)
    {
        ui->segdLabel->setText("Файл Segd");
        ui->actionOpenSegd->setText("Открыть файл SEGD");
    }
    else
    {
        ui->segdLabel->setText("Директория Segd");
        ui->actionOpenSegd->setText("Открыть директорию SEGD");
    }
    b = settings->value("/CST","1").toBool();
    if (b)
    {
        ui->outFileLabel->setText("Файл CST");
        ui->actionSaveSeisData->setText("Выбрать файл СST");

    }
    else
    {
        ui->outFileLabel->setText("Файл SEGY");
        ui->actionSaveSeisData->setText("Выбрать файл SEGY");
    }
    online = settings->value("/OnLine",false).toBool();
    if (online)
    {
       ui->actionOpenXPS->setChecked(false);
    }
    ui->actionOpenXPS->setEnabled(!online);
    ui->actionAuxes->setEnabled(settings->value("/AnalisysAuxes",false).toBool());

    settings->endGroup();
}

//выбрать sps файл

void SegdConverterWindow::selectSpsFile(bool checked)
{
    if (checked)
    {
        QString str;
        str = QFileDialog::getOpenFileName(this,"Выберите файл *.sps", WorkDir,"SPS files (*.sps)");
        if (!str.isEmpty())
        {
            spsFile = str;
        }
        else
        {
            ui->actionOpenSPS->setChecked(false);
        }
    }
}

// выбрать rps файл

void SegdConverterWindow::selectRpsFile(bool checked)
{
    if (checked)
    {
        QString str;
        str = QFileDialog::getOpenFileName(this,"Выберите файл *.rps", WorkDir,"RPS files (*.rps)");
        if (!str.isEmpty())
        {
            rpsFile = str;
        }
        else
        {
            ui->actionOpenRPS->setChecked(false);
        }
    }
}

void SegdConverterWindow::selectXpsFile(bool checked)
{
    if (checked)
    {
        QString str;
        str = QFileDialog::getOpenFileName(this,"Выберите файл *.xps", WorkDir,"XPS files (*.xps)");
        if (!str.isEmpty())
        {
            xpsFile = str;
        }
        else
        {
            ui->actionOpenXPS->setChecked(false);
        }
    }
}

//открываем папку/файл с данными для конвертирования
void SegdConverterWindow::openDataSlot()
{
    //QString str=ui->segdDirLineEdit->text();
    QString str=ui->segdLineEdit->text();
    if (ui->segdLabel->text()=="Директория Segd")
    {
        if (str.isEmpty())
        {
            str = QFileDialog::getExistingDirectory(this,"Выберите директорию с *.segd файлами",WorkDir,QFileDialog::DontResolveSymlinks);
        }
        else
        {
            //str=str.left(str.lastIndexOf(QDir::separator()));
            str = QFileDialog::getExistingDirectory(this,"Выберите директорию с *.segd файлами",str,QFileDialog::DontResolveSymlinks);
        }

    }
    else
    {
        if (str.isEmpty())
        {
            str = QFileDialog::getOpenFileName(this,"Выберите файл *.segd",WorkDir,"SEGD files(*.segd *.SEGD)");
        }
        else
        {
            str = QFileDialog::getOpenFileName(this,"Выберите файл *.segd",str,"SEGD files(*.segd *.SEGD)");
        }

    }
    if (!str.isEmpty())
    {
        ui->segdLineEdit->setText(str);
        WorkDir = str;
    }
}
//выбираем файл, в который будет сохранен результат конвертации
void SegdConverterWindow::saveDataSlot()
{
    QString str=ui->outFileLineEdit->text();
    if (ui->outFileLabel->text()=="Файл CST")
    {
        if (str.isEmpty())
        {
            str = QFileDialog::getSaveFileName(this,"Выберите файл *.cst для сохранения",WorkDir,"CST files(*.cst *.CST)");
        }
        else
        {
            str = QFileDialog::getSaveFileName(this,"Выберите файл *.cst для сохранения",str,"SEGD files(*.cst *.CST)");
        }
    }
    else
    {
        if (str.isEmpty())
        {
            str = QFileDialog::getSaveFileName(this,"Выберите файл *.sgy для сохранения",WorkDir,"SEGY files(*.segy *.SEGY *.sgy *.SGY)");
        }
        else
        {
            str = QFileDialog::getSaveFileName(this,"Выберите файл *.segy для сохранения",str,"SEGY files(*.segy *.SEGY *.sgy *.SGY)");
        }
    }
    if (!str.isEmpty())
    {
        ui->outFileLineEdit->setText(str);
        WorkDir = str;
    }
}
//выбираем файл для атрибутов
void SegdConverterWindow::saveAttributesFileSlot()
{
    QString str=ui->attrFileLineEdit->text();
    if (str.isEmpty())
    {
        str = QFileDialog::getSaveFileName(this,"Выберите файл *.xlsx для сохранения",WorkDir,"Excel files(*.xlsx *.XLSX)");
    }
    else
    {
        str = QFileDialog::getSaveFileName(this,"Выберите файл *.xlsx для сохранения",str,"Excel files(*.xlsx *.XLSX)");
    }

    if (!str.isEmpty())
    {
        ui->attrFileLineEdit->setText(str);
        WorkDir = str;
    }
}

void SegdConverterWindow::runActionSlot()
{

    ui->logTextEdit->clear();
    p_myThread = new QThread();
    ui->actionRun->setEnabled(false);
    ui->actionExit->setEnabled(false);
    ui->actionStop->setEnabled(true);

    if (ui->outFileLabel->text()=="Файл CST")
    {
        if (online)
        {
            runCstOnline();
        }
        else
        {
            runCst();
        }
    }
    else
    {
        if (online)
        {
            runSegyOnline();
        }
        else
        {
            runSegy();
        }
    }
}
void SegdConverterWindow::disableStop(bool disable)
{
    Q_UNUSED(disable);
    ui->actionStop->setEnabled(false);
}

void SegdConverterWindow::convertingEnded()
{
     ui->logTextEdit->append("Конвертация завершена");
     attr_model->saveDataInXlsx(ui->attrFileLineEdit->text());
     ui->actionRun->setEnabled(true);
     ui->actionExit->setEnabled(true);
     ui->actionStop->setEnabled(false);
}

void SegdConverterWindow::receiveSomeError(const QString &err)
{
    ui->logTextEdit->setTextColor(Qt::red);
    ui->logTextEdit->append(err);
    ui->logTextEdit->setTextColor(Qt::black);
}
void SegdConverterWindow::recieveInfoMessage(const QString &message, const QColor &color)
{
    ui->logTextEdit->setTextColor(color);
    ui->logTextEdit->append(message);
    ui->logTextEdit->setTextColor(Qt::black);
}
/*void SegdConverterWindow::setColumnsForView(const QStringList &columns)
{
    for (int i=0; i< ui->attributesTableView->columnCount();++i)
    {
        ui->attributesTableView->horizontalHeader()->moveSection(ui->attributesTableView->visualColumn(i),i);
    }
    for (int i=0; i< ui->attributesTableView->columnCount();++i)
    {
        if (columns.indexOf(ui->attributesTableView->horizontalHeaderItem(i)->text())==-1)
        {
            ui->attributesTableView->removeColumn(i);
            i--;
        }
    }
    for (int i =0; i<columns.count(); ++i )
    {
        if (AttributeColumns.indexOf(columns.value(i))==-1)
        {
            ui->attributesTableView->insertColumn(i);
        }
    }

    ui->attributesTableView->setHorizontalHeaderLabels(columns);
    AttributeColumns.clear();
    AttributeColumns = columns;
}*/


//конвертация в файл segy
void SegdConverterWindow::runSegy()
{
    //SegyWorker *p_segyWorker = new SegyWorker();
    SegyWorker *p_segyWorker = new SegyWorker(attr_model->getAttributes());
    connect(p_segyWorker,SIGNAL(sendSomeError(QString)),this,SLOT(receiveSomeError(QString)));
    p_segyWorker->readSettings();
    p_segyWorker->setSegdPath(ui->segdLineEdit->text());
    p_segyWorker->setOutPath(ui->outFileLineEdit->text());
    p_segyWorker->setAttrFilePath(ui->attrFileLineEdit->text());
    p_segyWorker->setMode(ui->segdLabel->text()=="Директория Segd");
    p_segyWorker->moveToThread(p_myThread);
    if (ui->actionOpenRPS->isChecked())
    {
        if (p_segyWorker->readRps(rpsFile))
        {
            p_segyWorker->setUseExternalRps(true);
        }
        else
        {
            p_segyWorker->setUseExternalRps(false);
        }
    }
    if (ui->actionOpenSPS->isChecked())
    {
        if (p_segyWorker->readSps(spsFile))
        {
            p_segyWorker->setUseExternalSps(true);
        }
        else
        {
            p_segyWorker->setUseExternalSps(false);
        }
    }
    if (ui->actionOpenXPS->isChecked())
    {
        p_segyWorker->setUseExternalXps(true);
        p_segyWorker->setXpsPath(xpsFile);
    }
    else
    {
        p_segyWorker->setUseExternalXps(false);
    }
    if (ui->actionAuxes->isEnabled())
    {
        setViewAuxesDialog(p_segyWorker);
    }
    ui->logTextEdit->append("Начало конвертации.");
    connect(p_myThread,SIGNAL(started()),p_segyWorker,SLOT(Converting()));
    connect(p_segyWorker,SIGNAL(finished()),this,SLOT(convertingEnded()));
    connect(p_segyWorker,SIGNAL(finished()),p_segyWorker,SLOT(deleteLater()));
    connect(p_segyWorker,SIGNAL(sendInfoMessage(QString,QColor)),this,SLOT(recieveInfoMessage(QString,QColor)));
    connect(p_segyWorker,SIGNAL(attributesCounted()),attr_model,SIGNAL(layoutChanged()));
    connect(p_myThread,SIGNAL(finished()),p_myThread,SLOT(deleteLater()));
    connect(ui->actionStop,SIGNAL(triggered(bool)),p_segyWorker,SLOT(stopRunning()),Qt::DirectConnection);
    connect(ui->actionStop,SIGNAL(triggered(bool)),this,SLOT(disableStop(bool)));
    //connect(ui->actionStop,SIGNAL(triggered(bool)),ui->actionStop,SLOT(setVisible(false)));

    p_myThread->start();

}


//конвертация в файл segy в режиме онлайн
void SegdConverterWindow::runSegyOnline()
{
    SegyWorker *p_segyWorker = new SegyWorkerOnline(attr_model->getAttributes());
    connect(p_segyWorker,SIGNAL(sendSomeError(QString)),this,SLOT(receiveSomeError(QString)));
    p_segyWorker->readSettings();
    p_segyWorker->setSegdPath(ui->segdLineEdit->text());
    p_segyWorker->setOutPath(ui->outFileLineEdit->text());
    p_segyWorker->setAttrFilePath(ui->attrFileLineEdit->text());
    p_segyWorker->setMode(ui->segdLabel->text()=="Директория Segd");
    p_segyWorker->moveToThread(p_myThread);
    if (ui->actionOpenRPS->isChecked())
    {
        if (p_segyWorker->readRps(rpsFile))
        {
            p_segyWorker->setUseExternalRps(true);
        }
        else
        {
            p_segyWorker->setUseExternalRps(false);
        }
    }
    if (ui->actionOpenSPS->isChecked())
    {
        if (p_segyWorker->readSps(spsFile))
        {
            p_segyWorker->setUseExternalSps(true);
        }
        else
        {
            p_segyWorker->setUseExternalSps(false);
        }
    }
    p_segyWorker->setUseExternalXps(false);
    if (ui->actionAuxes->isEnabled())
    {
        setViewAuxesDialog(p_segyWorker);
    }
    ui->logTextEdit->append("Начало конвертации");
    connect(p_myThread,SIGNAL(started()),p_segyWorker,SLOT(Converting()));
    connect(p_segyWorker,SIGNAL(finished()),p_myThread,SLOT(quit()));
    connect(p_segyWorker,SIGNAL(finished()),this,SLOT(convertingEnded()));
    connect(p_segyWorker,SIGNAL(finished()),p_segyWorker,SLOT(deleteLater()));
    connect(p_segyWorker,SIGNAL(sendInfoMessage(QString,QColor)),this,SLOT(recieveInfoMessage(QString,QColor)));
    connect(p_segyWorker,SIGNAL(attributesCounted()),attr_model,SIGNAL(layoutChanged()));
    connect(p_myThread,SIGNAL(finished()),p_myThread,SLOT(deleteLater()));
    connect(ui->actionStop,SIGNAL(triggered(bool)),p_segyWorker,SLOT(stopRunning()),Qt::DirectConnection);
    connect(ui->actionStop,SIGNAL(triggered(bool)),this,SLOT(disableStop(bool)));
    p_myThread->start();

}
//конвертация в файл cst

void SegdConverterWindow::runCst()
{
    CstWorker *p_cstWorker = new CstWorker(attr_model->getAttributes());
    connect(p_cstWorker,SIGNAL(sendSomeError(QString)),this,SLOT(receiveSomeError(QString)));
    p_cstWorker->readSettings();
    p_cstWorker->setSegdPath(ui->segdLineEdit->text());
    p_cstWorker->setOutPath(ui->outFileLineEdit->text());
    p_cstWorker->setAttrFilePath(ui->attrFileLineEdit->text());
    p_cstWorker->setMode(ui->segdLabel->text()=="Директория Segd");
    p_cstWorker->moveToThread(p_myThread);
    if (ui->actionOpenRPS->isChecked())
    {
        if (p_cstWorker->readRps(rpsFile))
        {
            p_cstWorker->setUseExternalRps(true);
        }
        else
        {
            p_cstWorker->setUseExternalRps(false);
        }
    }
    if (ui->actionOpenSPS->isChecked())
    {
        if (p_cstWorker->readSps(spsFile))
        {
            p_cstWorker->setUseExternalSps(true);
        }
        else
        {
            p_cstWorker->setUseExternalSps(false);
        }
    }
    if (ui->actionOpenXPS->isChecked())
    {
        p_cstWorker->setUseExternalXps(true);
        p_cstWorker->setXpsPath(xpsFile);
    }
    else
    {
        p_cstWorker->setUseExternalXps(false);
    }
    if (ui->actionAuxes->isEnabled())
    {
        setViewAuxesDialog(p_cstWorker);
    }
    ui->logTextEdit->append("Начало конвертации");
    connect(p_myThread,SIGNAL(started()),p_cstWorker,SLOT(Converting()));
    connect(p_cstWorker,SIGNAL(finished()),p_myThread,SLOT(quit()));
    connect(p_cstWorker,SIGNAL(finished()),this,SLOT(convertingEnded()));
    connect(p_cstWorker,SIGNAL(finished()),p_cstWorker,SLOT(deleteLater()));
    connect(p_cstWorker,SIGNAL(sendInfoMessage(QString,QColor)),this,SLOT(recieveInfoMessage(QString,QColor)));
    connect(p_cstWorker,SIGNAL(attributesCounted()),attr_model,SIGNAL(layoutChanged()));
    connect(p_myThread,SIGNAL(finished()),p_myThread,SLOT(deleteLater()));
    connect(ui->actionStop,SIGNAL(triggered(bool)),p_cstWorker,SLOT(stopRunning()),Qt::DirectConnection);
    connect(ui->actionStop,SIGNAL(triggered(bool)),this,SLOT(disableStop(bool)));
    p_myThread->start();
}

void SegdConverterWindow::runCstOnline()
{
    CstWorkerOnline *p_cstWorker = new CstWorkerOnline(attr_model->getAttributes());
    connect(p_cstWorker,SIGNAL(sendSomeError(QString)),this,SLOT(receiveSomeError(QString)));
    p_cstWorker->readSettings();
    p_cstWorker->setSegdPath(ui->segdLineEdit->text());
    p_cstWorker->setOutPath(ui->outFileLineEdit->text());
    p_cstWorker->setAttrFilePath(ui->attrFileLineEdit->text());
    p_cstWorker->setMode(ui->segdLabel->text()=="Директория Segd");
    p_cstWorker->moveToThread(p_myThread);
    if (ui->actionOpenRPS->isChecked())
    {
        if (p_cstWorker->readRps(rpsFile))
        {
            p_cstWorker->setUseExternalRps(true);
        }
        else
        {
            p_cstWorker->setUseExternalRps(false);
        }
    }
    if (ui->actionOpenSPS->isChecked())
    {
        if (p_cstWorker->readSps(spsFile))
        {
            p_cstWorker->setUseExternalSps(true);
        }
        else
        {
            p_cstWorker->setUseExternalSps(false);
        }
    }
    p_cstWorker->setUseExternalXps(false);
    if (ui->actionAuxes->isEnabled())
    {
        setViewAuxesDialog(p_cstWorker);
    }
    ui->logTextEdit->append("Начало конвертации");
    connect(p_myThread,SIGNAL(started()),p_cstWorker,SLOT(Converting()));
    connect(p_cstWorker,SIGNAL(finished()),p_myThread,SLOT(quit()));
    connect(p_cstWorker,SIGNAL(finished()),this,SLOT(convertingEnded()));
    connect(p_cstWorker,SIGNAL(finished()),p_cstWorker,SLOT(deleteLater()));
    connect(p_cstWorker,SIGNAL(sendInfoMessage(QString,QColor)),this,SLOT(recieveInfoMessage(QString,QColor)));
    connect(p_cstWorker,SIGNAL(attributesCounted()),attr_model,SIGNAL(layoutChanged()));
    connect(p_myThread,SIGNAL(finished()),p_myThread,SLOT(deleteLater()));
    connect(ui->actionStop,SIGNAL(triggered(bool)),p_cstWorker,SLOT(stopRunning()),Qt::DirectConnection);
    connect(ui->actionStop,SIGNAL(triggered(bool)),this,SLOT(disableStop(bool)));
    p_myThread->start();
}

void SegdConverterWindow::setViewAuxesDialog(BaseWorker *worker)
{
    if (viewDialog.isNull())
    {
        viewDialog = new AuxViewDialog(this);
    }
    connect (worker,SIGNAL(sendVectors(QVector<QPointF>*,bool,QVector<QPointF>*,bool,int)),viewDialog,SLOT(receiveVectors(QVector<QPointF>*,bool,QVector<QPointF>*,bool,int)));
    connect (worker,SIGNAL(sendExplAuxes(QVector<QPointF>*,bool,QVector<QPointF>*,bool,QVector<QPointF>*,bool)),viewDialog,SLOT(receiveExplAuxes(QVector<QPointF>*,bool,QVector<QPointF>*,bool,QVector<QPointF>*,bool)));
    viewDialog.data()->show();

}

/*void SegdConverterWindow::receiveFfidDataSlot(const FfidData &data)
{
     ui->attributesTableView->setRowCount(ui->attributesTableView->rowCount()+1);
     ui->attributesTableView->scrollToBottom();
     //ui->attributesTableView->setCurrentIndex(QModelIndex());
     int columnNb;
     columnNb = AttributeColumns.indexOf("FFID");
     QTableWidgetItem *pItem;
     if (columnNb!=-1)
     {
         pItem = new QTableWidgetItem(QString::number(data.ffid));
         ui->attributesTableView->setItem(ui->attributesTableView->rowCount()-1,columnNb,pItem);
     }
     columnNb = AttributeColumns.indexOf("Source Line");
     if (columnNb!=-1)
     {
         pItem = new QTableWidgetItem(QString::number(data.line));
         ui->attributesTableView->setItem(ui->attributesTableView->rowCount()-1,columnNb,pItem);
     }
     columnNb = AttributeColumns.indexOf("Source Point");
     if (columnNb!=-1)
     {
         pItem = new QTableWidgetItem(QString::number(data.source));
         ui->attributesTableView->setItem(ui->attributesTableView->rowCount()-1,columnNb,pItem);
     }
     columnNb = AttributeColumns.indexOf("Source X");
     if (columnNb!=-1)
     {
         pItem = new QTableWidgetItem(QString::number(data.X));
         ui->attributesTableView->setItem(ui->attributesTableView->rowCount()-1,columnNb,pItem);
     }
     columnNb = AttributeColumns.indexOf("Source Y");
     if (columnNb!=-1)
     {
         pItem = new QTableWidgetItem(QString::number(data.Y));
         ui->attributesTableView->setItem(ui->attributesTableView->rowCount()-1,columnNb,pItem);
     }
     columnNb = AttributeColumns.indexOf("Source Z");
     if (columnNb!=-1)
     {
         pItem = new QTableWidgetItem(QString::number(data.Z));
         ui->attributesTableView->setItem(ui->attributesTableView->rowCount()-1,columnNb,pItem);
     }
}*/

//получаем атрибуты сейсмической записи

/*void SegdConverterWindow::receiveSeisAttributes(SeisAttributes *attr, const int &winNb)
{
    int columnNb;
    QTableWidgetItem *pItem;
    columnNb = AttributeColumns.indexOf(QString("Abs A#%1").arg(winNb));
    if (columnNb!=-1)
    {
        pItem = new QTableWidgetItem(QString::number(attr->ampl));
        if (!attr->correctAmpl)
        {
            pItem->setBackgroundColor(Qt::red);
        }
        ui->attributesTableView->setItem(ui->attributesTableView->rowCount()-1,columnNb,pItem);
    }
    columnNb = AttributeColumns.indexOf(QString("RMS #%1").arg(winNb));
    if (columnNb!=-1)
    {
        pItem = new QTableWidgetItem(QString::number(attr->rms));
        if (!attr->correctRms)
        {
            pItem->setBackgroundColor(Qt::red);
        }
        ui->attributesTableView->setItem(ui->attributesTableView->rowCount()-1,columnNb,pItem);
    }
    columnNb = AttributeColumns.indexOf(QString("Frq #%1").arg(winNb));
    if (columnNb!=-1)
    {
        pItem = new QTableWidgetItem(QString::number(attr->freq));
        if (!attr->correctFreq)
        {
            pItem->setBackgroundColor(Qt::red);
        }
        ui->attributesTableView->setItem(ui->attributesTableView->rowCount()-1,columnNb,pItem);
    }
    columnNb = AttributeColumns.indexOf(QString("Energy #%1").arg(winNb));
    if (columnNb!=-1)
    {
        pItem = new QTableWidgetItem(QString::number(attr->energy));
        if (!attr->correctEnergy)
        {
            pItem->setBackgroundColor(Qt::red);
        }
        ui->attributesTableView->setItem(ui->attributesTableView->rowCount()-1,columnNb,pItem);
    }
    columnNb = AttributeColumns.indexOf(QString("DFR #%1").arg(winNb));
    if (columnNb!=-1)
    {
        pItem = new QTableWidgetItem(QString::number(attr->dfr));
        if (!attr->correctDfr)
        {
            pItem->setBackgroundColor(Qt::red);
        }
        ui->attributesTableView->setItem(ui->attributesTableView->rowCount()-1,columnNb,pItem);
    }
}*/

/*void SegdConverterWindow::receiveRelation(QString name, float value, bool correct)
{
    int columnNb = AttributeColumns.indexOf(name);
    if (columnNb!=-1)
    {
        QTableWidgetItem *pItem = new QTableWidgetItem(QString::number(value));
        if (!correct)
        {
            pItem->setBackgroundColor(Qt::red);
        }
        ui->attributesTableView->setItem(ui->attributesTableView->rowCount()-1,columnNb,pItem);
    }
}*/

/*void SegdConverterWindow::receiveAuxStatus(const bool &status)
{
    int columnNb = AttributeColumns.indexOf("Check Aux");
    if (columnNb!=-1)
    {
        QTableWidgetItem *pItem = new QTableWidgetItem();
        if (status)
        {
            pItem->setBackgroundColor(Qt::green);
            pItem->setText("ok");
        }
        else
        {
            pItem->setBackgroundColor(Qt::red);
            pItem->setText("bad");
        }
        ui->attributesTableView->setItem(ui->attributesTableView->rowCount()-1,columnNb,pItem);
    }

}*/

/*void SegdConverterWindow::receiveTestStatus(const float &percent, const QColor &color)
{
    int columnNb = AttributeColumns.indexOf("Check Tests");
    if (columnNb!=-1)
    {
        QTableWidgetItem *pItem = new QTableWidgetItem(QString::number(percent));
        pItem->setBackgroundColor(color);
        ui->attributesTableView->setItem(ui->attributesTableView->rowCount()-1,columnNb,pItem);
    }

}*/
void SegdConverterWindow::closeEvent(QCloseEvent *event)
{
    if (ui->actionStop->isEnabled())
    {
        QMessageBox::warning(0,"Дождитесь завершения","Дождитесь завершения процедуры конвертации",QMessageBox::Ok);
        event->ignore();
    }
}
void SegdConverterWindow::aboutQtSlot()
{
    QMessageBox::aboutQt(this);
}

void SegdConverterWindow::aboutSlot()
{
    QMessageBox::about(0,"MSK Hodograph","Экспресс-анализ и конвертация первичных \n"
                                         "сейсмических записей \n"
                                         "Разработка: Мордашов Николай Владимирович \n"
                                         "email: mordashovnv@sngeo.ru \n"
                                         "Группа сопровождения полевых сейсморазведочных работ \n"
                                         "ПАО СНГЕО \n"
                                         "2016 год \n"
                                         "Ver 2.0 \n"  );

}

// открывыем настройки служебных каналов
void SegdConverterWindow::openAuxParametersDialog()
{
    AuxesDialog *dialog = new AuxesDialog(this);
    dialog->exec();
}
