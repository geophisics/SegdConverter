#include "cstworker.h"
#include "Segd/segdfile.h"
#include "Cst/cstfile.h"
#include "aquila/functions.h"
QTXLSX_USE_NAMESPACE


CstWorker::CstWorker(QObject *parent) : BaseWorker(parent)
{
    settings = new QSettings(QCoreApplication::applicationDirPath()+QDir::separator()+"config.ini",QSettings::IniFormat,this);
    //readSettings();
}

void CstWorker::Converting()
{
    QFileInfo fInfo(segdPath);
    QDir segdDir;

    logFile = new QFile(outPath+"_log.txt");
    logFile->open(QIODevice::Text|QIODevice::WriteOnly);
    logStream = new QTextStream(logFile);
    logStream->setCodec("UTF8");
    *logStream << QString("%1 Начало конвертации\n").arg(QDateTime::currentDateTime().toString("ddd dd.MMMM.yyyy hh:mm::ss"));

    int fileForConvertingNum;

    QFile cstFile;
    cstFile.setFileName(outPath);

    if (cstFile.open(QIODevice::WriteOnly))
    {
        cstFile.close();
    }
    else {
        emit sendSomeError("Ошибка создания файла СST");
        emit finished();
    }
    if (writeAuxesNewFile)
    {
        cstFile.setFileName(outAuxesPath);
        if (cstFile.open(QIODevice::WriteOnly))
        {
            cstFile.close();
        }
        else
        {
            emit sendSomeError("Ошибка создания файла служебный трасс. Запись служебный трасс не производится");
            writeAuxesNewFile = false;
        }
    }

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
    writeXlsxHeaders();
    currentRow = 11 + windows.count();
//    FfidData segdData;
//    QString fileForWork;
    run = new bool;
    *run = true;
    while (fileForConvertingNum<segdFilesInDir.count() && *run)
    {
        if (convertOneFile(segdFilesInDir.value(fileForConvertingNum).absoluteFilePath()))
        {
            currentRow++;
            fileCount++;
            *logStream << QString("%1 Выполнена конвертация файла %2\n").arg(QDateTime::currentDateTime().toString("ddd dd.MMMM.yyyy hh:mm::ss")).arg(segdFilesInDir.value(fileForConvertingNum).fileName());
            emit sendInfoMessage("Выполнена конвертация файла "+segdFilesInDir.value(fileForConvertingNum).absoluteFilePath(),Qt::darkGreen);
        }
        else
        {
            *logStream << QString("%1 Ошибка чтения файла %2. Переход к следующему файлу ").arg(QDateTime::currentDateTime().toString("ddd dd.MMMM.yyyy hh:mm::ss")).arg(segdFilesInDir.value(fileForConvertingNum).fileName());
            emit sendSomeError(QString ("Ошибка чтения файла %1. Переход к следующему файлу").arg(segdFilesInDir.value(fileForConvertingNum).absoluteFilePath()));
        }
        if (limitMaxFiles && fileCount >= maxFilesValue)
        {
            fileCount=0;
            outPath.insert(outPath.lastIndexOf('/')+1,'_');
            outAuxesPath.insert(outAuxesPath.lastIndexOf('/')+1,'_');
            createFileForMissedTraces();
        }
        fileForConvertingNum++;
    }
    saveXlsxFile();
    *logStream << QString("%1 Завершение конвертации\n").arg(QDateTime::currentDateTime().toString("ddd dd.MMMM.yyyy hh:mm:ss"));
    delete logStream;
    logFile->close();
    delete logFile;

    emit finished();
    //segyFile->close();
}

