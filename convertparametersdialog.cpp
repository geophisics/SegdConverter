#include "convertparametersdialog.h"
#include "ui_convertparametersdialog.h"
#include <QDir>
#include <QFileDialog>
#include <mydoublevalidator.h>
#include <QMessageBox>


ConvertParametersDialog::ConvertParametersDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConvertParametersDialog)
{
    //this->setLocale(QLocale(QLocale::English,QLocale::UnitedStates));
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    MyDoubleValidator *doubleVal = new MyDoubleValidator(0,1,20,ui->userMpFactorLineEdit);
    doubleVal->setNotation(QDoubleValidator::StandardNotation);
    ui->userMpFactorLineEdit->setValidator(doubleVal);
    //ui->userMpFactorLineEdit->setValidator();
    settings = new QSettings(QCoreApplication::applicationDirPath()+QDir::separator()+"config.ini",QSettings::IniFormat,this);
    connect(ui->testsCheckBox,SIGNAL(stateChanged(int)),this,SLOT(activateTestSpinBox(int)));
    readSettings();

    connect(ui->onlineYesRadioButton,SIGNAL(toggled(bool)),ui->waitingTimeSpinBox,SLOT(setEnabled(bool)));
    //setAttribute(Qt::WA_DeleteOnClose);
}

ConvertParametersDialog::~ConvertParametersDialog()
{
    //saveSettings();
    delete ui;
}

//выбор папки для копий segd файлов
void ConvertParametersDialog::selectBackupFolderSlot()
{
    QString str=ui->backupLineEdit->text();
    if (str.isEmpty())
    {
        str = QFileDialog::getExistingDirectory(this,"Выберите директорию для резервного копирования",QDir::currentPath(),QFileDialog::DontResolveSymlinks);
    }
    else
    {
        //str=str.left(str.lastIndexOf(QDir::separator()));
        str = QFileDialog::getExistingDirectory(this,"Выберите директорию для резервного копирования",str,QFileDialog::DontResolveSymlinks);
    }
    if (!str.isEmpty())
    {
        ui->backupLineEdit->setText(str);
        //workDir = str;
    }
}
//выбираем мезовский файл с эксклюзивными зонами
void ConvertParametersDialog::selectExclusionFileSlot()
{
    QString str = ui->exclusionLineEdit->text();
    QString filter;
    if (ui->xclRadioButton->isChecked())
    {
        filter = "XCL files (*.xcl)";
    }
    else
    {
        filter = "Txt files (*.txt)";
    }
    if (str.isEmpty())
    {
        str = QFileDialog::getOpenFileName(this,"Выберите файл с эксклюзивными зонами",QDir::currentPath(),filter);
    }
    else
    {
        str = QFileDialog::getOpenFileName(this,"Выберите файл с эксклюзивными зонами",str,filter);
    }
    if (!str.isEmpty())
    {
        ui->exclusionLineEdit->setText(str);
    }
}
// делаем активной или неактивной возможность выбора папки для бекапа
void ConvertParametersDialog::backupCheckBoxStateChangedSlot(int state)
{
    if (state==2)
    {
        ui->backupLineEdit->setEnabled(true);
        ui->backupPushButton->setEnabled(true);
    }
    else
    {
        ui->backupLineEdit->setEnabled(false);
        ui->backupPushButton->setEnabled(false);
    }
}
//делаем активным или неактивным спинбокс с допусками по тестам
void ConvertParametersDialog::activateTestSpinBox(int state)
{
    if (state==2)
    {
        ui->testsSpinBox->setEnabled(true);
    }
    else
    {
        ui->testsSpinBox->setEnabled(false);
    }
}
// делаем активными или неактивными настройки максимального числа файлов
void ConvertParametersDialog::maxFilesCheckBoxStateChangedSlot(int state)
{
    if (state==2)
    {
        ui->maxFileHorizontalSlider->setEnabled(true);
        ui->maxFileSpinBox->setEnabled(true);
    }
    else
    {
        ui->maxFileHorizontalSlider->setEnabled(false);
        ui->maxFileSpinBox->setEnabled(false);
    }
}


//делаем активным или нет возможность выбора файла эксклюзивных зон

