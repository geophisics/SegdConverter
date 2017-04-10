#include "cstworker.h"
#include "Segd/segdfile.h"
#include "Cst/cstfile.h"
#include "aquila/functions.h"

void CstWorker::Converting()
{
    QFileInfo fInfo(paths.value("SegdPath"));
    QDir segdDir;

    messaging(" Начало конвертации");
    fileForConvertingNum =0; // порядковый номер в директории конвертируемого файла
    fileCount = 0; // количество ф.н. в сводном файле
    QFileInfoList segdFilesInDir;
    segdDir = fInfo.dir();
    segdFilesInDir = segdDir.entryInfoList(QStringList()<<"*.segd",QDir::Files,QDir::Name);
    if (rMode==readMode::fromFile) {
        fileForConvertingNum = segdFilesInDir.indexOf(fInfo);
    }
    if (fileForConvertingNum==-1)
    {
        messaging(QString(" Не найден указанный путь ").append(paths.value("SegdPath")),Qt::red);
        stopRunning();
        return;
    }

    int numOfFilesInDir = segdFilesInDir.count();
    if (online) {
        numOfFilesInDir--;
    }

    while (fileForConvertingNum<numOfFilesInDir && *p_running)
    {
        if (convertOneFile(segdFilesInDir.value(fileForConvertingNum).absoluteFilePath()))
        {
            fileCount++;

            messaging(QString(" Выполнена конвертация файла ").append(segdFilesInDir.value(fileForConvertingNum).absoluteFilePath()),Qt::darkGreen);
        }
        else {
            messaging(QString (" Ошибка чтения файла %1. Переход к следующему файлу").arg(segdFilesInDir.value(fileForConvertingNum).fileName()),Qt::red);
        }
        if (limitMaxFiles && fileCount >= maxFilesValue)
        {
            fileCount=0;
            QString newPath = paths.value("OutPath");
            newPath.insert(newPath.lastIndexOf(QDir::separator())+1,'_');
            paths.insert("OutPath",QString(newPath));
            newPath = paths.value("AuxPath");
            newPath.insert(newPath.lastIndexOf(QDir::separator())+1,'_');
            paths.insert("AuxPath",QString(newPath));
        }
        if (online){
            segdFilesInDir = segdDir.entryInfoList(QStringList()<<"*.segd",QDir::Files,QDir::Name);
            numOfFilesInDir = segdFilesInDir.count()-1;
        }
        fileForConvertingNum++;
    }
    if (*p_running && online)
    {
        watcher  = new QFileSystemWatcher(this);
        watcher->addPath(segdDir.absolutePath());
        connect(watcher,SIGNAL(directoryChanged(QString)),this,SLOT(segdDirChanged(QString)),Qt::DirectConnection);
        segdDirChanged(segdDir.absolutePath());
    }
    else
    {
         stopRunning();
    }
}

void CstWorker::countAttributes(CstFile *cst)
{
    QVector<QVector<float> > tracesInWindow;
    QMap<QString,float> amplitudes;

    for (int i=0; i<windows.count();++i)
    {
        logStream << QString("%1 Расчет атрибутов в окне Min Offset = %2; Max Offset = %3; Min Time = %4мс; Max Time = %5мс\n").arg(QDateTime::currentDateTime().toString("ddd dd.MMMM.yyyy hh:mm::ss")).arg(windows.at(i).minOffset).arg(windows.at(i).maxOffset).arg(windows.at(i).minTime).arg(windows.at(i).maxTime);
        switch (exType) {
        case exclusionType::txtExcl:
            tracesInWindow = cst->getDataInWindow(&logStream,windows.at(i).minOffset,windows.at(i).maxOffset,windows.at(i).minTime,windows.at(i).maxTime,notUseMutedTraces,badTests,minAmpl,exclPoints);
            break;
        case exclusionType::mesaExcl:
            tracesInWindow = cst->getDataInWindow(&logStream,windows.at(i).minOffset,windows.at(i).maxOffset,windows.at(i).minTime,windows.at(i).maxTime,notUseMutedTraces,badTests,minAmpl,exclusions);
            break;
        default:
            tracesInWindow = cst->getDataInWindow(&logStream,windows.at(i).minOffset,windows.at(i).maxOffset,windows.at(i).minTime,windows.at(i).maxTime,notUseMutedTraces,badTests,minAmpl);
            break;
        }
        countAttributesInWindow(tracesInWindow,i,cst->getSampleRate(),cst->getFfid(),&amplitudes);
        tracesInWindow.clear();
    }
    countRelations(amplitudes);
}

