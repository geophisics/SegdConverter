#include "segdconverterwindow.h"
#include "ui_segdconverterwindow.h"
#include "convertparametersdialog.h"
#include "binningparametersdialog.h"
#include "windowsparametersdialog.h"
#include "tableviewdialog.h"
#include <QDir>
#include <QFileDialog>
#include <QHeaderView>
#include "QMessageBox"
#include "auxesdialog.h"
#include "auxviewdialog.h"
#include "SUB/general.h"
#include <mydoublevalidator.h>
#include <testparametersdialog.h>
#include "form.h"

SegdConverterWindow::SegdConverterWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SegdConverterWindow)
{
    ui->setupUi(this);
    settings = new QSettings(QCoreApplication::applicationDirPath()+QDir::separator()+"config.ini",QSettings::IniFormat,this);
    ui->actionStop->setEnabled(false);
    ui->filterGroupBox->setVisible(false);
    ui->actionAuxesDisplay->setEnabled(false);
    ui->actionErase->setEnabled(false);
    readSettings();
    readConvertParamsSettings();
    attr_model=new AttributesModel(this);
    attr_model->setHeaders(settings);
    attr_sortFilterModel = new AttributesSortFilterProxyModel(this);
    attr_sortFilterModel->setSourceModel(attr_model);
    attr_sortFilterModel->setVisibleColumns(settings);
    ui->attributesTableView->setModel(attr_sortFilterModel);
    ui->attributesTableView->setSortingEnabled(false);

    filtersConnecting();
    setColumnsForFiltering();
    setValidators();



    connect(attr_model,SIGNAL(modelReset()),attr_sortFilterModel,SLOT(resetVisibleColumns()));
    connect(attr_model,SIGNAL(modelReset()),this,SLOT(resetTableViewPositions()));
    connect(attr_model,SIGNAL(modelReset()),this,SLOT(setColumnsForFiltering()));
    connect(ui->actionSaveAsAttributes,SIGNAL(triggered(bool)),this,SLOT(saveAsAttributesSlot()));

    connect(ui->actionOpenSegd,SIGNAL(triggered(bool)),this,SLOT(openDataSlot()));
    connect(ui->actionSaveSeisData,SIGNAL(triggered(bool)),this,SLOT(saveDataSlot()));
    connect(ui->actionSaveAttributes,SIGNAL(triggered(bool)),this,SLOT(saveAttributesFileSlot()));

    connect(ui->segdPushButton,SIGNAL(clicked(bool)),this,SLOT(openDataSlot()));
    connect(ui->outFilePushButton,SIGNAL(clicked(bool)),this,SLOT(saveDataSlot()));
    connect(ui->attrFilePushButton,SIGNAL(clicked(bool)),this,SLOT(saveAttributesFileSlot()));

    connect(ui->actionAttributeTable,SIGNAL(triggered(bool)),this,SLOT(openTableViewParametersDialog()));
    connect(ui->actionOpenRPS,SIGNAL(triggered(bool)),this,SLOT(selectRpsFile(bool)));
    connect(ui->actionOpenSPS,SIGNAL(triggered(bool)),this,SLOT(selectSpsFile(bool)));
    connect(ui->actionOpenXPS,SIGNAL(triggered(bool)),this,SLOT(selectXpsFile(bool)));
    connect(ui->actionRun,SIGNAL(triggered(bool)),this,SLOT(runActionSlot()));
    connect(ui->actionAboutQt,SIGNAL(triggered(bool)),this,SLOT(aboutQtSlot()));
    connect(ui->actionAbout,SIGNAL(triggered(bool)),this,SLOT(aboutSlot()));
    connect(ui->actionAuxes,SIGNAL(triggered(bool)),this,SLOT(openAuxParametersDialog(bool)));
    connect(ui->actionAnalyzeTests,SIGNAL(triggered(bool)),this,SLOT(openTestParametersDialog(bool)));
    //connect(ui->actionFilter,SIGNAL(triggered(bool)),ui->filterGroupBox,SLOT(setVisible(bool)));
    connect(ui->actionFilter,SIGNAL(triggered(bool)),this,SLOT(filtersEnabled(bool)));
    connect(ui->actionSorting,SIGNAL(triggered(bool)),this,SLOT(enableSorting(bool)));
    connect(ui->actionErase,SIGNAL(triggered(bool)),this,SLOT(eraseCountedAttributes()));
    connect(ui->attributesTableView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(tableViewItemDoubleClicked(QModelIndex)));
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
//
        if (!viewDialog.isNull())
        {
            viewDialog.data()->clearData();
        }
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
        attr_model->setHeaders(settings);
        if (!viewDialog.isNull())
        {
            viewDialog.data()->clearData();
        }
    }
}
void SegdConverterWindow::openTableViewParametersDialog()
{
    TableViewDialog *dialog = new TableViewDialog(attr_model->getHeaders(),attr_sortFilterModel->getVisibleColumns());
    connect(dialog,SIGNAL(accepted()),attr_sortFilterModel,SLOT(invalidate()));
    connect(dialog,SIGNAL(accepted()),this,SLOT(resetTableViewPositions()));
    dialog->exec();
}
//загружаем настройки
void SegdConverterWindow::readSettings()
{

    settings->beginGroup("/MainSettings");
    ui->segdLineEdit->setText(settings->value("/SegdPath","").toString());
    ui->outFileLineEdit->setText(settings->value("/OutFile","").toString());
    ui->attrFileLineEdit->setText(settings->value("AttrPath","").toString());
    ui->actionAuxes->setChecked(settings->value("/CheckAuxes",false).toBool());
    ui->actionAnalyzeTests->setChecked(settings->value("/CheckTests",false).toBool());
    WorkDir = settings->value("/WorkDir","").toString();
    QByteArray MyArray = settings->value("/TableState","").toByteArray();
    settings->endGroup();
    settings->beginGroup("/ViewSettings");
    int size = settings->beginReadArray("/ColumnForView");
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
    ui->attributesTableView->horizontalHeader()->restoreState(MyArray);

}

