#include "auxesdialog.h"
#include "ui_auxesdialog.h"
#include "Segd/segdfile.h"
#include <QMessageBox>
#include <attributewindow.h>
#include <QFileDialog>
#include "mydoublevalidator.h"
#include "aquila/global.h"
#include "aquila/transform/FftFactory.h"
#include "aquila/functions.h"
#include <algorithm>
#include <functional>
#include <memory>



AuxesDialog::AuxesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AuxesDialog)
{
    ui->setupUi(this);
    settings = new QSettings(QCoreApplication::applicationDirPath()+QDir::separator()+"config.ini",QSettings::IniFormat,this);
    MyDoubleValidator *doubleVal = new MyDoubleValidator(0,10000,2,ui->timeBreakAmaxLineEdit);
    doubleVal->setNotation(QDoubleValidator::StandardNotation);
    ui->timeBreakAmaxLineEdit->setValidator(doubleVal);
    doubleVal = new MyDoubleValidator(0,10000,2,ui->confirmedTimeBreakAmaxLineEdit);
    doubleVal->setNotation(QDoubleValidator::StandardNotation);
    ui->confirmedTimeBreakAmaxLineEdit->setValidator(doubleVal);
    readSettings();
    connect(ui->openSegdPushButton,SIGNAL(clicked(bool)),this,SLOT(openVibroSegdSlot()));
    connect(ui->openExamplePushButton,SIGNAL(clicked(bool)),this,SLOT(openExplSegdSlot()));
    connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(writeSettings()));
}

AuxesDialog::~AuxesDialog()
{
    delete ui;
}

void AuxesDialog::openVibroSegdSlot()
{
    //if (ui->maxFreqSpinBox->value() <= ui->mi)
    QString str = QFileDialog::getOpenFileName(this,"Выберите файл *.segd",QCoreApplication::applicationDirPath(),"SEGD files(*.segd *.SEGD)");
    if (!str.isEmpty())
    {
        SegdFile *segdAuxes = new SegdFile(str,true,this);
        if (segdAuxes->getExtendedHeader().getTypeOfSource()==2 && segdAuxes->getExtendedHeader().getTypeOfProcess()>2 && segdAuxes->getFileState())
        {
            if (ui->akfTraceNbSpinBox->value()>segdAuxes->getTraces().count())
            {
                QMessageBox::warning(0,"Ошибка","Номер трассы автокорреляционной функции превышает число служебных каналов");
            }
            else
            {
                QVector<float> traceData = segdAuxes->getTrace(ui->akfTraceNbSpinBox->value()-1)->getTraceData();
                getSpectrum(traceData.toStdVector() ,segdAuxes->getExtendedHeader().getSampleRate(), segdAuxes->getExtendedHeader().getDumpStackingFold());
                delete segdAuxes;
            }
        }
        else
        {
            QMessageBox::warning(0,"Ошибка","Выбранный файл не является коррелограммой");
            delete segdAuxes;
        }
    }
}

void AuxesDialog::openExplSegdSlot()
{
    QString str = QFileDialog::getOpenFileName(this,"Выберите файл *.segd",QCoreApplication::applicationDirPath(),"SEGD files(*.segd *.SEGD)");
    if (!str.isEmpty())
    {
        SegdFile *segdAuxes = new SegdFile(str,true,this);
        if (segdAuxes->getExtendedHeader().getTypeOfSource()==1 && segdAuxes->getFileState())
        {
            if (ui->timeBreakTraceNbSpinBox->value() > segdAuxes->getTraces().count())
            {
                QMessageBox::warning(0,"Ошибка","Номер трассы отметки момента превышает число служебных каналов\n"
                                                "Настройки  отметки момента не обновлены");
            }
            else
            {
                setTimeBreakSettings(segdAuxes->getTrace(ui->timeBreakTraceNbSpinBox->value()-1)->getTraceData(),segdAuxes->getExtendedHeader().getSampleRate());
            }
            if (ui->confirmedTimeBreakTraceNbSpinBox->value()>segdAuxes->getTraces().count())
            {
                QMessageBox::warning(0,"Ошибка","Номер трассы подтвержденной отметки момента превышает число служебных каналов\n"
                                                "Настройки подтвержденной отметки момента не обновлены");
            }
            else
            {
                setConfirmedTimeBreakSettings(segdAuxes->getTrace(ui->confirmedTimeBreakTraceNbSpinBox->value()-1)->getTraceData(),segdAuxes->getExtendedHeader().getSampleRate());
            }
        }
        else
        {
            QMessageBox::warning(0,"Ошибка","Выбранный файл не является взрывным");
        }
        delete segdAuxes;
    }
}

