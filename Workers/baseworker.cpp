#include "baseworker.h"
#include "QQueue"
#include "aquila/transform/FftFactory.h"
#include "aquila/functions.h"
#include <algorithm>
#include <functional>
#include <memory>
#include <complex>
#include <cmath>
#include <Segd/segdfile.h>


QT_CHARTS_USE_NAMESPACE

BaseWorker::BaseWorker(CountedAttributes *attr):attributes(attr)
{
    settings = new QSettings(QCoreApplication::applicationDirPath()+QDir::separator()+"config.ini",QSettings::IniFormat,this);
}

// устанавливаем путь до файлов segd
void BaseWorker::setSegdPath(const QString &path)
{
    segdPath = path;
}

// устанавливаем куда будем писать выходной файл
void BaseWorker::setOutPath(const QString &path)
{
    outPath = path;
    QString tmp=path;
    outAuxesPath = tmp.insert(tmp.lastIndexOf('.'),"_auxes");
}

//устанавливаем куда будем писать файл атрибутов
void BaseWorker::setAttrFilePath(const QString &path)
{
    attrFilePath = path;
}

//устанавливаем откуда будем читать r-файл
void BaseWorker::setRpsPath(const QString &path)
{
    rpsPath = path;
}

//устанавливаем откуда будем читать s-файл
void BaseWorker::setSpsPath(const QString &path)
{
    spsPath = path;
}

void BaseWorker::setXpsPath(const QString &path)
{
    xpsPath = path;
}