//сохраняем настройки
void SegdConverterWindow::saveSettings()
{
    settings->beginGroup("/MainSettings");
    settings->setValue("/SegdPath",ui->segdLineEdit->text());
    settings->setValue("/OutFile",ui->outFileLineEdit->text());
    settings->setValue("/AttrPath",ui->attrFileLineEdit->text());
    settings->setValue("/CheckTests",ui->actionAnalyzeTests->isChecked());
    settings->setValue("CheckAuxes",ui->actionAuxes->isChecked());
    settings->setValue("/WorkDir",WorkDir);
    settings->beginWriteArray("/VisibleColumns");
    for (int i=0; i< attr_sortFilterModel->getVisibleColumns()->count();++i)
    {
        settings->setArrayIndex(i);
        settings->setValue("/columnNb",attr_sortFilterModel->getVisibleColumns()->values().value(i));
    }
    settings->endArray();
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
            WorkDir = QFileInfo(str).absolutePath();
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
            WorkDir = QFileInfo(str).absolutePath();
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
            WorkDir = QFileInfo(str).absolutePath();
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
    QString str = ui->segdLineEdit->text().isEmpty()? WorkDir:QFileInfo(ui->segdLineEdit->text()).path();
    if (ui->segdLabel->text()=="Директория Segd")
    {
        str = QFileDialog::getExistingDirectory(this,"Выберите директорию с *.segd файлами",str,QFileDialog::DontResolveSymlinks);
    }
    else
    {
        str = QFileDialog::getOpenFileName(this,"Выберите файл *.segd",str,"SEGD files(*.segd *.SEGD)");
    }
    if (!str.isEmpty())
    {
        ui->segdLineEdit->setText(str);
        WorkDir = QFileInfo(str).absolutePath();
    }
}
//выбираем файл, в который будет сохранен результат конвертации
void SegdConverterWindow::saveDataSlot()
{
    QString str = ui->outFileLineEdit->text().isEmpty() ? WorkDir : QFileInfo(ui->outFileLineEdit->text()).path();
    if (ui->outFileLabel->text()=="Файл CST")
    {
        str = QFileDialog::getSaveFileName(this,"Выберите файл *.cst для сохранения",str,"SEGD files(*.cst *.CST)");
    }
    else
    {
        str = QFileDialog::getSaveFileName(this,"Выберите файл *.segy для сохранения",str,"SEGY files(*.segy *.SEGY *.sgy *.SGY)");
    }
    if (!str.isEmpty())
    {
        ui->outFileLineEdit->setText(str);
        WorkDir = QFileInfo(str).absolutePath();
    }
}
//выбираем файл для атрибутов
void SegdConverterWindow::saveAttributesFileSlot()
{
    QString str = ui->attrFileLineEdit->text().isEmpty() ? WorkDir : QFileInfo(ui->attrFileLineEdit->text()).path() ;
    str = QFileDialog::getSaveFileName(this,"Выберите файл *.xlsx для сохранения",str,"Excel files(*.xlsx *.XLSX)");
    if (!str.isEmpty())
    {
        ui->attrFileLineEdit->setText(str);
        WorkDir = QFileInfo(str).absolutePath();
    }
}

