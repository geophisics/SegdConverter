#include "segyworkeronline.h"
#include "Segd/segdfile.h"
#include "Segy/segyfile.h"


SegyWorkerOnline::SegyWorkerOnline(QObject *parent) : SegyWorker(parent)
{
    settings = new QSettings(QCoreApplication::applicationDirPath()+QDir::separator()+"config.ini",QSettings::IniFormat,this);
    readSettings();

    watcher  = new QFileSystemWatcher(this);
    connect(watcher,SIGNAL(directoryChanged(QString)),this,SLOT(segdDirChanged(QString)),Qt::DirectConnection);
}

void SegyWorkerOnline::Converting()
{
    QFileInfo fInfo(segdPath);
    QDir segdDir;

    logFile = new QFile(outPath+"_log.txt");
    logFile->open(QIODevice::Text|QIODevice::WriteOnly);
    logStream = new QTextStream(logFile);
    logStream->setCodec("UTF8");
    *logStream << QString("%1 Начало конвертации\n").arg(QDateTime::currentDateTime().toString("ddd dd.MMMM.yyyy hh:mm::ss"));

    fileCount = 0; // счетчик файлов в сводном файле
    currentRow = 11 + windows.count(); //текущая ячейка в таблице xlsx

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

    watcher->addPath(segdDir.absolutePath());
    // количество ф.н. в сводном файле
    writeXlsxHeaders();

    //FfidData segdData;
    writeFileHeaders = true;
    run = new bool;
    *run=true;
    //QString fileForConverting;
    if (segdFilesInDir.isEmpty())
    {
        return;
    }
    while (fileForConvertingNum<segdFilesInDir.count()-1 && *run)
    {

        //if (backup)
        //{
        //    fileForConverting =
        //}




        if (convertOneFile(segdFilesInDir.value(fileForConvertingNum).absoluteFilePath(),writeFileHeaders))
        {
            currentRow++;
            writeFileHeaders = false;
            fileCount++;
            *logStream << QString("%1 Выполнена конвертация файла %2\n").arg(QDateTime::currentDateTime().toString("ddd dd.MMMM.yyyy hh:mm::ss")).arg(segdFilesInDir.value(fileForConvertingNum).fileName());
            emit sendInfoMessage("Выполнена конвертация файла" + segdFilesInDir.value(fileForConvertingNum).absoluteFilePath(),Qt::darkGreen);

        }
        else {
            *logStream << QString("%1 Ошибка чтения файла %2. Переход к следующему файлу ").arg(QDateTime::currentDateTime().toString("ddd dd.MMMM.yyyy hh:mm::ss")).arg(segdFilesInDir.value(fileForConvertingNum).fileName());
            emit sendSomeError(QString ("Ошибка чтения файла %1. Переход к следующему файлу").arg(segdFilesInDir.value(fileForConvertingNum).absoluteFilePath()));
        }
        if (limitMaxFiles && fileCount >= maxFilesValue)
        {
            fileCount=0;
            outPath.insert(outPath.lastIndexOf('/')+1,'_');
            outAuxesPath.insert(outAuxesPath.lastIndexOf('/')+1,'_');
            writeFileHeaders = true;
            createFileForMissedTraces();
        }
        segdFilesInDir = segdDir.entryInfoList(filter,QDir::Files,QDir::Name);
        fileForConvertingNum++;
    }
    if (!(*run))
    {
        saveXlsxFile();
        *logStream << QString("%1 Завершение конвертации\n").arg(QDateTime::currentDateTime().toString("ddd dd.MMMM.yyyy hh:mm::ss"));
        delete logStream;
        logFile->close();
        delete logFile;
        emit finished();
    }
    else
    {
        int w = 0;
        while (!convertOneFileOnline(segdFilesInDir.value(fileForConvertingNum).absoluteFilePath(),writeFileHeaders) && w<10)
        {
            thread()->sleep(waitingTime);
            w++;
        }
        if (w<10)
        {
            *logStream << QString("%1 Выполнена конвертация файла %2\n").arg(QDateTime::currentDateTime().toString("ddd dd.MMMM.yyyy hh:mm::ss")).arg(segdFilesInDir.value(fileForConvertingNum).fileName());
            emit sendInfoMessage("Выполнена конвертация файла " + segdFilesInDir.value(fileForConvertingNum).absoluteFilePath(),Qt::darkGreen);
        }
        else
        {
            *logStream << QString("%1 Ошибка чтения файла %2. Переход к следующему файлу ").arg(QDateTime::currentDateTime().toString("ddd dd.MMMM.yyyy hh:mm::ss")).arg(segdFilesInDir.value(fileForConvertingNum).fileName());
            emit sendSomeError("Ошибка чтения файла "+segdFilesInDir.value(fileForConvertingNum).absoluteFilePath() + " Файл пропущен");
        }
        fileForConvertingNum++;
        if (!(*run))
        {
            saveXlsxFile();
            *logStream << QString("%1 Завершение конвертации\n").arg(QDateTime::currentDateTime().toString("ddd dd.MMMM.yyyy hh:mm::ss"));
            delete logStream;
            logFile->close();
            delete logFile;
            emit finished();
        }
        else
        {
            *run=false;
        }
    }
}