bool CstWorker::convertOneFile(const QString &filePath)
{
    QString fileForWork;
    SegdFile *segd;
    CstFile *cst;
    fileAttributes.clear();
    messaging(QString(" Обработка файла ").append(filePath));
    if (backup)
    {
        messaging(QString(" Копирование файла ").append(filePath));
        fileForWork = paths.value("BackupFolder") +"/"+filePath.mid(filePath.lastIndexOf('/')+1);// segdFilesInDir.value(fileForConvertingNum).fileName();
        if (QFile::exists(fileForWork))
        {
            QFile::remove(fileForWork);
        }
        if (!QFile::copy(filePath,fileForWork))
        {
            backup = false;
            fileForWork = filePath;
            messaging(QString(" Ошибка создания резервной копии. Копирование файлов не производится"),Qt::red);
        }
    }
    else {
        fileForWork = filePath;
    }
    messaging(QString(" Конвертация файла ").append(fileForWork));
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
       if (!paths.value("XpsPath").isEmpty())
      {
            QQueue<QString> templates = findTemplates(cst->getFfid());
            if (!templates.isEmpty())
            {
                XFile xF(templates);// = new XFile(*templates);
                if (xF.checkTemplates())
                {
                    if (!cst->setTemplates(&xF))
                    {
                        messaging(QString(" Несоответсвие количества трасс в XPS и SEGD файлах. Геометрия присваивается из заголовков"),Qt::red);
                    }
                }
                else
                {
                    messaging(QString(" Некорректные расстановки в XPS файле. Геометрия присваивается из заголовков"),Qt::red);
                }
            }
            else
            {
                messaging(QString(" Файл № %1 не найден в XPS файле. Геометрия присваивается из заголовков").arg(cst->getFfid()),Qt::red);
            }

        }
        if (!pp.isEmpty())
        {
            messaging(" Редакция координат ПП");
            int receivers = cst->setReceiverCoordinats(&logStream,pp);
            if (receivers>0){
                messaging(QString(" В R-файле не содержатся координаты для %1 ПП").arg(receivers),Qt::red);
            }
        }
        if (!pv.isEmpty())
        {
            messaging(" Редакция координат ПВ");
            if(!cst->setSourceCoordinats(pv)) {
                 messaging(QString(" В sps файле не содержатся координаты для ПВ %1_%2. Координаты взяты из заголовков").arg(cst->getLine()).arg(cst->getPoint()),Qt::red);
            }
        }
        cst->setGeometry();
        fileAttributes.append(qMakePair(cst->getFfid(),true));
        fileAttributes.append(qMakePair(cst->getLine(),true));
        fileAttributes.append(qMakePair(cst->getPoint(),true));
        fileAttributes.append(qMakePair(cst->getEasting(),true));
        fileAttributes.append(qMakePair(cst->getNorthing(),true));
        fileAttributes.append(qMakePair(cst->getElevation(),true));
        if (analysisAuxes)
        {
            chekingAuxData(segd);
        }
        if (checkTests)
        {
            checkingTests(segd);
        }
        delete segd;
        switch (auxMode) {
        case writeAuxesMode::writeInNewFile:
            cst->writeAuxTraces(paths.value("AuxPath"));
            break;
        case writeAuxesMode::write:
            cst->writeAuxTraces(paths.value("OutPath"));
            break;
        default:
            break;
        }
 
        cst->writeTraces(paths.value("OutPath"),writeMutedChannels,writeMissedChannels);
        countAttributes(cst);
        attributes->append(AttributesFromFile(fileAttributes));
        emit attributesCounted();
        delete cst;
        return true;
    }
    else
    {
        return false;
    }
}

void CstWorker::segdDirChanged(QString string)
{
       QDir segdDir(string);
       QFileInfoList segdFilesInDir;
       QStringList filter;
       filter << "*.segd";
       segdFilesInDir = segdDir.entryInfoList(filter,QDir::Files,QDir::Name);
       int attempt;// = 0;
       for (;fileForConvertingNum<segdFilesInDir.count();fileForConvertingNum++)
       {
           messaging(QString(" Обнаружен файл ").append(segdFilesInDir.value(fileForConvertingNum).fileName()),Qt::blue);
           attempt=0;

           while(!convertOneFile(segdFilesInDir.value(fileForConvertingNum).absoluteFilePath()) && attempt<10 && *p_running) {
               attempt++;
               messaging(QString(" Ошибка доступа к файлу. Ожидание"),Qt::red);
               thread()->sleep(waitingTime);
           }
           if (attempt<10)
           {
               fileCount++;
               messaging(QString(" Выполнена конвертация файла ").append(segdFilesInDir.value(fileForConvertingNum).fileName()),Qt::darkGreen);
           }
           else {
               messaging(QString(" Превышено время ожидания файла ").append(segdFilesInDir.value(fileForConvertingNum).fileName()),Qt::red);
           }
           if (*p_running)
           {
               segdFilesInDir = segdDir.entryInfoList(filter,QDir::Files,QDir::Name);
           }
           else
           {
               stopRunning();
               break;
           }
       }
}