void SegdConverterWindow::saveAsAttributesSlot()
{
    QString str = ui->attrFileLineEdit->text().isEmpty() ? WorkDir : QFileInfo(ui->attrFileLineEdit->text()).path() ;
    str = QFileDialog::getSaveFileName(this,"Выберите файл *.xlsx для сохранения",str,"Excel files(*.xlsx *.XLSX)");
    if (!str.isEmpty())
    {
        saveAttributes(str);
        //WorkDir = QFileInfo(str).absolutePath();
    }
}

void SegdConverterWindow::runActionSlot()
{

    ui->logTextEdit->clear();
    p_myThread = new QThread();
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,"Атрибуты","Удалить рассчитанные атрибуты?",QMessageBox::Yes|QMessageBox::No|QMessageBox::Save);
    if (reply==QMessageBox::Yes)
    {
        eraseCountedAttributes();
    }
    if (reply==QMessageBox::Save)
    {
        saveAsAttributesSlot();
    }

    ui->actionRun->setEnabled(false);
    ui->actionExit->setEnabled(false);
    ui->actionBinPreferences->setEnabled(false);
    ui->actionAttributesPreferences->setEnabled(false);
    ui->actionConvertParameters->setEnabled(false);
    ui->actionErase->setEnabled(false);
    ui->actionStop->setEnabled(true);

    if (ui->outFileLabel->text()=="Файл CST")
    {
        CstWorker *p_cstWorker = new CstWorker(&running,attr_model->getAttributes());
        //CstWorker *p_cstWorker = new CstWorker(&running,attr_model->getAttributes(),&tMap);
        startThread(p_cstWorker);
    }
    else
    {
        SegyWorker *p_segyWorker = new SegyWorker(&running,attr_model->getAttributes());
        //SegyWorker *p_segyWorker = new SegyWorker(&running,attr_model->getAttributes(),&tMap);
        startThread(p_segyWorker);
    }
}
void SegdConverterWindow::disableStop(bool disable)
{
    Q_UNUSED(disable);
    running = false;
    ui->actionStop->setEnabled(false);
}