//чтение настроек конвертации
void BaseWorker::readSettings()
{
    settings->beginGroup("/ConvertSettings");
    SercelMpFactor = settings->value("/UseHeaderMpFactor",1).toBool();
    analysisAuxes = settings->value("/AnalisysAuxes",false).toBool();

    checkTests = settings->value("/CheckTests",false).toBool();
    testsPercent = settings->value("/TestPercent",2).toInt();

    if (!SercelMpFactor)
    {
        if (settings->value("/NotUseMpFactor",true).toBool())
        {
            userMpFactorValue = 1;
        }
        else
        {
            QString mpStr = settings->value("/UserMpFactorValue","1").toString();
            mpStr = mpStr.replace(',','.');
            userMpFactorValue = mpStr.toFloat();
        }
    }
    waitingTime = settings->value("/WaitingTime",10).toInt(0);
    useExclusions = settings->value("/Exclusions",false).toBool();

    if (settings->value("/TxtExclusion",false).toBool())
    {
        exType = exclusionType::txtExcl;
    }
    else
    {
        exType = exclusionType::mesaExcl;
    }

    if (useExclusions)
    {
        QString exclPath =settings->value("/ExclusionsPath","").toString();
        if (exType ==exclusionType::mesaExcl)
        {
            setExclusions(exclPath);
        }
        else
        {
            setReceiversInExclusions(exclPath);
        }
    }
    writeMutedChannels = settings->value("/MuteChannels",true).toBool();
    writeMissedChannels = settings->value("/MissedChannels",false).toBool();
    writeAuxes=(settings->value("/WriteAuxes",0).toBool());
    writeAuxesNewFile = settings->value("/WriteAuxesInNewFile",0).toBool();
    backup = settings->value("/Backup",false).toBool();
    if (backup)
    {

        BackupFolder = settings->value("/BackupFolder","").toString();
        QDir backupSegdPath(BackupFolder);
        if (!backupSegdPath.exists())
        {
            bool succes = backupSegdPath.mkdir(BackupFolder);
            qDebug()<<succes;
            if (!succes)
            {
                backup=false;
                emit sendSomeError(QString("Ошибка создания директории %1. Создание резервной копии файлов segd не выполняется").arg(BackupFolder));
            }
        }
    }
    limitMaxFiles = settings->value("/MaxFiles",false).toBool();
    if (limitMaxFiles)
    {
        maxFilesValue = settings->value("/MaxFilesValue",9999).toInt();
    }
    settings->endGroup();
    settings->beginGroup("/WindowsSettings");

    notUseMutedTraces = settings->value("/MutedTraces",true).toBool();
    badTests = settings->value("/CheckTests",false).toBool();
    minAmpl = settings->value("/MinAmplValue",0.0).toFloat();


    int size = settings->beginReadArray("/Windows");
    AttributeWindow window;
    for (int i=0; i<size; ++i)
    {
        settings->setArrayIndex(i);
        window.minTime=settings->value("/MinTime","0").toInt();
        window.maxTime=settings->value("/MaxTime","0").toInt();
        window.minOffset=settings->value("/MinOffset","0").toInt();
        window.maxOffset=settings->value("/MaxOffset","0").toInt();
        window.minAmpl=settings->value("/MinAmpl","0.0").toFloat();
        window.minRms=settings->value("/MinRms","0.0").toFloat();
        window.minFreq=settings->value("/MinFreq","0.0").toFloat();
        window.minDfr=settings->value("/MinDfr","0.0").toFloat();
        window.minEnergy=settings->value("/MinEnergy","0.0").toFloat();
        window.countAmpl=settings->value("/CountAmpl",false).toBool();
        window.countRms=settings->value("/CountRms",false).toBool();
        window.countFreq=settings->value("/CountFreq",false).toBool();
        window.countEnergy=settings->value("/CountEnergy",false).toBool();
        window.countDfr=settings->value("/CountDfr",false).toBool();
        window.writeSpectrum=settings->value("/WriteSpectrum",false).toBool();
        //AttributeWindow window; //= new AttributeWindow(this);

        /*window->setMinTime(settings->value("/MinTime","0").toInt());
        window->setMaxTime(settings->value("/MaxTime","0").toInt());
        window->setMinOffset(settings->value("/MinOffset","0").toInt());
        window->setMaxOffset(settings->value("/MaxOffset","0").toInt());
        window->setMinAmpl(settings->value("/MinAmpl","0.0").toFloat());
        window->setMinRms(settings->value("/MinRms","0.0").toFloat());
        window->setMinFreq(settings->value("/MinFreq","0.0").toFloat());
        window->setMinDfr(settings->value("/MinDfr","0.0").toFloat());
        window->setMinEnergy(settings->value("/MinEnergy","0.0").toFloat());
        window->setCountAmpl(settings->value("/CountAmpl",false).toBool());
        window->setCountRms(settings->value("/CountRms",false).toBool());
        window->setCountFreq(settings->value("/CountFreq",false).toBool());
        window->setCountEnergy(settings->value("/CountEnergy",false).toBool());
        window->setCountDfr(settings->value("/CountDfr",false).toBool());
        window->setWriteSpectrum(settings->value("/WriteSpectrum",false).toBool());*/
        windows.append(AttributeWindow(window));
    }
    settings->endArray();
    size = settings->beginReadArray("/Relations");
    //QString tmp;
    for (int i=0; i<size; ++i)
    {
        settings->setArrayIndex(i);
        relations.append(QString("%1/%2>%3").arg(settings->value("/A1","A1").toString()).arg(settings->value("/A2","A1").toString()).arg(settings->value("/ratio","0").toString()));
    }
    settings->endArray();
    energyMinFreq =settings->value("/EnergyMinFreq",1).toUInt();
    energyMaxFreq = settings->value("/EnergyMaxFreq",125).toUInt();
    widthLvl = settings->value("/FreqLevel",15).toUInt();
    widthByEnergy = settings->value("/WidthEnergy",false).toBool();
    rmsSpectrum = settings->value("/RmsSpectrum",false).toBool();
    settings->endGroup();
    if (analysisAuxes)
    {
        settings->beginGroup("/VibAuxSettings");
        akfTraceNb = settings->value("/AkfTrace",2).toUInt();
        akfMaxTime = settings->value("/PeakTime",500).toUInt();
        akfFrqLvl = settings->value("/FreqLvl",-50).toInt();
        akfMinFrq = settings->value("/MinFreq",7).toUInt();
        akfMaxFrq =settings->value("/MaxFreq",100).toUInt();
        akfMaxAmpl = settings->value("/AkfMaxAmpl",0.0).toDouble();
        settings->endGroup();

        settings->beginGroup("/ExplAuxSettings");

        timeBreak.check = settings->value("/CheckTimeBreak",true).toBool();
        timeBreak.traceNb = settings->value("/TimeBreakTraceNb",1).toUInt();
        timeBreak.maxTime = settings->value("/TimeBreakTmax",10).toInt();
        timeBreak.maxAmpl = settings->value("/TimeBreakAmax",500).toFloat();
        timeBreak.nbOfDiscret = settings->value("/TimeBreakNbOfDiscret",4).toUInt();

        confirmedTimeBreak.check =  settings->value("/CheckConfirmedTimeBreak",true).toBool();

        confirmedTimeBreak.traceNb = settings->value("/ConfirmedTimeBreakTraceNb",2).toUInt();
        confirmedTimeBreak.maxTime = settings->value("/ConfirmedTimeBreakTmax",10).toUInt();
        confirmedTimeBreak.maxAmpl = settings->value("/ConfirmedTimeBreakAmax",500).toFloat();
        confirmedTimeBreak.nbOfDiscret = settings->value("/ConfirmedTimeBreakNbOfDiscret",90).toUInt();

        upholeTime.check = settings->value("/CheckUpholeTime",false).toBool();
        upholeTime.traceNb = settings->value("/UpholeTimeTraceNb",3).toInt();

        settings->endGroup();
    }
}


