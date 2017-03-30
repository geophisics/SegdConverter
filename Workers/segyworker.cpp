#include "segyworker.h"
#include <Segd/segdfile.h>
#include <Segy/segyfile.h>
#include "aquila/functions.h"

QTXLSX_USE_NAMESPACE

SegyWorker::SegyWorker(QObject *parent) : BaseWorker(parent)
{
    settings = new QSettings(QCoreApplication::applicationDirPath()+QDir::separator()+"config.ini",QSettings::IniFormat,this);
    //readSettings();
}


void SegyWorker::Converting()
{
    QFileInfo fInfo(segdPath);
    QDir segdDir;

    logFile = new QFile(outPath+"_log.txt");
    logFile->open(QIODevice::Text|QIODevice::WriteOnly);
    logStream = new QTextStream(logFile);
    logStream->setCodec("UTF8");
    *logStream << QString("%1 Начало конвертации\n").arg(QDateTime::currentDateTime().toString("ddd dd.MMMM.yyyy hh:mm::ss"));

    int fileForConvertingNum;
    QFileInfoList segdFilesInDir;
    QStringList filter;
    filter << "*.segd";
    if (fInfo.isDir() && mode)
    {
        segdDir.setPath(segdPath);
        segdFilesInDir = segdDir.entryInfoList(filter,QDir::Files,QDir::Name);
        fileForConvertingNum =0;
    }
    else if (fInfo.isFile() && !mode)
    {
        segdDir=fInfo.dir();
        segdFilesInDir = segdDir.entryInfoList(filter,QDir::Files,QDir::Name);
        fileForConvertingNum = segdFilesInDir.indexOf(fInfo);
    }
    else
    {
        if (mode)
        {
            *logStream << QString ("Не найдена директория %1\n").arg(segdPath);
            emit sendSomeError(QString ("Не найдена директория %1").arg(segdPath));
        }
        else
        {
            *logStream << QString ("Не найден файл %1\n").arg(segdPath);
            emit sendSomeError(QString ("Не найден файл %1").arg(segdPath));
        }
        *logStream << QString("%1 Завершение конвертации\n").arg(QDateTime::currentDateTime().toString("ddd dd.MMMM.yyyy hh:mm::ss"));
        delete logStream;
        logFile->close();
        delete logFile;
        emit finished();
        return;
    }
    int fileCount = 0; // количество ф.н. в сводном файле
    writeFileHeaders = true;
    run = new bool;
    *run=true;
    while (fileForConvertingNum<segdFilesInDir.count() && *run)
    {
        if (convertOneFile(segdFilesInDir.value(fileForConvertingNum).absoluteFilePath(),writeFileHeaders))
        {
            fileCount++;
            writeFileHeaders = false;
            *logStream << QString("%1 Выполнена конвертация файла %2\n").arg(QDateTime::currentDateTime().toString("ddd dd.MMMM.yyyy hh:mm::ss")).arg(segdFilesInDir.value(fileForConvertingNum).fileName());
            emit sendInfoMessage("Выполнена конвертация файла "+segdFilesInDir.value(fileForConvertingNum).fileName(),Qt::darkGreen);
        }
        else {
            *logStream << QString("%1 Ошибка чтения файла %2. Переход к следующему файлу ").arg(QDateTime::currentDateTime().toString("ddd dd.MMMM.yyyy hh:mm::ss")).arg(segdFilesInDir.value(fileForConvertingNum).fileName());
            emit sendSomeError(QString ("Ошибка чтения файла %1. Переход к следующему файлу").arg(segdFilesInDir.value(fileForConvertingNum).fileName()));
        }

        if (limitMaxFiles && fileCount >= maxFilesValue)
        {
            fileCount=0;
            outPath.insert(outPath.lastIndexOf('/')+1,'_');
            outAuxesPath.insert(outAuxesPath.lastIndexOf('/')+1,'_');
            writeFileHeaders = true;
            createFileForMissedTraces();
        }
        fileForConvertingNum++;
    }
    *logStream << QString("%1 Завершение конвертации\n").arg(QDateTime::currentDateTime().toString("ddd dd.MMMM.yyyy hh:mm::ss"));
    delete logStream;
    logFile->close();
    delete logFile;
    emit finished();
}