void SegdConverterWindow::convertingEnded()
{
     saveAttributes(ui->attrFileLineEdit->text());
     ui->actionRun->setEnabled(true);
     ui->actionBinPreferences->setEnabled(true);
     ui->actionAttributesPreferences->setEnabled(true);
     ui->actionConvertParameters->setEnabled(true);
     ui->actionExit->setEnabled(true);
     ui->actionErase->setEnabled(true);
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

void SegdConverterWindow::startThread(BaseWorker *worker)
{
    running = true;
    worker->moveToThread(p_myThread);
    connect(worker,SIGNAL(sendInfoMessage(QString,QColor)),this,SLOT(recieveInfoMessage(QString,QColor)));
    worker->setSegdPath(ui->segdLineEdit->text());
    worker->setOutPath(ui->outFileLineEdit->text());
    worker->setCheckTests(ui->actionAnalyzeTests->isChecked());
    worker->setCheckAuxes(ui->actionAuxes->isChecked());
    worker->readSettings();
    if (ui->actionOpenRPS->isChecked()) {
        worker->readRps(rpsFile);
    }
    if (ui->actionOpenSPS->isChecked()) {
        worker->readSps(spsFile);
    }
    if (ui->actionOpenXPS->isChecked()) {
        worker->setXpsPath(xpsFile);
    }
    worker->setMode(ui->segdLabel->text()=="Директория Segd");
    if (ui->actionAuxes->isChecked()) {
        setViewAuxesDialog(worker);
    }
    if (ui->actionAnalyzeTests->isChecked())
    {
        setTestViewDialog(worker);
    }
    connect(p_myThread,SIGNAL(started()),worker,SLOT(Converting()));
    connect(worker,SIGNAL(finished()),p_myThread,SLOT(quit()));
    connect(worker,SIGNAL(finished()),this,SLOT(convertingEnded()));
    connect(worker,SIGNAL(finished()),worker,SLOT(deleteLater()));
    connect(p_myThread,SIGNAL(finished()),p_myThread,SLOT(deleteLater()));


    connect(worker,SIGNAL(attributesCounted()),attr_model,SIGNAL(layoutChanged()));
    connect(attr_model,SIGNAL(layoutChanged()),ui->attributesTableView,SLOT(scrollToBottom()));
    connect(ui->actionStop,SIGNAL(triggered(bool)),worker,SLOT(stopRunning()));
    connect(ui->actionStop,SIGNAL(triggered(bool)),this,SLOT(disableStop(bool)));
    p_myThread->start();
}


//конвертация в файл segy
void SegdConverterWindow::runSegy()
{
    running = true;
    SegyWorker *p_segyWorker = new SegyWorker(&running,attr_model->getAttributes());
    p_segyWorker->moveToThread(p_myThread);
    connect(p_segyWorker,SIGNAL(sendInfoMessage(QString,QColor)),this,SLOT(recieveInfoMessage(QString,QColor)));
    p_segyWorker->setSegdPath(ui->segdLineEdit->text());
    p_segyWorker->setOutPath(ui->outFileLineEdit->text());
    p_segyWorker->readSettings();
    if (ui->actionOpenRPS->isChecked()) {
        p_segyWorker->readRps(rpsFile);
    }
    if (ui->actionOpenSPS->isChecked()) {
        p_segyWorker->readSps(spsFile);
    }
    if (ui->actionOpenXPS->isChecked()) {
        p_segyWorker->setXpsPath(xpsFile);
    }
    p_segyWorker->setMode(ui->segdLabel->text()=="Директория Segd");
    if (ui->actionAuxes->isEnabled()) {
        setViewAuxesDialog(p_segyWorker);
    }
    connect(p_myThread,SIGNAL(started()),p_segyWorker,SLOT(Converting()));
    connect(p_segyWorker,SIGNAL(finished()),p_myThread,SLOT(quit()));
    connect(p_segyWorker,SIGNAL(finished()),this,SLOT(convertingEnded()));
    connect(p_segyWorker,SIGNAL(finished()),p_segyWorker,SLOT(deleteLater()));
    connect(p_myThread,SIGNAL(finished()),p_myThread,SLOT(deleteLater()));
    connect(p_segyWorker,SIGNAL(attributesCounted()),attr_model,SIGNAL(layoutChanged()));
    connect(ui->actionStop,SIGNAL(triggered(bool)),p_segyWorker,SLOT(stopRunning()));
    connect(ui->actionStop,SIGNAL(triggered(bool)),this,SLOT(disableStop(bool)));
    p_myThread->start();
}

//конвертация в файл cst
void SegdConverterWindow::runCst()
{
    running = true;
    CstWorker *p_cstWorker = new CstWorker(&running,attr_model->getAttributes());
    p_cstWorker->moveToThread(p_myThread);
    connect(p_cstWorker,SIGNAL(sendInfoMessage(QString,QColor)),this,SLOT(recieveInfoMessage(QString,QColor)));
    p_cstWorker->setSegdPath(ui->segdLineEdit->text());
    p_cstWorker->setOutPath(ui->outFileLineEdit->text());
    p_cstWorker->readSettings();
    if (ui->actionOpenRPS->isChecked()) {
        p_cstWorker->readRps(rpsFile);
    }
    if (ui->actionOpenSPS->isChecked())  {
        p_cstWorker->readSps(spsFile);
    }
    if (ui->actionOpenXPS->isChecked())
    {
        p_cstWorker->setXpsPath(xpsFile);
    }
    p_cstWorker->setMode(ui->segdLabel->text()=="Директория Segd");
    if (ui->actionAuxes->isEnabled())  {
        setViewAuxesDialog(p_cstWorker);
    }

    if (ui->actionAnalyzeTests->isChecked())
    {
        setTestViewDialog(p_cstWorker);
    }
    connect(p_myThread,SIGNAL(started()),p_cstWorker,SLOT(Converting()));
    connect(p_cstWorker,SIGNAL(finished()),p_myThread,SLOT(quit()));
    connect(p_cstWorker,SIGNAL(finished()),this,SLOT(convertingEnded()));
    connect(p_cstWorker,SIGNAL(finished()),p_cstWorker,SLOT(deleteLater()));
    connect(p_myThread,SIGNAL(finished()),p_myThread,SLOT(deleteLater()));
    connect(p_cstWorker,SIGNAL(attributesCounted()),attr_model,SIGNAL(layoutChanged()));
    connect(ui->actionStop,SIGNAL(triggered(bool)),this,SLOT(disableStop(bool)));
    connect(ui->actionStop,SIGNAL(triggered(bool)),p_cstWorker,SLOT(stopRunning()));
    p_myThread->start();
}



void SegdConverterWindow::setViewAuxesDialog(BaseWorker *worker)
{
    if (viewDialog.isNull())
    {
        viewDialog = new AuxViewDialog();
    }
    connect (ui->actionAuxesDisplay,SIGNAL(triggered(bool)),viewDialog.data(),SLOT(show()));
    connect (worker,SIGNAL(sendVectors(QVector<QPointF>*,bool,QVector<QPointF>*,bool,int)),viewDialog,SLOT(receiveVibAuxes(QVector<QPointF>*,bool,QVector<QPointF>*,bool,int)));
    connect (worker,SIGNAL(sendExplAuxes(int, QVector<QPointF>*,bool,QVector<QPointF>*,bool,QVector<QPointF>*,bool,float,float)),viewDialog,SLOT(receiveExplAuxes(int,QVector<QPointF>*,bool,QVector<QPointF>*,bool,QVector<QPointF>*,bool,float,float)));
    viewDialog.data()->show();
    ui->actionAuxesDisplay->setEnabled(true);
}


void SegdConverterWindow::setTestViewDialog(BaseWorker *worker)
{
    if (testViewDialog.isNull())
    {
        testViewDialog = new TestViewDialog(this);
    }
    worker->setTestMap(testViewDialog.data()->getTestMap());
    connect(worker,SIGNAL(testCounted()),testViewDialog.data(),SLOT(newTestReceived()));
    testViewDialog.data()->show();
}


void SegdConverterWindow::saveAttributes(const QString &path)
{
    if (attr_model->saveDataInXlsx(path))
    {
        QMessageBox::information(this,"Сохранено",QString("Рассчитанные атрибуты успешно сохранены в файл\n").append(ui->attrFileLineEdit->text()));
    }
    else
    {
        QMessageBox::critical(this,"Ошибка сохранения",QString("Ошибка сохранения атрибутов в файл\n").append(ui->attrFileLineEdit->text()));
    }
}

void SegdConverterWindow::setValidators()
{
    MyDoubleValidator *doubleVal = new MyDoubleValidator(0,100000,10,ui->firstFilterFromLineEdit);
    doubleVal->setNotation(QDoubleValidator::StandardNotation);
    ui->firstFilterFromLineEdit->setValidator(doubleVal);
    doubleVal = new MyDoubleValidator(0,100000,10,ui->firstFilterToLineEdit);
    doubleVal->setNotation(QDoubleValidator::StandardNotation);
    ui->firstFilterToLineEdit->setValidator(doubleVal);
    doubleVal = new MyDoubleValidator(0,100000,10,ui->secondFilterFromLineEdit);
    doubleVal->setNotation(QDoubleValidator::StandardNotation);
    ui->secondFilterFromLineEdit->setValidator(doubleVal);
    doubleVal = new MyDoubleValidator(0,100000,10,ui->secondFilterToLineEdit);
    doubleVal->setNotation(QDoubleValidator::StandardNotation);
    ui->secondFilterToLineEdit->setValidator(doubleVal);
    doubleVal = new MyDoubleValidator(0,100000,10,ui->thirdFilterFromLineEdit);
    doubleVal->setNotation(QDoubleValidator::StandardNotation);
    ui->thirdFilterFromLineEdit->setValidator(doubleVal);
    doubleVal = new MyDoubleValidator(0,100000,10,ui->thirdFilterToLineEdit);
    doubleVal->setNotation(QDoubleValidator::StandardNotation);
    ui->thirdFilterToLineEdit->setValidator(doubleVal);
}

void SegdConverterWindow::filtersConnecting()
{
    connect(ui->firstFilterFromLineEdit,SIGNAL(textChanged(QString)),this,SLOT(firstRangeChanged()));
    connect(ui->firstFilterToLineEdit,SIGNAL(textChanged(QString)),this,SLOT(firstRangeChanged()));
    connect(ui->secondFilterFromLineEdit,SIGNAL(textChanged(QString)),this,SLOT(secondRangeChanged()));
    connect(ui->secondFilterToLineEdit,SIGNAL(textChanged(QString)),this,SLOT(secondRangeChanged()));
    connect(ui->thirdFilterFromLineEdit,SIGNAL(textChanged(QString)),this,SLOT(thirdRangeChanged()));
    connect(ui->thirdFilterToLineEdit,SIGNAL(textChanged(QString)),this,SLOT(thirdRangeChanged()));

    connect (ui->firstFilterComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(firstFilterComboItemChanged(int)));
    connect (ui->secondFilterComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(secondFilterComboItemChanged(int)));
    connect (ui->thirdFilterComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(thirdFilterComboItemChanged(int)));
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
    if (!ui->actionRun->isEnabled())
    {
        QMessageBox::warning(0,"Дождитесь завершения","Дождитесь завершения процедуры конвертации",QMessageBox::Ok);
        event->ignore();
        return;
    }
    if (!attr_model->dataStatus())
    {
        //QMessageBox::warning(this,"Рассчитанные атрибуты не сохранены")
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this,"Атрибуты","Рассчитанные атрибуты не были сохранены\nПродолжить?",QMessageBox::Yes|QMessageBox::No|QMessageBox::Save);
        if (reply==QMessageBox::No)
        {
            event->ignore();
        }
        if (reply==QMessageBox::Save)
        {
            saveAsAttributesSlot();
        }
    }
}