void ConvertParametersDialog::exclusionCheckBoxStateChangedSlot(int state)
{
    if (state==2)
    {
        ui->exclusionLineEdit->setEnabled(true);
        ui->exclusionPushButton->setEnabled(true);
        ui->xclRadioButton->setEnabled(true);
        ui->txtRadioButton->setEnabled(true);
    }
    else
    {
        ui->exclusionLineEdit->setEnabled(false);
        ui->exclusionPushButton->setEnabled(false);
        ui->xclVerticalLayout->setEnabled(false);
        ui->xclRadioButton->setEnabled(false);
        ui->txtRadioButton->setEnabled(false);
    }
}
//делаем активными или нет lineEdit с пользовательским MP фактором
void ConvertParametersDialog::userMpFactorRadioButtonToogled(bool checked)
{
    ui->userMpFactorLineEdit->setEnabled(checked);
}
//сохраняем настройки
void ConvertParametersDialog::saveSettings()
{

//extern Q_CORE_EXPORT int qt_ntfs_permission_lookup;
//qt_ntfs_permission_lookup++;
//QFile::Permissions perm = QFile::permissions(ui->backupLineEdit->text());
//    if (ui->backupCheckBox->isChecked())
//    {
//        if (!(QFile::permissions(ui->backupLineEdit->text()) & QFile::WriteOther))
//        {
//            QMessageBox::warning(0, "Проверьте путь", "Путь, заданный для резервной копии"
//                                                      " недоступен для записи");
//            return;
//        }
//    }
    if (ui->userMpFactorRadioButton->isChecked())
    {
        //QLocale locale(QLocale::ru)

        QString str = ui->userMpFactorLineEdit->text().replace(locale().decimalPoint(),'.');
        double d = str.toDouble();
        //double d =str.toDouble(); //ui->userMpFactorLineEdit->text().toDouble();
        if (d==0)
        {
            QMessageBox::warning(0, "Проверьте значение", "MP-фактор не может быть равен 0");
            return;
        }

    }
    settings->beginGroup("/ConvertSettings");
    settings->setValue("/ReadMode",ui->fromFileRadioButton->isChecked());
    settings->setValue("/OnLine",ui->onlineYesRadioButton->isChecked());
    settings->setValue("/WaitingTime",ui->waitingTimeSpinBox->value());
    settings->setValue("/CST",ui->formatCstRadioButton->isChecked());
    settings->setValue("/CheckTests",ui->testsCheckBox->isChecked());
    settings->setValue("/TestPercent",ui->testsSpinBox->value());
    settings->setValue("/NoWriteAuxes",ui->noWriteAuxRadioButton->isChecked());
    settings->setValue("/WriteAuxes",ui->writeAuxRadioButton->isChecked());
    settings->setValue("/WriteAuxesInNewFile",ui->writeAuxInNewFileRadioButton->isChecked());
    settings->setValue("/AnalisysAuxes",ui->analisysAuxesCheckBox->isChecked());
    settings->setValue("/MuteChannels",ui->useMuteChannelsRadioButton->isChecked());
    settings->setValue("/MissedChannels",ui->writeMissedRadioButton->isChecked());
    settings->setValue("/NotUseMpFactor",ui->notUseMpFactorRadioButton->isChecked());
    settings->setValue("/UseHeaderMpFactor",ui->headersMpFactorRadioButton->isChecked());
    settings->setValue("/UseUserMpFactor",ui->userMpFactorRadioButton->isChecked());
    settings->setValue("/UserMpFactorValue",ui->userMpFactorLineEdit->text());
    settings->setValue("/Backup",ui->backupCheckBox->isChecked());
    settings->setValue("/BackupFolder",ui->backupLineEdit->text());
    settings->setValue("/Exclusions",ui->exclusionCheckBox->isChecked());
    settings->setValue("/TxtExclusion",ui->txtRadioButton->isChecked());
    settings->setValue("/XclExlusion",ui->xclRadioButton->isChecked());
    settings->setValue("/ExclusionsPath",ui->exclusionLineEdit->text());
    settings->setValue("/MaxFiles",ui->maxFileCheckBox->isChecked());
    settings->setValue("/MaxFilesValue",ui->maxFileSpinBox->value());
    settings->endGroup();
    accept();
}
//загружаем настройки
void ConvertParametersDialog::readSettings()
{
    settings->beginGroup("/ConvertSettings");
    ui->fromFileRadioButton->setChecked(settings->value("ReadMode",1).toBool());
    ui->allFolderRadioButton->setChecked(!ui->fromFileRadioButton->isChecked());
    ui->onlineYesRadioButton->setChecked(settings->value("/OnLine",0).toBool());
    ui->onlineNoRadioButton->setChecked(!ui->onlineYesRadioButton->isChecked());
    ui->waitingTimeSpinBox->setValue(settings->value("/WaitingTime",10).toInt());
    ui->waitingTimeSpinBox->setEnabled(ui->onlineYesRadioButton->isChecked());
    ui->formatCstRadioButton->setChecked(settings->value("/CST",1).toBool());
    ui->formatSegyRadioButton->setChecked(!ui->formatCstRadioButton->isChecked());
    ui->testsCheckBox->setChecked(settings->value("/CheckTests",false).toBool());
    ui->testsSpinBox->setValue(settings->value("/TestPercent",2).toInt());
    ui->testsSpinBox->setEnabled(ui->testsCheckBox->isChecked());
    ui->noWriteAuxRadioButton->setChecked(settings->value("/NoWriteAuxes",1).toBool());
    ui->writeAuxRadioButton->setChecked(settings->value("/WriteAuxes",0).toBool());
    ui->writeAuxInNewFileRadioButton->setChecked(settings->value("/WriteAuxesInNewFile",0).toBool());
    ui->analisysAuxesCheckBox->setChecked(settings->value("/AnalisysAuxes",false).toBool());
    ui->useMuteChannelsRadioButton->setChecked(settings->value("/MuteChannels",1).toBool());
    ui->noUseMuteChannelsRadioButton->setChecked(!ui->useMuteChannelsRadioButton->isChecked());
    ui->writeMissedRadioButton->setChecked(settings->value("/MissedChannels",false).toBool());
    ui->noWriteMissedRadioButton->setChecked(!ui->writeMissedRadioButton->isChecked());
    ui->notUseMpFactorRadioButton->setChecked(settings->value("/NotUseMpFactor",0).toBool());
    ui->headersMpFactorRadioButton->setChecked(settings->value("/UseHeaderMpFactor",1).toBool());
    ui->userMpFactorRadioButton->setChecked(settings->value("/UseUserMpFactor",0).toBool());
    ui->userMpFactorLineEdit->setText(settings->value("/UserMpFactorValue","1").toString());
    ui->userMpFactorLineEdit->setEnabled(ui->userMpFactorRadioButton->isChecked());
    ui->exclusionCheckBox->setChecked(settings->value("/Exclusions",0).toBool());
    ui->txtRadioButton->setChecked(settings->value("/TxtExclusion",false).toBool());
    ui->xclRadioButton->setChecked(settings->value("/XclExlusion",true).toBool());
    ui->exclusionLineEdit->setText(settings->value("/ExclusionsPath","").toString());
    ui->exclusionLineEdit->setEnabled(ui->exclusionCheckBox->isChecked());
    ui->exclusionPushButton->setEnabled(ui->exclusionCheckBox->isChecked());
    ui->txtRadioButton->setEnabled(ui->exclusionCheckBox->isChecked());
    ui->xclRadioButton->setEnabled(ui->exclusionCheckBox->isChecked());
    ui->backupCheckBox->setChecked(settings->value("/Backup",0).toBool());
    ui->backupLineEdit->setText(settings->value("/BackupFolder","").toString());
    ui->backupLineEdit->setEnabled(ui->backupCheckBox->isChecked());
    ui->backupPushButton->setEnabled(ui->backupCheckBox->isChecked());
    ui->maxFileCheckBox->setChecked(settings->value("/MaxFiles",0).toBool());
    ui->maxFileSpinBox->setValue(settings->value("/MaxFilesValue",100).toInt());
    ui->maxFileSpinBox->setEnabled(ui->maxFileCheckBox->isChecked());
    ui->maxFileHorizontalSlider->setEnabled(ui->maxFileCheckBox->isChecked());
    settings->endGroup();
}