void AuxesDialog::writeSettings()
{
    settings->beginGroup("/VibAuxSettings");
    settings->setValue("/AkfTrace",ui->akfTraceNbSpinBox->value());
    settings->setValue("/MinFreq",ui->minFreqSpinBox->value());
    settings->setValue("/MaxFreq",ui->maxFreqSpinBox->value());
    settings->setValue("/PeakTime",ui->maxTimeSpinBox->value());
    settings->setValue("/FreqLvl",ui->freqLevelSpinBox->value());
    settings->setValue("/AkfMaxAmpl",ui->akfAmplDoubleSpinBox->value());
    settings->endGroup();
    settings->beginGroup("/ExplAuxSettings");
    settings->setValue("/CheckTimeBreak",ui->timeBreakCheckBox->isChecked());
    settings->setValue("/TimeBreakTraceNb",ui->timeBreakTraceNbSpinBox->value());
    settings->setValue("/TimeBreakTmax",ui->timeBreakTmaxSpinBox->value());
    settings->setValue("/TimeBreakAmax",ui->timeBreakAmaxLineEdit->text().replace(locale().decimalPoint(),'.'));
    settings->setValue("/TimeBreakNbOfDiscret",ui->timeBreakNbOfDiscretSpinBox->value());

    settings->setValue("/CheckConfirmedTimeBreak",ui->confirmedTimeBreakCheckBox->isChecked());
    settings->setValue("/ConfirmedTimeBreakTraceNb",ui->confirmedTimeBreakTraceNbSpinBox->value());
    settings->setValue("/ConfirmedTimeBreakTmax",ui->confirmedTimeBreakTmaxSpinBox->value());
    settings->setValue("/ConfirmedTimeBreakAmax",ui->confirmedTimeBreakAmaxLineEdit->text().replace(locale().decimalPoint(),'.'));
    settings->setValue("/ConfirmedTimeBreakNbOfDiscret",ui->confirmedTimeBreakNbOfDiscretSpinBox->value());

    settings->setValue("/CheckUpholeTime",ui->upholeTimeCheckBox->isChecked());
    settings->setValue("/UpholeTimeTraceNb",ui->upholeTimeTraceNbSpinBox->value());
    settings->endGroup();
    accept();
}

void AuxesDialog::readSettings()
{
    settings->beginGroup("/VibAuxSettings");
    ui->akfTraceNbSpinBox->setValue(settings->value("/AkfTrace",2).toInt());
    ui->minFreqSpinBox->setValue(settings->value("/MinFreq",7).toInt());
    ui->maxFreqSpinBox->setValue(settings->value("MaxFreq",100).toInt());
    ui->maxTimeSpinBox->setValue(settings->value("/PeakTime",500).toInt());
    ui->freqLevelSpinBox->setValue(settings->value("/FreqLvl",50).toInt());
    ui->akfAmplDoubleSpinBox->setValue(settings->value("/AkfMaxAmpl",0.0).toDouble());
    settings->endGroup();

    settings->beginGroup("/ExplAuxSettings");

    ui->timeBreakCheckBox->setChecked(settings->value("/CheckTimeBreak",false).toBool());
    ui->timeBreakSettingsGroupBox->setEnabled(ui->timeBreakCheckBox->isChecked());
    ui->timeBreakTraceNbSpinBox->setValue(settings->value("/TimeBreakTraceNb",1).toInt());
    ui->timeBreakTmaxSpinBox->setValue(settings->value("/TimeBreakTmax",10).toInt());
    ui->timeBreakAmaxLineEdit->setText(settings->value("/TimeBreakAmax",500).toString().replace('.',locale().decimalPoint()));
    ui->timeBreakNbOfDiscretSpinBox->setValue(settings->value("/TimeBreakNbOfDiscret",4).toInt());

    ui->confirmedTimeBreakCheckBox->setChecked(settings->value("/CheckConfirmedTimeBreak",false).toBool());
    ui->confirmedTimeBreakSettingsGroupBox->setEnabled(ui->confirmedTimeBreakCheckBox->isChecked());
    ui->confirmedTimeBreakTraceNbSpinBox->setValue(settings->value("/ConfirmedTimeBreakTraceNb",2).toInt());
    ui->confirmedTimeBreakTmaxSpinBox->setValue(settings->value("/ConfirmedTimeBreakTmax",10).toInt());
    ui->confirmedTimeBreakAmaxLineEdit->setText(settings->value("/ConfirmedTimeBreakAmax",500).toString().replace('.',locale().decimalPoint()));
    ui->confirmedTimeBreakNbOfDiscretSpinBox->setValue(settings->value("/ConfirmedTimeBreakNbOfDiscret",90).toInt());

    ui->upholeTimeCheckBox->setChecked(settings->value("/CheckUpholeTime",false).toBool());
    ui->upholeTimeTraceNbSpinBox->setEnabled(ui->upholeTimeCheckBox->isChecked());
    ui->upholeTimeTraceNbSpinBox->setValue(settings->value("/UpholeTimeTraceNb",3).toInt());

    settings->endGroup();

}