void SegdConverterWindow::eraseCountedAttributes()
{
    if (!attr_model->dataStatus())
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this,"Атрибуты","Рассчитанные атрибуты не были сохранены\nПродолжить?",QMessageBox::Yes|QMessageBox::No);
        if (reply==QMessageBox::No)
        {
            return;
        }
    }
    attr_model->setHeaders(settings);
    if (!viewDialog.isNull())
    {
        viewDialog.data()->clearData();
    }
    ui->actionErase->setEnabled(false);
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

void SegdConverterWindow::resetTableViewPositions()
{
    /*int j = ui->attributesTableView->horizontalHeader()->count();
    for (int i=0; i<j;i++)
    {
        ui->attributesTableView->horizontalHeader()->setOffsetToSectionPosition(i);
    }*/
    for (int i=0 ; i<ui->attributesTableView->horizontalHeader()->count();i++)
    {
        ui->attributesTableView->horizontalHeader()->moveSection(ui->attributesTableView->horizontalHeader()->visualIndex(i),i);
    }

}

// открывыем настройки служебных каналов
void SegdConverterWindow::openAuxParametersDialog(bool checked)
{
    if (checked)
    {
        AuxesDialog *dialog = new AuxesDialog(this);
        int dialogResult = dialog->exec();//dialog->exec();
        if (dialogResult ==1)
        {
            ui->actionAuxes->setChecked(true);
        }
        else
        {
            ui->actionAuxes->setChecked(false);
        }
    }
    settings->beginGroup("/MainSettings");
    settings->setValue("/CheckAuxes",ui->actionAuxes->isChecked());
    settings->endGroup();
    attr_model->setHeaders(settings);
}