//формирование заголовков файла атрибутов

/*void BaseWorker::writeXlsxHeaders()
{

    xlsx.write(1,1,QString("Рассчет атрибутов"));
    xlsx.write(2,1,QString("Количество окон: %1").arg(windows.count()));
    xlsx.write(3,1,QString("Парметры окон:"));
    currentRow = 3;
    for (int i=0; i<windows.count();++i)
    {
        xlsx.write(4+i,1,QString("Окно#%1: Мин. удаление: %2; Макс удаление: %3; Мин. время: %4; Макс. время: %5").arg(i+1).arg(windows.value(i)->getMinOffset()).arg(windows.value(i)->getMaxOffset()).arg(windows.value(i)->getMinTime()).arg(windows.value(i)->getMaxTime()));
        currentRow++;
    }
    currentRow=currentRow+7;
    xlsxFormat.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    xlsxFormat.setPatternBackgroundColor(Qt::white);
    xlsx.write(currentRow,1,"FFID",xlsxFormat);
    xlsx.write(currentRow,2,"Line Nb",xlsxFormat);
    xlsx.write(currentRow,3, "Point Nb",xlsxFormat);
    xlsx.write(currentRow,4,"SourceX",xlsxFormat);
    xlsx.write(currentRow,5,"SourceY",xlsxFormat);
    xlsx.write(currentRow,6,"SourceZ",xlsxFormat);
    currentColumn = 7;

    if (checkTests)
    {
        xlsx.write(currentRow,currentColumn,"Test Status",xlsxFormat);
        currentColumn++;
    }
    if (analysisAuxes)
    {
        xlsx.write(currentRow,currentColumn,"Aux Status",xlsxFormat);
        currentColumn++;
    }

    for (int i=0; i<windows.count();++i)
    {
        if (windows.value(i)->getCountAmpl())
        {
            xlsx.write(currentRow, currentColumn, QString("Ampl%1").arg(i+1),xlsxFormat);
            currentColumn++;
        }
        if (windows.value(i)->getCountRms())
        {
            xlsx.write(currentRow, currentColumn, QString("RMS%1").arg(i+1),xlsxFormat);
            currentColumn++;
        }
        if (windows.value(i)->getCountFreq())
        {
            xlsx.write(currentRow, currentColumn, QString("Freq%1").arg(i+1),xlsxFormat);
            currentColumn++;
        }
        if (windows.value(i)->getCountEnergy())
        {
            xlsx.write(currentRow, currentColumn, QString("Energy%1").arg(i+1),xlsxFormat);
            currentColumn++;
        }
        if (windows.value(i)->getCountDfr())
        {
            xlsx.write(currentRow,currentColumn,QString("DFR%1").arg(i+1),xlsxFormat);
            currentColumn++;
        }
    }
    currentColumn++;
    QString tmp;
    for (int i=0; i<relations.count();++i)
    {
        tmp = relations.value(i);
        tmp = tmp.left(tmp.lastIndexOf(">"));
        xlsx.write(currentRow,currentColumn,tmp,xlsxFormat);
        currentColumn++;
    }

}*/

//cохранение файла атрибутов
/*void BaseWorker::saveXlsxFile()
{
    *logStream << QString("%1 Сохранение файла атрибутов \n").arg(QDateTime::currentDateTime().toString("ddd dd.MMMM.yyyy hh:mm::ss"));
    bool save = xlsx.saveAs(attrFilePath);
    QString newName;
    while (!save)
    {
        qDebug()<<QString(attrFilePath.left(attrFilePath.size()-5)+ QDateTime::currentDateTime().toString("_dd_MM_yy_hh_mm_ss")+".xlsx");
        newName = QString(attrFilePath.left(attrFilePath.size()-5)+ QDateTime::currentDateTime().toString("_dd_MM_yy_hh_mm_ss")+".xlsx");
        save = xlsx.saveAs(newName);
    }
    *logStream << QString("%1 Файл атрибутов %2 сохранен успешно!\n").arg(QDateTime::currentDateTime().toString("ddd dd.MMMM.yyyy hh:mm::ss")).arg(newName);
}*/