void SegyWorker::countAttributes(SegyFile *sgy)
{
    QVector<QVector<float> > tracesInWindow;
    QList<AttributeWindow*>::iterator windowsIterator= windows.begin();
    QMap<QString,float> amplitudes;
    int windowCount=0;
    AttributeWindow *attrWin;
    float attribute=0.0;
    bool checkAttribute=true;
    for(;windowsIterator!=windows.end(); ++ windowsIterator )
    {
        attrWin  = *windowsIterator;
        *logStream << QString("%1 Расчет атрибутов в окне Min Offset = %2; Max Offset = %3; Min Time = %4мс; Max Time = %5мс\n").arg(QDateTime::currentDateTime().toString("ddd dd.MMMM.yyyy hh:mm::ss")).arg(attrWin ->getMinOffset()).arg(attrWin->getMaxOffset()).arg(attrWin->getMinTime()).arg(attrWin->getMaxTime());
        windowCount++;
        if (useExclusions)
        {
            if  (exType == exclusionType::txtExcl) {
                tracesInWindow = sgy->getDataInWindow(logStream,attrWin ->getMinOffset(),attrWin ->getMaxOffset(),attrWin ->getMinTime(),attrWin ->getMaxTime(),notUseMutedTraces,badTests,minAmpl,exclPoints);
            }
            else
            {
                tracesInWindow = sgy->getDataInWindow(logStream,attrWin ->getMinOffset(),attrWin ->getMaxOffset(),attrWin ->getMinTime(),attrWin ->getMaxTime(),notUseMutedTraces,badTests,minAmpl,exclusions);
            }
        }
        else {
            tracesInWindow = sgy->getDataInWindow(logStream,attrWin ->getMinOffset(),attrWin ->getMaxOffset(),attrWin ->getMinTime(),attrWin ->getMaxTime(),notUseMutedTraces,badTests,minAmpl);
        }
        if (attrWin->getCountAmpl() )
        {
            attribute = getAbsAvg(tracesInWindow);
            checkAttribute = attribute>attrWin->getMinAmpl() ? true:false;
            fileAttributes.append(qMakePair(attribute,checkAttribute));
            amplitudes.insert(QString("A%1").arg(windowCount),attribute);
        }
        if (attrWin->getCountRms())
        {
            attribute = getAbsAvg(tracesInWindow);
            checkAttribute = attribute>attrWin->getMinRms() ? true:false;
            fileAttributes.append(qMakePair(attribute,checkAttribute));
            amplitudes.insert(QString("R%1").arg(windowCount),attribute);
        }
        if (attrWin->getCountFreq())
        {
            attribute = countFreq(tracesInWindow,sgy->getSampleInterval());
            checkAttribute = attribute>attrWin->getMinFreq() ? true:false;
            fileAttributes.append(qMakePair(attribute,checkAttribute));
        }
        if (attrWin->getCountEnergy() || attrWin->getCountDfr() || attrWin->getWriteSpectrum() )
        {
            std::vector<double> spectrum = getSpectrum(tracesInWindow);
            float frqStep = (1000000.0/sgy->getSampleInterval())/spectrum.size();
            spectrum.resize(spectrum.size()/2+1);
            double maxAmpl = *(std::max_element(spectrum.begin(),spectrum.end()));
            if (attrWin->getWriteSpectrum())
            {

                QFile *spectrumFile = new QFile();
                spectrumFile->setFileName(QString("%1_ffid%2_spectrum_for_attributeWindow#_%3.txt").arg(attrFilePath).arg(sgy->getFileNumFirstTrace()).arg(windows.indexOf(attrWin)+1));
                if (spectrumFile->open(QIODevice::WriteOnly|QIODevice::Text))
                {
                    QTextStream tStream(spectrumFile);
                    tStream <<"Freq, Hz \t A \t A, dB \n";
                    for (std::size_t i = 0; i<spectrum.size(); ++i)
                    {
                        tStream << QString::number(frqStep*i)<<"\t"<<QString::number(spectrum.at(i))<<"\t"<<QString::number(Aquila::dB(spectrum.at(i),maxAmpl))<<"\n";
                    }
                    spectrumFile->close();
                    delete spectrumFile;
                }
            }
            if (attrWin->getCountEnergy() || attrWin->getCountDfr())
            {
               attribute = getEnergy(spectrum,frqStep);
               if (attrWin->getCountEnergy())
               {
                   checkAttribute = attribute>attrWin->getMinEnergy() ? true:false;
                   fileAttributes.append(qMakePair(attribute,checkAttribute));
               }
               if (attrWin->getCountDfr())
               {
                   if (widthByEnergy)
                   {
                       attribute = attribute/maxAmpl;
                   }
                   else
                   {
                       attribute = getWidth(spectrum)*frqStep;
                   }
                   checkAttribute = attribute>attrWin->getMinDfr() ? true:false;
                   fileAttributes.append(qMakePair(attribute,checkAttribute));

               }

            }
        }
        tracesInWindow.clear();
    }
    foreach (QString str, relations) {
        QString tmp = str.left(str.lastIndexOf("/"));
        float a = amplitudes.value(tmp);
        tmp = str.mid(str.indexOf("/")+1,str.indexOf(">")-str.indexOf("/")-1);
        float b = amplitudes.value(tmp);
        attribute = a/b;
        tmp = str.mid(str.lastIndexOf(">")+1);
        float c = tmp.toFloat();
        checkAttribute = attribute>c ? true:false;
        fileAttributes.append(qMakePair(attribute,checkAttribute));
    }
}