void SegdConverterWindow::openTestParametersDialog(const bool &checked)
{
    if (checked)
    {
        TestParametersDialog *dialog = new TestParametersDialog(settings,this);
        int dialogResult = dialog->exec();
        if (dialogResult==1)
        {
            ui->actionAnalyzeTests->setChecked(true);
        }
        else
        {
            ui->actionAnalyzeTests->setChecked(false);
        }
    }
    settings->beginGroup("/MainSettings");
    settings->setValue("/CheckTests",ui->actionAnalyzeTests->isChecked());
    settings->endGroup();
    attr_model->setHeaders(settings);
}

void SegdConverterWindow::slot1(const int &i)
{
    QObject *obj=sender();
    QMessageBox::warning(0,QString::number(i),obj->objectName());
}

void SegdConverterWindow::setColumnsForFiltering()
{
    ui->firstFilterComboBox->clear();
    ui->secondFilterComboBox->clear();
    ui->thirdFilterComboBox->clear();

    ui->firstFilterComboBox->addItem("None");
    ui->secondFilterComboBox->addItem("None");
    ui->thirdFilterComboBox->addItem("None");

    ui->firstFilterComboBox->addItems(*(attr_model->getHeaders()));
    ui->secondFilterComboBox->addItems(*(attr_model->getHeaders()));
    ui->thirdFilterComboBox->addItems(*(attr_model->getHeaders()));



}

