#include "segyworker.h"
#include <QFileInfo>
#include <QDir>
#include <Segd/segdfile.h>
#include <Segy/segyfile.h>
#include <QtXlsx>
#include <QTextStream>
#include "segdconverterwindow.h"
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
    //format.setHorizontalAlignment(Format::AlignHCenter);
    writeXlsxHeaders();
    currentRow = 11 + windows.count();
    //FfidData segdData;

    writeFileHeaders = true;
    run = new bool;
    *run=true;
    while (fileForConvertingNum<segdFilesInDir.count() && *run)
    {
        currentColumn=7;
        if (convertOneFile(segdFilesInDir.value(fileForConvertingNum).absoluteFilePath(),writeFileHeaders))
        {
            currentRow++;
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
    saveXlsxFile();
    *logStream << QString("%1 Завершение конвертации\n").arg(QDateTime::currentDateTime().toString("ddd dd.MMMM.yyyy hh:mm::ss"));
    delete logStream;
    logFile->close();
    delete logFile;
    emit finished();
}

//прописываем заголовки в файл xlsx

void SegyWorker::countAttributes(SegyFile *sgy)
{
    QVector<QVector<float> > tracesInWindow;
    QList<AttributeWindow*>::iterator windowsIterator= windows.begin();
    QMap<QString,float> amplitudes;

    //int currentColumn = 8;

    int windowCount=0;
    AttributeWindow *attrWin;
    SeisAttributes *attributes;
    for(;windowsIterator!=windows.end(); ++ windowsIterator )
    {

        attributes = new SeisAttributes();
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
        //tracesInWindow = sgy->getDataInWindow(attrWin ->getMinOffset(),attrWin ->getMaxOffset(),attrWin ->getMinTime(),attrWin ->getMaxTime(),320,320,notUseMutedTraces);
        if (attrWin->getCountAmpl() )
        {
            attributes->ampl = getAbsAvg(tracesInWindow);
            if (attributes->ampl < attrWin->getMinAmpl())
            {
                attributes->correctAmpl = false;
                xlsxFormat.setPatternBackgroundColor(Qt::red);
            }
            xlsx.write(currentRow,currentColumn,attributes->ampl,xlsxFormat);
            xlsxFormat.setPatternBackgroundColor(Qt::white);
            amplitudes.insert(QString("A%1").arg(windowCount),attributes->ampl);
            currentColumn++;
        }
        if (attrWin->getCountRms())
        {
            attributes->rms = getRms(tracesInWindow);
            if (attributes->rms < attrWin->getMinRms())
            {
                attributes->correctRms = false;
                xlsxFormat.setPatternBackgroundColor(Qt::red);
            }
            xlsx.write(currentRow,currentColumn,attributes->rms,xlsxFormat);
            xlsxFormat.setPatternBackgroundColor(Qt::white);
            amplitudes.insert(QString("R%1").arg(windowCount),attributes->rms);
            currentColumn++;
        }
        if (attrWin->getCountFreq())
        {
            attributes->freq = countFreq(tracesInWindow,sgy->getSampleInterval());
            if (attributes->freq < attrWin->getMinFreq())
            {
                attributes->correctFreq = false;
                xlsxFormat.setPatternBackgroundColor(Qt::red);
            }
            xlsx.write(currentRow,currentColumn,attributes->freq,xlsxFormat);
            xlsxFormat.setPatternBackgroundColor(Qt::white);
            currentColumn++;
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
            attributes->energy = getEnergy(spectrum,frqStep);

            if (attrWin ->getCountEnergy())
            {
                if (attributes->energy < attrWin->getMinEnergy())
                {
                    attributes->correctEnergy = false;
                    xlsxFormat.setPatternBackgroundColor(Qt::red);
                }
                xlsx.write(currentRow,currentColumn,attributes->energy,xlsxFormat);
                xlsxFormat.setPatternBackgroundColor(Qt::white);
                currentColumn++;
            }

            if (attrWin->getCountDfr())
            {
                if (widthByEnergy)
                {
                    attributes->dfr = attributes->energy/maxAmpl;
                }
                else
                {
                    attributes->dfr = getWidth(spectrum)*frqStep;
                }
                if (attributes->dfr < attrWin->getMinDfr())
                {
                    attributes->correctDfr = false;
                    xlsxFormat.setPatternBackgroundColor(Qt::red);
                }
                xlsx.write(currentRow,currentColumn,attributes->dfr,xlsxFormat);
                xlsxFormat.setPatternBackgroundColor(Qt::white);
                currentColumn++;
            }
        }
        tracesInWindow.clear();
        emit sendSeisAttributes(attributes,windowCount);
    }
    currentColumn++;
    float relation;
    foreach (QString str, relations) {
        QString tmp = str.left(str.lastIndexOf("/"));
        float a = amplitudes.value(tmp);
        tmp = str.mid(str.indexOf("/")+1,str.indexOf(">")-str.indexOf("/")-1);
        float b = amplitudes.value(tmp);
        relation = a/b;
        tmp = str.mid(str.lastIndexOf(">")+1);
        float c = tmp.toFloat();
        if (relation<c)
        {
            xlsxFormat.setPatternBackgroundColor(Qt::red);
            emit sendRelation(str.left(str.lastIndexOf(">")),a/b,false);
        }
        else
        {
            emit sendRelation(str.left(str.lastIndexOf(">")),a/b,true);
        }
        xlsx.write(currentRow,currentColumn,relation,xlsxFormat);

        xlsxFormat.setPatternBackgroundColor(Qt::white);
        currentColumn++;
    }
}

bool SegyWorker::convertOneFile(const QString &filePath, const bool &writeHeaders)
{
    QString fileForWork;
    FfidData segdData;
    SegdFile *segd;
    SegyFile *segy;
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
        segdData.ffid = segy->getFileNumFirstTrace();
        segdData.line = segy->getSP();
        segdData.source = segy->getgetShotPointNum();
        segdData.X = segy->getSourceX(0);
        segdData.Y = segy->getSourceY(0);
        segdData.Z = segy->getSourceZ(0);
        emit sendSegdData(segdData);
        if (checkTests)
        {
            checkingTests(segd);
            currentColumn++;
        }
        if (analysisAuxes)
        {
            chekingAuxData(segd);
            currentColumn++;
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
        xlsx.write(currentRow,1,segy->getFileNumFirstTrace(),xlsxFormat);
        xlsx.write(currentRow,2,segy->getSP(),xlsxFormat);
        xlsx.write(currentRow,3,segy->getgetShotPointNum(),xlsxFormat);
        xlsx.write(currentRow,4,segy->getSourceX(0),xlsxFormat);
        xlsx.write(currentRow,5,segy->getSourceY(0),xlsxFormat);
        xlsx.write(currentRow,6,segy->getSourceZ(0),xlsxFormat);
        /*if (useExclusions)
        {
            if (saveExclReport)
            {
                QStringList *missed;
                if (exType == exclusionType::mesaExcl)
                {
                    missed = new QStringList(segy->deleteFileInExclusions(exclusions));
                }
                else
                {
                    missed = new QStringList(segy->deleteReceiversInExclusions(exclPoints));
                }
                QFile* missedTraces = new QFile(outPath+"_missedTraces.txt");
                if (missedTraces->open(QIODevice::Append|QIODevice::Text))
                {
                    QTextStream *missedStream = new QTextStream(missedTraces);

                    for (int i=0;i<missed->count();++i)
                    {
                        *missedStream<<missed->value(i);
                    }
                    missed->clear();
                    missedTraces->close();
                    delete missedTraces;
                    delete missedStream;
                }
                else
                {
                    emit sendSomeError(QString("Ошибка открытия файла для записи отчета эксклюзивных зон \n"
                                               "Сохранение отчета экслюзивных зон не производится"));
                    saveExclReport = false;
                }

            }
            else
            {
                if (exType == exclusionType::mesaExcl)
                {
                    segy->deleteFileInExclusions(exclusions);
                }
                else
                {
                    segy->deleteReceiversInExclusions(exclPoints);
                }
            }
        }*/
        countAttributes(segy);
        emit sendInfoMessage(QTime::currentTime().toString(),Qt::black);
        delete segy;
        return true;
    }
    else
    {
        return false;
    }
}