//чтение эксклюзивных зон

void BaseWorker::setExclusions(const QString &exclFileName)
{
    QFile xclFile(exclFileName);
    if (xclFile.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QString tmp;
        tmp=xclFile.readLine();
        if (tmp!="# Version Number\n")
        {
            emit sendSomeError(QString("Файл %1 не является файлом эксклюзивных зон Mesa").arg(exclFileName));
            useExclusions = false;
            return;
        }
        tmp = xclFile.readLine();
        tmp = xclFile.readLine();
        int exclCount = tmp.toInt();
        tmp = xclFile.readLine();
        //QString tmp;
        QStringList exclSettings;
        Exclusion* p_excl;
        int type;
        for (int i=0; i<exclCount;++i)
        {
            p_excl = new Exclusion();
            tmp = xclFile.readLine();
            if (tmp=="LAYER\n")
            {
                xclFile.readLine();
                tmp = xclFile.readLine();
            }
            p_excl->setName(tmp);
            tmp = xclFile.readLine();
            exclSettings=tmp.split(" ",QString::SkipEmptyParts);
            type = exclSettings.value(1).toInt();
            p_excl->setType(type);
            int angles;
            QStringList coords;
            switch (type) {
            case 0:
                tmp = xclFile.readLine();
                p_excl->addCircle(exclSettings.value(0),tmp);
                exclusions.append(p_excl);
                break;
            case 1:
                angles = exclSettings.value(3).toInt();
                for (int j=0;j<angles;++j)
                {
                    tmp = xclFile.readLine();
                    coords.append(tmp);
                }
                p_excl->addLine(exclSettings.value(0),coords);
                exclusions.append(p_excl);
                break;
            case 2:
                angles = exclSettings.value(3).toInt();
                for (int j=0;j<angles;++j)
                {
                    tmp = xclFile.readLine();
                    coords.append(tmp);
                }
                p_excl->addPoligon(coords);
                exclusions.append(p_excl);
                break;
            default:
                angles = exclSettings.value(3).toInt();
                for (int j=0;j<angles;++j)
                {
                    tmp = xclFile.readLine();
                }
                break;
            }
            coords.clear();
        }
    }
    else
    {
        useExclusions = false;
        return;
    }

}

void BaseWorker::setReceiversInExclusions(const QString &exclFileName)
{
    QFile xclFile(exclFileName);
    if (xclFile.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QString tmp;
        QStringList tmpList;
        while(!xclFile.atEnd())
        {
            tmp = xclFile.readLine();
            tmpList = tmp.split(QRegExp("\\s"),QString::SkipEmptyParts);
            if (!tmpList.isEmpty())
            {
                exclPoints.append(tmpList.value(0).toInt());
            }
        }
        xclFile.close();
    }
    else
    {
        useExclusions = false;
        return;
    }
}

//устанавливаем режим чтения
void BaseWorker::setMode(const bool &md)
{
    mode =md; // режим чтения из папки или начиная с файла
}

// устанавливаем откуда брать координаты пв

void BaseWorker::setUseExternalSps(const bool &use)
{
    useExternalSps = use;
}

// устанавливаем откуда брать координаты пп

void BaseWorker::setUseExternalRps(const bool &use)
{
    useExternalRps = use;
}

void BaseWorker::setUseExternalXps(const bool &use)
{
    useExternalXps = use;
}
//читаем r-файл
bool BaseWorker::readRps(const QString &path)
{
    QFile rpsFile(path);
    QString tmp;
    Point *p_receiver;
    if (rpsFile.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        while (!rpsFile.atEnd()) {
            tmp=rpsFile.readLine();
            if (tmp[0]=='R')
            {
                p_receiver = new Point(tmp);
                pp.insert(p_receiver->getLinePoint(),p_receiver);
            }
        }
    }
    else
    {
        return false;
    }
    if (pp.isEmpty())
    {
        return false;
    }
    else
    {
        return true;
    }
}
//читаем s-файл
bool BaseWorker::readSps(const QString &path)
{
    QFile spsFile(path);
    QString tmp;
    Point *p_source;
    if (spsFile.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        while (!spsFile.atEnd()) {
            tmp=spsFile.readLine();
            if (tmp[0]=='S')
            {
                p_source = new Point(tmp);
                pv.insert(p_source->getLinePoint(),p_source);
            }
        }
    }
    else
    {
        return false;
    }
    if (pp.isEmpty())
    {
        return false;
    }
    else
    {
        return true;
    }
}