void SegdConverterWindow::firstRangeChanged()
{
    float f1;
    float f2;
    f1=ui->firstFilterFromLineEdit->text().replace(locale().decimalPoint(),'.').toFloat();
    f2=ui->firstFilterToLineEdit->text().replace(locale().decimalPoint(),'.').toFloat();
    attr_sortFilterModel->setFirstRange(f1,f2);
}

void SegdConverterWindow::secondRangeChanged()
{
    float f1;
    float f2;
    f1=ui->secondFilterFromLineEdit->text().replace(locale().decimalPoint(),'.').toFloat();
    f2=ui->secondFilterToLineEdit->text().replace(locale().decimalPoint(),'.').toFloat();
    attr_sortFilterModel->setSecondRange(f1,f2);

}

void SegdConverterWindow::thirdRangeChanged()
{
    float f1;
    float f2;
    f1=ui->thirdFilterFromLineEdit->text().replace(locale().decimalPoint(),'.').toFloat();
    f2=ui->thirdFilterToLineEdit->text().replace(locale().decimalPoint(),'.').toFloat();
    attr_sortFilterModel->setThirdRange(f1,f2);

}

void SegdConverterWindow::filtersEnabled(const bool &b)
{
    if (b) {
        ui->filterGroupBox->setVisible(true);
        attr_sortFilterModel->setFirstFilterColumn(ui->firstFilterComboBox->currentIndex());
        attr_sortFilterModel->setSecondFilterColumn(ui->secondFilterComboBox->currentIndex());
        attr_sortFilterModel->setThirdFilterColumn(ui->thirdFilterComboBox->currentIndex());

    }
    else
    {
        ui->filterGroupBox->setVisible(false);
        attr_sortFilterModel->setFirstFilterColumn(0);
        attr_sortFilterModel->setSecondFilterColumn(0);
        attr_sortFilterModel->setThirdFilterColumn(0);
    }
    attr_sortFilterModel->invalidate();
}