bool SegyWorkerOnline:: convertOneFileOnline(const QString &filePath, const bool &writeHeaders)
{
    QString fileForWork;
    FfidData segdData;
    SegdFile *segd;
    SegyFile *segy;
    *logStream << QString("%1 Начало обработки файла %2\n").arg(QDateTime::currentDateTime().toString("ddd dd.MMMM.yyyy hh:mm::ss")).arg(filePath);
    if (backup)
    {

        //QString tmp = filePath.mid(filePath.lastIndexOf('/')+1);
        //QString tmp = filePath.right(filePath.lastIndexOf('/')+1);
        *logStream << QString("%1 Копирование файла %2\n").arg(QDateTime::currentDateTime().toString("ddd dd.MMMM.yyyy hh:mm::ss")).arg(filePath);

        emit sendInfoMessage("Копирование файла " + filePath,Qt::black);
        fileForWork = BackupFolder +"/"+filePath.mid(filePath.lastIndexOf('/')+1);// segdFilesInDir.value(fileForConvertingNum).fileName();
        if (QFile::exists(fileForWork))
        {
            QFile::remove(fileForWork);
        }
        if (!QFile::copy(filePath,fileForWork))
        {
            *logStream << QString("%1 Ошибка создания резервной копии. Копирование файлов не производится").arg(QDateTime::currentDateTime().toString("ddd dd.MMMM.yyyy hh:mm::ss"));
            emit sendSomeError("Ошибка копирования");
            return false;
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
        delete(segd);
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
        }
        if (analysisAuxes)
        {
            chekingAuxData(segd);
        }
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
        segy->writeTraces(outPath,writeMutedChannels);

        xlsx.write(currentRow,1,segy->getFileNumFirstTrace(),xlsxFormat);
        xlsx.write(currentRow,2,segy->getSP(),xlsxFormat);
        xlsx.write(currentRow,3,segy->getgetShotPointNum(),xlsxFormat);
        xlsx.write(currentRow,4,segy->getSourceX(0),xlsxFormat);
        xlsx.write(currentRow,5,segy->getSourceY(0),xlsxFormat);
        xlsx.write(currentRow,6,segy->getSourceZ(0),xlsxFormat);
        countAttributes(segy);
        delete segy;
        return true;
    }
    else
    {
        return false;
    }
}




void SegyWorkerOnline::segdDirChanged(QString string)
{
   QDir segdDir(string);
   int w;
   QFileInfoList segdFilesInDir;
   QStringList filter;
   filter << "*.segd";
   segdFilesInDir = segdDir.entryInfoList(filter,QDir::Files,QDir::Name);
   *run = true;
   if (fileForConvertingNum < segdFilesInDir.count())
   {
       *logStream << QString("Обнаружен файл %1").arg(segdFilesInDir.value(fileForConvertingNum).fileName());
       emit sendInfoMessage("Обнаружен файл " + segdFilesInDir.value(fileForConvertingNum).fileName(),Qt::blue);
   }
   while (fileForConvertingNum<segdFilesInDir.count() && *run)
   {
       w = 0;
       while (!convertOneFileOnline(segdFilesInDir.value(fileForConvertingNum).absoluteFilePath(),writeFileHeaders) && w<10)
       {
           w++;
           *logStream << "Ошибка доступа к файлу. Ожидание.";
           emit sendSomeError("Ошибка доступа к файлу. Ожидание.");
           thread()->sleep(waitingTime);
       }

       if (w<10)
       {
            currentRow++;
            writeFileHeaders = false;
            fileCount++;
            *logStream << QString("%1 Выполнена конвертация файла %2\n").arg(QDateTime::currentDateTime().toString("ddd dd.MMMM.yyyy hh:mm::ss")).arg(segdFilesInDir.value(fileForConvertingNum).fileName());
            emit sendInfoMessage(QString("Выполнена конвертация файла %1").arg(segdFilesInDir.value(fileForConvertingNum).absoluteFilePath()),Qt::darkGreen);

       }
       else {
           *logStream << QString("%1 Превышено время ожидания файла %2\n").arg(QDateTime::currentDateTime().toString("ddd dd.MMMM.yyyy hh:mm::ss")).arg(segdFilesInDir.value(fileForConvertingNum).fileName());
           emit sendSomeError(QString ("Превышено врямя ожидания файла %1. Переход к следующему файлу").arg(segdFilesInDir.value(fileForConvertingNum).fileName()));
       }
       fileForConvertingNum++;
       if (limitMaxFiles && fileCount >= maxFilesValue)
       {
           fileCount=0;
           outPath.insert(outPath.lastIndexOf('/')+1,'_');
           outAuxesPath.insert(outAuxesPath.lastIndexOf('/')+1,'_');
           writeFileHeaders = true;
           createFileForMissedTraces();
       }
       //segdFilesInDir = segdDir.entryInfoList(filter,QDir::Files,QDir::Name);
   }
   if (!(*run))
   {
       saveXlsxFile();
       *logStream << QString("%1 Завершение конвертации\n").arg(QDateTime::currentDateTime().toString("ddd dd.MMMM.yyyy hh:mm::ss"));
       delete logStream;
       logFile->close();
       delete logFile;
       emit finished();
   }
   else
   {
       *run=false;
   }
   //emit sendInfoMessage(QString::number(waitingTime));
}