//останавливаем конвертацию
void BaseWorker::stopRunning()
{
    qDebug()<<QString("stopped");
    if (*run)
    {
        *run=false;
    }
    else {
        emit finished();
    }
}

//создаем файл отчета для трасс не участвующих в рассчете атрибутов
void BaseWorker::createFileForMissedTraces()
{
    QFile* missedTraces= new QFile(outPath+"_missedTraces.txt");
    if (missedTraces->open(QIODevice::WriteOnly|QIODevice::Text))
    {
        QTextStream *missedStream = new QTextStream(missedTraces);
        *missedStream<<"FFID\t"<<"LinePoint\t"<<"Receiver X\t"<<"Receiver Y\t"<<"Receiver Z\n";
        missedTraces->close();
        delete missedStream;
        delete missedTraces;
    }
    else
    {
        emit sendSomeError("Ошибка открытия файла отчета трасс, попадающих в эксклюзивные зоны \n"
                           "Отчет не сохраняется");

    }
}

QQueue<QString> *BaseWorker::findTemplates(const int &ffid)
{
    QQueue<QString> *result = new QQueue<QString>();
    QFile xps(xpsPath);
    if (xps.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QString line;
        while (!xps.atEnd()) {

            line = xps.readLine();
            if (line.mid(7,4).toInt()==ffid)
            {
                while(line.mid(7,4).toInt()==ffid)
                {
                    result->enqueue(QString(line));
                    line = xps.readLine();
                }
                break;
            }
        }
    }
    else
    {
        useExternalXps = false;
    }
    return result;
}