void SegdConverterWindow::enableSorting(const bool &b)
{
    ui->attributesTableView->setSortingEnabled(b);
    if (!b)
    {
        attr_sortFilterModel->setSortRole(Qt::InitialSortOrderRole);
        attr_sortFilterModel->invalidate();
    }
    else
    {
        attr_sortFilterModel->setSortRole(Qt::DisplayRole);
    }
}

void SegdConverterWindow::tableViewItemDoubleClicked(QModelIndex index)
{
    if (viewDialog.isNull())
    {
        return;
    }
    int ffid = attr_model->getFirstColumnValue(index);
    if (viewDialog.data()->showAuxesByFfid(ffid))
    {
        viewDialog.data()->setVisible(true);
    }
    else
    {
        QMessageBox::warning(this, "Внимание",QString("Не найдены служебные каналы для файла %1").arg(ffid));
    }
}

void SegdConverterWindow::firstFilterComboItemChanged(const int &i)
{
    attr_sortFilterModel->setFirstFilterColumn(i);
    if (i==0)
    {
        attr_sortFilterModel->setFirstRange(0,100000);
    }
    else
    {
        attr_sortFilterModel->setFirstRange(attr_model->getMinValueInColumn(i-1),attr_model->getMaxValueInColumn(i-1));
    }
    ui->firstFilterFromLineEdit->blockSignals(true);
    ui->firstFilterToLineEdit->blockSignals(true);
    ui->firstFilterFromLineEdit->setText(QString::number(attr_sortFilterModel->getFirstFilterRange().first));
    ui->firstFilterToLineEdit->setText(QString::number(attr_sortFilterModel->getFirstFilterRange().second));
    ui->firstFilterFromLineEdit->blockSignals(false);
    ui->firstFilterToLineEdit->blockSignals(false);

}

void SegdConverterWindow::secondFilterComboItemChanged(const int &i)
{
    attr_sortFilterModel->setSecondFilterColumn(i);
    if (i==0)
    {
        attr_sortFilterModel->setSecondRange(0,100000);
    }
    else
    {
        attr_sortFilterModel->setSecondRange(attr_model->getMinValueInColumn(i-1),attr_model->getMaxValueInColumn(i-1));
    }
    ui->secondFilterFromLineEdit->blockSignals(true);
    ui->secondFilterToLineEdit->blockSignals(true);
    ui->secondFilterFromLineEdit->setText(QString::number(attr_sortFilterModel->getSecondFilterRange().first));
    ui->secondFilterToLineEdit->setText(QString::number(attr_sortFilterModel->getSecondFilterRange().second));
    ui->secondFilterFromLineEdit->blockSignals(false);
    ui->secondFilterToLineEdit->blockSignals(false);
}


void SegdConverterWindow::thirdFilterComboItemChanged(const int &i)
{
    attr_sortFilterModel->setThirdFilterColumn(i);
    if (i==0)
    {
        attr_sortFilterModel->setThirdRange(0,100000);
    }
    else
    {
        attr_sortFilterModel->setThirdRange(attr_model->getMinValueInColumn(i-1),attr_model->getMaxValueInColumn(i-1));
    }
    ui->thirdFilterFromLineEdit->blockSignals(true);
    ui->thirdFilterToLineEdit->blockSignals(true);
    ui->thirdFilterFromLineEdit->setText(QString::number(attr_sortFilterModel->getThirdFilterRange().first));
    ui->thirdFilterToLineEdit->setText(QString::number(attr_sortFilterModel->getThirdFilterRange().second));
    ui->thirdFilterFromLineEdit->blockSignals(false);
    ui->thirdFilterToLineEdit->blockSignals(false);
}