void CstWorker::countAttributes(CstFile *cst, const int currentRow)
{
    QVector<QVector<float> > tracesInWindow;
    QList<AttributeWindow*>::iterator windowsIterator= windows.begin();
    QMap<QString,float> amplitudes;
    int currentColumn = 7;
    int windowCount=0;
    //float ampl;
    //int currentRow = windows.count()+11;
    AttributeWindow * attrWin;
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
                tracesInWindow = cst->getDataInWindow(logStream,attrWin ->getMinOffset(),attrWin ->getMaxOffset(),attrWin ->getMinTime(),attrWin ->getMaxTime(),notUseMutedTraces,badTests,minAmpl,exclPoints);
            }
            else
            {
                tracesInWindow = cst->getDataInWindow(logStream,attrWin ->getMinOffset(),attrWin ->getMaxOffset(),attrWin ->getMinTime(),attrWin ->getMaxTime(),notUseMutedTraces,badTests,minAmpl,exclusions);
            }
        }
        else {
            tracesInWindow = cst->getDataInWindow(logStream,attrWin ->getMinOffset(),attrWin ->getMaxOffset(),attrWin ->getMinTime(),attrWin ->getMaxTime(),notUseMutedTraces,badTests,minAmpl);
        }
        if (attrWin->getCountAmpl() )
        {
            attributes->ampl = getAbsAvg(tracesInWindow);
            if (attributes->ampl < attrWin->getMinAmpl())
            {
                attributes->correctAmpl = false;
                xlsxFormat.setPatternBackgroundColor(Qt::red);
            }
            //attributes->ampl = ampl;
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
            //attributes->rms = ampl;
            xlsx.write(currentRow,currentColumn,attributes->rms,xlsxFormat);
            xlsxFormat.setPatternBackgroundColor(Qt::white);
            amplitudes.insert(QString("R%1").arg(windowCount),attributes->rms);
            currentColumn++;
        }
        if (attrWin->getCountFreq())
        {
            attributes->freq = countFreq(tracesInWindow,cst->getSampleRate());
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
            float frqStep = (1000000.0/cst->getSampleRate())/spectrum.size();
            spectrum.resize(spectrum.size()/2+1);
            double maxAmpl = *(std::max_element(spectrum.begin(),spectrum.end()));
            if (attrWin->getWriteSpectrum())
            {

                QFile *spectrumFile = new QFile();
                spectrumFile->setFileName(QString("%1_ffid%2_spectrum_for_attributeWindow#_%3.txt").arg(attrFilePath).arg(cst->getFfid()).arg(windows.indexOf(attrWin)+1));
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
        //if (relation<
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

bool CstWorker::convertOneFile(const QString &filePath)
{
    QString fileForWork;
    FfidData segdData;
    SegdFile *segd;
    CstFile *cst;
    *logStream << QString("%1 Начало обработки файла %2\n").arg(QDateTime::currentDateTime().toString("ddd dd.MMMM.yyyy hh:mm::ss")).arg(filePath);
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
        cst = new CstFile(segd,this);

        if (useExternalXps)
        {
            QQueue<QString>* templates = findTemplates(cst->getFfid());
            if (!templates->isEmpty())
            {
                XFile *xF = new XFile(*templates);
                if (xF->checkTemplates())
                {
                    if (!cst->setTemplates(xF))
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
                emit sendSomeError(QString("Файл № %1 не найден в XPS файле. Геометрия присваивается из заголовков").arg(cst->getFfid()));
            }

        }


        if (useExternalRps)
        {
            cst->setReceiverCoordinats(pp);
        }
        if (useExternalSps)
        {
            cst->setSourceCoordinats(pv);
        }
        cst->setGeometry();
        segdData.ffid = cst->getFfid();
        segdData.line = cst->getLine();
        segdData.source = cst->getPoint();
        segdData.X = cst->getEasting();
        segdData.Y = cst->getNorthing();
        segdData.Z = cst->getElevation();
        emit sendSegdData(segdData);
        if (checkTests)
        {
            checkingTests(segd);
        }
        if (analysisAuxes)
        {
            chekingAuxData(segd);
        }
        delete segd;
        if (writeAuxesNewFile)
        {
            cst->writeAuxTraces(outAuxesPath);
        }
        if (writeAuxes)
        {
            cst->writeAuxTraces(outPath);
        }
        cst->writeTraces(outPath,writeMutedChannels,writeMissedChannels);
        xlsx.write(currentRow,1,segdData.ffid,xlsxFormat);
        xlsx.write(currentRow,2,segdData.line,xlsxFormat);
        xlsx.write(currentRow,3,segdData.source,xlsxFormat);
        xlsx.write(currentRow,4,segdData.X,xlsxFormat);
        xlsx.write(currentRow,5,segdData.Y,xlsxFormat);
        xlsx.write(currentRow,6,segdData.Z,xlsxFormat);
        /*if (useExclusions)
        {
            QStringList *missed = new QStringList(cst->deleteTracesInExclusions(exclusions));
            if (saveExclReport)
            {
                QFile* missedTraces = new QFile(outPath+"_missedTraces.txt");
                QTextStream *missedStream = new QTextStream(missedTraces);
                for (int i=0;i<missed->count();++i)
                {
                    *missedStream<<missed->value(i);
                }
                missedTraces->close();
                delete missedTraces;
                delete missedStream;
            }
            missed->clear();
            delete missed;
        }*/
        countAttributes(cst,currentRow);
        delete cst;
        return true;
    }
    else
    {
        return false;
    }
}