//рассчитываем все атрибуты в окне
void BaseWorker::countAttriburesInWindow(QVector<QVector<float> > &traces, const int &winNb, const int &sInt, const int &ffid, QMap<QString,float> *ampls)
{
    float attribute=0.0;
    bool checkAttribute=true;
    if (windows.at(winNb).countAmpl)
    {
        attribute = getAbsAvg(traces);
        checkAttribute = attribute>windows.at(winNb).minAmpl ? true:false;
        fileAttributes.append(qMakePair(attribute,checkAttribute));
        ampls->insert(QString("A%1").arg(winNb+1),attribute);
    }
    if (windows.at(winNb).countRms)
    {
        attribute = getAbsAvg(traces);
        checkAttribute = attribute>windows.at(winNb).minRms ? true:false;
        fileAttributes.append(qMakePair(attribute,checkAttribute));
        ampls->insert(QString("R%1").arg(winNb+1),attribute);
    }
    if (windows.at(winNb).countFreq)
    {
        attribute = countFreq(traces,sInt);
        checkAttribute = attribute>windows.at(winNb).minFreq ? true:false;
        fileAttributes.append(qMakePair(attribute,checkAttribute));
    }
    if (windows.at(winNb).countEnergy || windows.at(winNb).countDfr || windows.at(winNb).writeSpectrum )
    {
        std::vector<double> spectrum = getSpectrum(traces);
        float frqStep = (1000000.0/sInt)/spectrum.size();
        spectrum.resize(spectrum.size()/2+1);
        double maxAmpl = *(std::max_element(spectrum.begin(),spectrum.end()));
        if (windows.at(winNb).writeSpectrum)
        {

            QFile *spectrumFile = new QFile();
            spectrumFile->setFileName(QString("%1_ffid%2_spectrum_for_attributeWindow#_%3.txt").arg(attrFilePath).arg(ffid).arg(winNb+1));
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
        if (windows.at(winNb).countEnergy || windows.at(winNb).countDfr)
        {
           attribute = getEnergy(spectrum,frqStep);
           if (windows.at(winNb).countEnergy)
           {
               checkAttribute = attribute>windows.at(winNb).minEnergy ? true:false;
               fileAttributes.append(qMakePair(attribute,checkAttribute));
           }
           if (windows.at(winNb).countDfr)
           {
               if (widthByEnergy)
               {
                   attribute = attribute/maxAmpl;
               }
               else
               {
                   attribute = getWidth(spectrum)*frqStep;
               }
               checkAttribute = attribute>windows.at(winNb).minDfr ? true:false;
               fileAttributes.append(qMakePair(attribute,checkAttribute));

           }

        }
    }
    //tracesInWindow.clear();
}


// считаем среднее значение сигнала
float BaseWorker::getAbsAvg(QVector<QVector<float> > &tracesData)
{
    int count  = 0;
    float sum=0.0;
    foreach (QVector<float> fVec, tracesData) {
        for (int i=0; i<fVec.count(); ++i )
        {
            sum +=fabs(fVec.value(i));
            count ++;
        }
    }
    return sum/count;
}
//считаем среднеквадратическое значение сигнала
float BaseWorker::getRms(QVector<QVector<float> > &tracesData)
{
    int count  = 0;
    float sum=0.0;
    foreach (QVector<float> fVec, tracesData) {
        for (int i=0; i<fVec.count(); ++i )
        {
            sum += powf(fVec.value(i), 2);
            count ++;
        }
    }
    return sqrt(sum/count);
}

double BaseWorker::countFreq(QVector<QVector<float> > &tracesData, const int &sRate)
{
    double freq = 0.0;
    int count=0;
    foreach (QVector<float> vec, tracesData) {
        if (vec.size()>32)
        {
        freq+=countFreqByTrace(vec,sRate);
        count++;
        }
    }
    //return freq/tracesData.count();
    if (count!=0) {
        return freq/count;
    }
    else
    {
        return 0.0;
    }
}

double BaseWorker::countFreqByTrace(QVector<float> &trace, const int &sRate)
{
   const std::size_t SIZE = pow(2,ceil(log2(trace.size())))*2;
   std::vector <double> vecD(trace.begin(),trace.end());
   double avg = std::accumulate(vecD.begin(),vecD.end(),0.0)/vecD.size();
   std::transform(vecD.begin(),vecD.end(),vecD.begin(), std::bind2nd(std::minus<double>(),avg));
   vecD.resize(SIZE);
   auto fft = Aquila::FftFactory::getFft(SIZE);
   Aquila::SpectrumType spectrum = fft->fft(vecD.data());
   std::transform(spectrum.begin(),spectrum.end(),spectrum.begin(),[](std::complex<double> c ) {return std::complex<double> (c.real()*c.real(),-1*c.imag()*c.imag());});
   double akf[SIZE];
   fft->ifft(spectrum,akf);
   int n0=0;
   if (akf[0] ==0){
     return 0.0;
   }
   else
   {
       while (akf[n0] > 0) {
           n0++;
       }
   }
   double t0 = n0-1 - (akf[n0-1]/(akf[n0]-akf[n0-1]));
   return (1000000.0/sRate)/(4*t0);
       //std::transform(rmsSpectrum.begin(),rmsSpectrum.end(),rmsSpectrum.begin(), [traces] (double _d) {return sqrt(_d/traces);});
}


std::vector<double> BaseWorker::getSpectrum(QVector<QVector<float> > tracesData)
{

    const std::size_t SIZE = pow(2,ceil(log2(maxSizeOfVectors(tracesData))));
    /*
    const std::size_t SIZE = pow(2,ceil(log2(tracesData.value(0).size())));

    const std::size_t TRACES = tracesData.count();*/
    std::size_t TRACES =0;
    std::vector<double> avgSpectrum(SIZE);
    std::vector<double> spectrum(SIZE);
    foreach (QVector<float> vec, tracesData) {
        if (vec.size()>32)
        {
            spectrum = getSpectrumByTrace((vec.toStdVector()),SIZE);
            TRACES++;
            if (rmsSpectrum)
            {
                std::transform(spectrum.begin(),spectrum.end(),spectrum.begin(),[] (double _d){return pow(_d,2.0);});
            }
            std::transform(avgSpectrum.begin(),avgSpectrum.end(),spectrum.begin(),avgSpectrum.begin(),std::plus<double>());
        }
    }
    if (TRACES!=0)
    {
        if (rmsSpectrum){
            std::transform(avgSpectrum.begin(),avgSpectrum.end(),avgSpectrum.begin(), [TRACES] (double _d) {return sqrt(_d/TRACES);}); //среднеквадратичные спектры
        }
        else
        {
            std::transform(avgSpectrum.begin(),avgSpectrum.end(),avgSpectrum.begin(), [TRACES] (double _d) {return _d/TRACES;});
        }
    }
    return avgSpectrum;
}

int BaseWorker::maxSizeOfVectors(QVector<QVector<float> > tracesData)
{
    int max = 0;
    foreach (QVector<float> vec , tracesData)
    {
        if (max<vec.size())
        {
            max = vec.size();
        }
    }
    return max;
}

std::vector<double> BaseWorker::getSpectrumByTrace(std::vector<float> traceData)
{
    const std::size_t SIZE = pow(2,ceil(log2(traceData.size())));
    std::vector<double> vecD(traceData.begin(),traceData.end());
    vecD.resize(SIZE);
    auto fft = Aquila::FftFactory::getFft(SIZE);
    Aquila::SpectrumType spectrum = fft->fft(vecD.data());
    std::vector<double> ampls(SIZE);

    for (std::size_t i=0;i<SIZE;++i)
    {
        ampls[i]= std::abs(spectrum[i]);
    }
    return ampls;
}

std::vector<double> BaseWorker::getSpectrumByTrace(std::vector<float> traceData, const std::size_t &SIZE)
{
    //const std::size_t SIZE = pow(2,ceil(log2(traceData.size())));
    std::vector<double> vecD(traceData.begin(),traceData.end());
    vecD.resize(SIZE);
    auto fft = Aquila::FftFactory::getFft(SIZE);
    Aquila::SpectrumType spectrum = fft->fft(vecD.data());
    std::vector<double> ampls(SIZE);

    for (std::size_t i=0;i<SIZE;++i)
    {
        ampls[i]= std::abs(spectrum[i]);
    }
    return ampls;
}

float BaseWorker::getEnergy(std::vector<double> spectrum, const float &frqStep)
{
    float result=0.0;
    int i = ceil(energyMinFreq/frqStep);
    for ( ; i <= energyMaxFreq/frqStep ; ++i )
    {
        result+=spectrum.at(i)*frqStep;
    }
    return result;
}

int BaseWorker::getWidth(std::vector<double> spectrum)
{
    int min=0;
    int max = spectrum.size()-1;
    double maxA = *(std::max_element(spectrum.begin(),spectrum.end()));
    while (spectrum.at(min)<maxA*widthLvl/100.0)
    {min++;}
    while (spectrum.at(max)<maxA*widthLvl/100.0)
    {max--;}
    return max-min;
}

//

void BaseWorker::chekingAuxData(SegdFile *segd)
{
    if (segd->getExtendedHeader().getTypeOfProcess() > 2 && segd->getExtendedHeader().getTypeOfSource()==2 )
    {
        QVector<float> traceData = segd->getTrace(akfTraceNb-1)->getTraceData();
        QVector<float> spectrum = getSpectrumDb(traceData.toStdVector());
        uint sampleRate = segd->getExtendedHeader().getSampleRate()/1000;
        float maxAmpl = *std::max_element(traceData.begin(),traceData.end());
        uint maxTime = traceData.indexOf(maxAmpl) * sampleRate ;
        maxAmpl = maxAmpl/segd->getExtendedHeader().getDumpStackingFold();
        float amplKoef=0.0;
        amplKoef = (maxAmpl-akfMaxAmpl)/akfMaxAmpl;
         QVector<QPointF> *spectrumPoints = new QVector<QPointF>;
        QVector<QPointF> *tracePoints = new QVector<QPointF>;
        float frqStep = 1000000.0/segd->getExtendedHeader().getSampleRate()/spectrum.count();
        bool checkAkfTrace=false;
        if ((maxTime==akfMaxTime) && fabs(round(amplKoef*10000)/10000.0)==0)
        {
            checkAkfTrace = true;
        }
        bool checkSpectrum;
        int freqCount =std::round(akfMinFrq/frqStep);
        float minFrqValue = spectrum.value(freqCount);
        freqCount =std::round(akfMaxFrq/frqStep);
        float maxFrqValue = spectrum.value(freqCount);
        if (abs (minFrqValue-maxFrqValue) <5 && abs(minFrqValue-akfFrqLvl) <5 && abs(maxFrqValue-akfFrqLvl)<5)
        {
            checkSpectrum = true;
        }
        else
        {
            checkSpectrum = false;
        }
        for (int i=0;i<spectrum.count()/2+1;++i)
        {
            spectrumPoints->append(QPointF(frqStep*i,spectrum.value(i)));
        }
        int numOfSamplesForTrace = akfMaxTime*2/sampleRate;
        for (int i=0 ; i<=numOfSamplesForTrace; i++)
        {
            tracePoints->append(QPointF(segd->getTrace(akfTraceNb-1)->getTraceData().value(i),i*segd->getExtendedHeader().getSampleRate()/1000));
        }
        if (checkSpectrum && checkAkfTrace)
        {
            fileAttributes.append(qMakePair(QString("ok!"),true));
        }
        else
        {
            fileAttributes.append(qMakePair(QString("bad!"),false));
        }
        emit sendVectors(tracePoints,checkAkfTrace,spectrumPoints,checkSpectrum,segd->getGeneralThree().getExtendedFileNumber());
    }
    else if (segd->getExtendedHeader().getTypeOfSource()==1)
    {
        bool correctTimeBreak, correctConfrimedTimeBreak, correctUpholeTime;
        int sampleInt = segd->getExtendedHeader().getSampleRate()/1000;
        if (timeBreak.check){
            correctTimeBreak = checkTimeBreak(segd->getTrace(timeBreak.traceNb-1)->getTraceData(),sampleInt);
        }
        else{
            correctTimeBreak = true;
        }
        if (confirmedTimeBreak.check)
        {
            correctConfrimedTimeBreak = checkConfirmedTimeBreak(segd->getTrace(confirmedTimeBreak.traceNb-1)->getTraceData(),sampleInt);
        }
        else{
            correctConfrimedTimeBreak = true;
        }
        correctUpholeTime = true;
        QVector<QPointF> *timeBreakPoints = new QVector<QPointF>;
        QVector<QPointF> *confirmedTimeBreakPoints = new QVector<QPointF>;
        QVector<QPointF> *upholeTimePoints = new QVector<QPointF>;

        for (int i=0; i<=1000;i+=sampleInt)
        {
            timeBreakPoints->append(QPointF(segd->getTrace(timeBreak.traceNb-1)->getTraceData().value(i/sampleInt),i));
            confirmedTimeBreakPoints->append(QPointF(segd->getTrace(confirmedTimeBreak.traceNb-1)->getTraceData().value(i/sampleInt),i));
            upholeTimePoints->append(QPointF(segd->getTrace(upholeTime.traceNb-1)->getTraceData().value(i/sampleInt),i));
        }
        emit sendAuxStatus(correctTimeBreak && correctConfrimedTimeBreak && correctUpholeTime);
        emit sendExplAuxes(timeBreakPoints,correctTimeBreak,confirmedTimeBreakPoints,correctConfrimedTimeBreak,upholeTimePoints,correctUpholeTime);
    }

}

void BaseWorker::checkingTests(SegdFile *segd)
{
    float badTestsPercent = segd->checkTests();
    bool checkTest = badTestsPercent<testsPercent ? true : false;
    fileAttributes.append(qMakePair(badTestsPercent,checkTest));
}

QVector<float> BaseWorker::getSpectrumDb(std::vector<float> traceData)
{
    std::vector<double> spectrum = getSpectrumByTrace(traceData);
    Aquila::SampleType max = *(std::max_element(spectrum.begin(),spectrum.end()));
    QVector<float> spectrumDb;
    for (std::size_t i=0; i<spectrum.size();++i)
    {
        spectrumDb.append(Aquila::dB(spectrum.at(i),max));
    }
    return spectrumDb;
}

bool BaseWorker::checkTimeBreak(QVector<float> traceData, const int &sInt)
{
    float maxAmpl = *std::max_element(traceData.begin(),traceData.end());
    int firstMaxDiscret = traceData.indexOf(maxAmpl);
    int lastMaxDiscret = traceData.lastIndexOf(maxAmpl);
    if (abs(firstMaxDiscret*sInt-timeBreak.maxTime)>sInt)
    {
        return false;
    }
    if (fabs(maxAmpl - timeBreak.maxAmpl)/timeBreak.maxAmpl*100 > 5)
    {
        return false;
    }
    if (abs(lastMaxDiscret - firstMaxDiscret +1 - timeBreak.nbOfDiscret)>1)
    {
        return false;
    }
    return true;

}

bool BaseWorker::checkConfirmedTimeBreak(QVector<float> traceData, const int &sInt)
{
    float maxAmpl = *std::max_element(traceData.begin(),traceData.end());
    int firstMaxDiscret = traceData.indexOf(maxAmpl);
    int lastMaxDiscret = firstMaxDiscret;
    while (traceData.value(lastMaxDiscret)==maxAmpl)
    {
        lastMaxDiscret++;
    }
    if (abs(firstMaxDiscret*sInt-confirmedTimeBreak.maxTime)>sInt)
    {
        return false;
    }
    if (fabs(maxAmpl - confirmedTimeBreak.maxAmpl)/confirmedTimeBreak.maxAmpl*100 > 5)
    {
        return false;
    }
    if (abs(lastMaxDiscret - firstMaxDiscret +1 - confirmedTimeBreak.nbOfDiscret)>1)
    {
        return false;
    }
    return true;

}


TimeBreakSettings::TimeBreakSettings()
{
    check =false;
    traceNb=1;
    nbOfDiscret=1;
    maxTime=1;
    maxAmpl=1.0;
}