void AuxesDialog::getSpectrum(std::vector<float> vec, const int &sampleRate, const int &dump)
{
    const std::size_t SIZE = pow(2,ceil(log2(vec.size())));
    std::vector <double> vecD(vec.begin(),vec.end());
    vecD.resize(SIZE);
    float frqStep = 1000000.0/sampleRate/SIZE;
    std::shared_ptr<Aquila::Fft> fft = Aquila::FftFactory::getFft(SIZE);
    Aquila::SpectrumType spectrum = fft->fft(vecD.data());
    const std::size_t SPECTRUM_SIZE = SIZE/2+1;
    std::vector<double> ampls(SPECTRUM_SIZE);
    for (std::size_t i=0;i<SPECTRUM_SIZE;++i)
    {
        ampls[i]= std::abs(spectrum[i]);
    }
    Aquila::SampleType max = *(std::max_element(ampls.begin(),ampls.end()));
    std::vector<double> spectrumdB(SPECTRUM_SIZE);
    for (std::size_t i=0;i<SPECTRUM_SIZE;++i)
    {
        spectrumdB[i]= Aquila::dB(ampls[i],max);
        //qDebug()<<spectrumdB[i];
    }
    //float frqStep = 500000.0/sampleRate/SPECTRUM_SIZE;
    //qDebug()<<"\n";
    //qDebug()<<frqStep;
    int freqCount =std::round(ui->minFreqSpinBox->value()/frqStep);
    //qDebug()<<freqCount;
    float minFreqLevel = spectrumdB[freqCount];
    freqCount =std::round(ui->maxFreqSpinBox->value()/frqStep);
    //qDebug()<<freqCount;
    float maxFreqLevel = spectrumdB[freqCount];
    if (fabs(minFreqLevel-maxFreqLevel)<5)
    {

        ui->freqLevelSpinBox->setValue((minFreqLevel+maxFreqLevel)/2);
        ui->maxTimeSpinBox->setValue(std::distance(vecD.begin(),std::max_element(vecD.begin(),vecD.end())) * sampleRate /1000);
        ui->akfAmplDoubleSpinBox->setValue(*std::max_element(vecD.begin(),vecD.end())/dump);

    }
    else
    {
        QMessageBox::warning(0,"Ошибка","Выбранный файл не соответствует установленным параметрам свип-сигнала");
    }
    vec.clear();
    vecD.clear();
    spectrum.clear();
    spectrumdB.clear();
}

void AuxesDialog::setTimeBreakSettings(QVector<float> traceData, const int &sInt)
{
    float maxAmpl = *std::max_element(traceData.begin(),traceData.end());
    int firstMaxDiscret = traceData.indexOf(maxAmpl);
    int lastMaxDiscret = traceData.lastIndexOf(maxAmpl);

    ui->timeBreakTmaxSpinBox->setValue(firstMaxDiscret*sInt/1000);
    ui->timeBreakNbOfDiscretSpinBox->setValue(lastMaxDiscret-firstMaxDiscret+1);
    ui->timeBreakAmaxLineEdit->setText(QString::number(maxAmpl,'f',2).replace('.',locale().decimalPoint()));

}

void AuxesDialog::setConfirmedTimeBreakSettings(QVector<float> traceData, const int &sInt)
{
    float maxAmpl = *std::max_element(traceData.begin(),traceData.end());
    int firstMaxDiscret = traceData.indexOf(maxAmpl);
    int lastMaxDiscret = firstMaxDiscret;
    while (traceData.value(lastMaxDiscret)==maxAmpl)
    {
        lastMaxDiscret++;
    }
    ui->confirmedTimeBreakTmaxSpinBox->setValue(firstMaxDiscret*sInt/1000);
    ui->confirmedTimeBreakNbOfDiscretSpinBox->setValue(lastMaxDiscret-firstMaxDiscret+1);
    ui->confirmedTimeBreakAmaxLineEdit->setText(QString::number(maxAmpl,'f',2).replace('.',locale().decimalPoint()));

}