bool SegyWorker::convertOneFile(const QString &filePath, const bool &writeHeaders)
{
    QString fileForWork;
    SegdFile *segd;
    SegyFile *segy;
    fileAttributes.clear();
    *logStream << QString("%1 Начало обработки файла %2\n").arg(QDateTime::currentDateTime().toString("ddd dd.MMMM.yyyy hh:mm::ss")).arg(filePath);
    emit sendInfoMessage(QTime::currentTime().toString(),Qt::black);
    if (backup)
    {
        *logStream << QString("%1 Копирование файла %2\n").arg(QDateTime::currentDateTime().toString("ddd dd.MMMM.yyyy hh:mm::ss")).arg(filePath);
        emit sendInfoMessage("Копирование файла " + filePath,Qt::black);
        fileForWork = BackupFolder +"/"+filePath.mid(filePath.lastIndexOf('/')+1);// segdFilesInDir.value(fileForConvertingNum).fileName();
        if (QFile::exists(fileForWork))
        {
            QFile::remove(fileForWork);
        }
        if (!QFile::copy(filePath,fileForWork))
        {
            backup = false;
            fileForWork = filePath;
            *logStream << QString("%1 Ошибка создания резервной копии. Копирование файлов не производится").arg(QDateTime::currentDateTime().toString("ddd dd.MMMM.yyyy hh:mm::ss"));
            emit sendSomeError("Ошибка создания резервной копии \n Копирование файлов не производится");
        }
    }
    else {
        fileForWork = filePath;
    }
    *logStream << QString("%1 Конвертация файла %2\n").arg(QDateTime::currentDateTime().toString("ddd dd.MMMM.yyyy hh:mm::ss")).arg(fileForWork);
    emit sendInfoMessage("Обработка файла " +fileForWork,Qt::black);
    if (SercelMpFactor)
    {
        segd = new SegdFile(fileForWork);
    }
    else
    {
        segd = new SegdFile(fileForWork,userMpFactorValue);
    }
    if (segd->getFileState())
    {
        segy = new SegyFile(segd,this);
        if (useExternalXps)
        {
            QQueue<QString>* templates = findTemplates(segy->getFileNumFirstTrace());
            if (!templates->isEmpty())
            {
                XFile *xF = new XFile(*templates);
                if (xF->checkTemplates())
                {
                    if (!segy->setTemplates(xF))
                    {
                        sendSomeError(QString("Несоответсвие количества трасс в XPS и SEGD файлах. Геометрия присваивается из заголовков"));
                    }
                }
                else
                {
                    sendSomeError(QString("Некорректные расстановки в XPS файле. Геометрия присваивается из заголовков"));
                }
            }
            else
            {
                emit sendSomeError(QString("Файл № %1 не найден в XPS файле. Геометрия присваивается из заголовков").arg(segy->getFileNumFirstTrace()));
            }

        }


        if (useExternalRps)
        {
            segy->setReceiverCoordinats(pp);
        }
        if (useExternalSps)
        {
            segy->setSourceCoordinats(pv);
        }
        segy->setGeometry();
        fileAttributes.append(qMakePair(segy->getFileNumFirstTrace(),true));
        fileAttributes.append(qMakePair(segy->getSP(),true));
        fileAttributes.append(qMakePair(segy->getgetShotPointNum(),true));
        fileAttributes.append(qMakePair(segy->getSourceX(0),true));
        fileAttributes.append(qMakePair(segy->getSourceY(0),true));
        fileAttributes.append(qMakePair(segy->getSourceZ(0),true));
        if (analysisAuxes)
        {
            chekingAuxData(segd);
        }
        if (checkTests)
        {
            checkingTests(segd);
        }
        delete(segd);
        if (writeHeaders)
        {
            if (writeAuxesNewFile)
            {
                segy->writeHeaders(outAuxesPath);
            }
            segy->writeHeaders(outPath);
        }
        if (writeAuxesNewFile)
        {
            segy->writeAuxTraces(outAuxesPath);
        }
        if (writeAuxes)
        {
            segy->writeAuxTraces(outPath);
        }
        segy->writeTraces(outPath,writeMutedChannels,writeMissedChannels);
        countAttributes(segy);
        attributes->append(AttributesFromFile(fileAttributes));
        emit fileConverted();
        emit sendInfoMessage(QTime::currentTime().toString(),Qt::black);
        delete segy;
        return true;
    }
    else
    {
        return false;
    }
}






