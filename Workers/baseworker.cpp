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
#include <QDir>

QT_CHARTS_USE_NAMESPACE

BaseWorker::BaseWorker(volatile bool *running, CountedAttributes *attr):p_running(running),attributes(attr)
{
    settings = new QSettings(QCoreApplication::applicationDirPath()+QDir::separator()+"config.ini",QSettings::IniFormat,this);
}
BaseWorker::BaseWorker(volatile bool *running, CountedAttributes *attr, TestMap *tMap):p_running(running),attributes(attr),testMap(tMap)
{
    settings = new QSettings(QCoreApplication::applicationDirPath()+QDir::separator()+"config.ini",QSettings::IniFormat,this);
}
// устанавливаем путь до файлов segd
void BaseWorker::setSegdPath(const QString &path)
{
    paths.insert("SegdPath",QDir::toNativeSeparators(path));
}

// устанавливаем куда будем писать выходной файл
void BaseWorker::setOutPath(const QString &path)
{
    QString newPath = QDir::toNativeSeparators(path);
    paths.insert("OutPath",newPath);
    paths.insert("SpectrumsPath",newPath.left(newPath.lastIndexOf(QDir::separator())+1)+"spectrums"+QDir::separator());

    paths.insert("AuxPath",newPath.insert(newPath.lastIndexOf('.'),"_auxes"));
    QDir dir(paths.value("SpectrumsPath"));
    if (!dir.exists())
    {
        dir.mkdir(paths.value("SpectrumsPath"));
    }
}


void BaseWorker::setXpsPath(const QString &path)
{
    paths.insert("XpsPath",QDir::toNativeSeparators(path));
}

//чтение настроек конвертации
void BaseWorker::readSettings()
{
    logFile.setFileName(paths.value("OutPath")+"_log.txt");
    logFile.open(QIODevice::Text|QIODevice::WriteOnly);
    logStream.setDevice(&logFile);
    logStream.setCodec("UTF8");
    settings->beginGroup("/ConvertSettings");
    SercelMpFactor = settings->value("/UseHeaderMpFactor",1).toBool();
    analysisAuxes = settings->value("/AnalisysAuxes",false).toBool();

    checkTests = settings->value("/CheckTests",false).toBool();
    testsPercent = settings->value("/TestPercent",2).toInt();
    online = settings->value("/OnLine",false).toBool();
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

    if (settings->value("/Exclusions",false).toBool())
    {
        exType = settings->value("/TxtExclusion",false).toBool() ? exclusionType::txtExcl : exclusionType::mesaExcl;
    }
    else
    {
        exType = exclusionType::noExcl;
    }
    if (exType!=exclusionType::noExcl)
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
    if (settings->value("/NoWriteAuxes",true).toBool())
    {
        auxMode = writeAuxesMode::noWrite;
    }
    else
    {
        if (settings->value("/WriteAuxesInNewFile",false).toBool()) {
            auxMode = writeAuxesMode::writeInNewFile;
        }
        else{
            auxMode = writeAuxesMode::write;
        }
    }
    backup = settings->value("/Backup",false).toBool();
    if (backup)
    {

        QString BackupFolder = settings->value("/BackupFolder","").toString();
        QDir backupSegdPath(BackupFolder);
        if (!backupSegdPath.exists())
        {
            bool succes = backupSegdPath.mkdir(BackupFolder);
            if (!succes)
            {
                backup=false;
                messaging(QString("Ошибка создания директории %1. Создание резервной копии файлов segd не выполняется").arg(BackupFolder),Qt::red);
            }
        }
        paths.insert("BackupFolder",BackupFolder);
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
        windows.append(AttributeWindow(window));
    }
    settings->endArray();
    size = settings->beginReadArray("/Relations");
    for (int i=0; i<size; ++i)
    {
        settings->setArrayIndex(i);
        relations.append(Relation(settings->value("/A1","A1").toString(),settings->value("/A2","A1").toString(),settings->value("/ratio","0").toFloat()));
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
        akf.traceNb =settings->value("/AkfTrace",2).toUInt();
        akf.maxTime = settings->value("/PeakTime",500).toUInt();
        akf.frqLvl = settings->value("/FreqLvl",-50).toInt();
        akf.minFrq = settings->value("/MinFreq",7).toUInt();
        akf.maxFrq = settings->value("/MaxFreq",100).toUInt();
        akf.maxAmpl = settings->value("/AkfMaxAmpl",0.0).toDouble();
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
            //emit sendSomeError(QString("Файл %1 не является файлом эксклюзивных зон Mesa").arg(exclFileName));
            messaging(QString("Файл %1 не является файлом эксклюзивных зон Mesa").arg(exclFileName),Qt::red);
            exType = exclusionType::noExcl;
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
        exType = exclusionType::noExcl;;
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
        exType = exclusionType::noExcl;
        return;
    }
}

//устанавливаем режим чтения
void BaseWorker::setMode(const bool &md)
{
    if (md)
    {
        rMode = readMode::fromDir;
    }
    else
    {
        rMode = readMode::fromFile;
    }
}

void BaseWorker::setUseExternalXps(const bool &use)
{
    useExternalXps = use;
}
//читаем r-файл
void BaseWorker::readRps(const QString &path)
{
    QFile rpsFile(path);
    QString tmp;
    Point receiver;
    if (rpsFile.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        while (!rpsFile.atEnd()) {
            tmp=rpsFile.readLine();
            if (tmp[0]=='R')
            {
                receiver = Point(tmp);
                pp.insert(receiver.getLinePoint(),Point(receiver));
            }
        }
    }
    else
    {
        messaging(QString(" Ошибка открытия файла %1. Координаты ПП будут взяты из заголовков SEGD файла.").arg(path),Qt::red);
    }
    if (pp.isEmpty())
    {
        messaging(QString(" В файле %1 не содержится R-записей. Координаты ПВ будут взяты из заголовков SEGD файла.").arg(path),Qt::red);
    }
    else
    {
        messaging(QString(" В файле %1 содержится %2 R-записей").arg(path).arg(pp.count()),Qt::darkGray);
    }
}
//читаем s-файл
void BaseWorker::readSps(const QString &path)
{
    QFile spsFile(path);
    QString tmp;
    Point source;
    if (spsFile.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        while (!spsFile.atEnd()) {
            tmp=spsFile.readLine();
            if (tmp[0]=='S')
            {
                source = Point(tmp);
                pv.insert(source.getLinePoint(),Point(source));
            }
        }
    }
    else
    {
        //emit sendSomeError(QString(" Ошибка открытия файла %1. Координаты ПВ будут взяты из заголовков SEGD файла.").arg(path));
        messaging(QString(" Ошибка открытия файла %1. Координаты ПВ будут взяты из заголовков SEGD файла.").arg(path),Qt::red);
    }
    if (pv.isEmpty())
    {
        messaging(QString(" В файле %1 не содержится S-записей. Координаты ПВ будут взяты из заголовков SEGD файла.").arg(path),Qt::red);

    }
    else
    {
        messaging(QString(" В файле %1 содержится %2 S-записей").arg(path).arg(pv.count()),Qt::darkGray);
    }
}

//останавливаем конвертацию
void BaseWorker::stopRunning()
{
    if (logFile.isOpen()){
    //logStream << QString("%1 Завершение конвертации\n").arg(QDateTime::currentDateTime().toString("ddd dd.MMMM.yyyy hh:mm::ss"));
    messaging(" Завершение конвертации");
    logFile.close();
    emit finished();}
}

QQueue<QString> BaseWorker::findTemplates(const int &ffid)
{
    QQueue<QString> result;// = new QQueue<QString>();
    QFile xps(paths.value("XpsPath"));
    if (xps.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QString line;
        while (!xps.atEnd()) {
            line = xps.readLine();
            if (line.mid(7,4).toInt()==ffid)
            {
                while(line.mid(7,4).toInt()==ffid)
                {
                    result.enqueue(QString(line));
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
void BaseWorker::countAttributesInWindow(QVector<QVector<float> > &traces, const int &winNb, const int &sInt, const int &ffid, QMap<QString,float> *ampls)
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
            spectrumFile->setFileName(QString("%1_ffid%2_spectrum_for_attributeWindow#_%3.txt").arg(paths.value("SpectrumsPath")).arg(ffid).arg(winNb+1));
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



void BaseWorker::countRelations(QMap<QString, float> ampls)
{
    float attribute =0.0;
    bool checkAttribute = true;
    foreach (Relation r, relations) {
        attribute = ampls.value(r.dividend)/ampls.value(r.devider);
        checkAttribute = attribute>r.minValue ? true:false;
        fileAttributes.append(qMakePair(attribute,checkAttribute));
    }
}

// считаем среднее значение сигнала
float BaseWorker::getAbsAvg(QVector<QVector<float> > &tracesData)
{
    int count  = 0;
    float sum = 0.0;
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
}


std::vector<double> BaseWorker::getSpectrum(QVector<QVector<float> > tracesData)
{

    const std::size_t SIZE = pow(2,ceil(log2(maxSizeOfVectors(tracesData))));
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
        QVector<float> traceData = segd->getTrace(akf.traceNb-1)->getTraceData();
        QVector<float> spectrum = getSpectrumDb(traceData.toStdVector());
        uint sampleRate = segd->getExtendedHeader().getSampleRate()/1000;
        float maxAmpl = *std::max_element(traceData.begin(),traceData.end());
        uint maxTime = traceData.indexOf(maxAmpl) * sampleRate ;
        maxAmpl = maxAmpl/segd->getExtendedHeader().getDumpStackingFold();
        float amplKoef=0.0;
        amplKoef = (maxAmpl-akf.maxAmpl)/akf.maxAmpl;
        QVector<QPointF> *spectrumPoints = new QVector<QPointF>;
        QVector<QPointF> *tracePoints = new QVector<QPointF>;
        float frqStep = 1000000.0/segd->getExtendedHeader().getSampleRate()/spectrum.count();
        bool checkAkfTrace=false;
        if ((maxTime==akf.maxTime) && fabs(round(amplKoef*10000)/10000.0)==0)
        {
            checkAkfTrace = true;
        }
        bool checkSpectrum;
        int freqCount =std::round(akf.minFrq/frqStep);
        float minFrqValue = spectrum.value(freqCount);
        freqCount =std::round(akf.maxFrq/frqStep);
        float maxFrqValue = spectrum.value(freqCount);
        if (abs (minFrqValue-maxFrqValue) <5 && abs(minFrqValue-akf.frqLvl) <5 && abs(maxFrqValue-akf.frqLvl)<5)
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
        int numOfSamplesForTrace = akf.maxTime*2/sampleRate;
        for (int i=0 ; i<=numOfSamplesForTrace; i++)
        {
            tracePoints->append(QPointF(segd->getTrace(akf.traceNb-1)->getTraceData().value(i),i*segd->getExtendedHeader().getSampleRate()/1000));
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
        int countedUphole = countUpholeTime(segd->getTrace(upholeTime.traceNb-1)->getTraceData().mid(800/sampleInt+1,200/sampleInt))*sampleInt;
        if (abs(countedUphole*1000 - segd->getExtendedHeader().getUpholeTime())<segd->getExtendedHeader().getSampleRate())
        {
            correctUpholeTime = true;
        }
        else
        {
            correctUpholeTime =false;
        }
        QVector<QPointF> *timeBreakPoints = new QVector<QPointF>;
        QVector<QPointF> *confirmedTimeBreakPoints = new QVector<QPointF>;
        QVector<QPointF> *upholeTimePoints = new QVector<QPointF>;

        for (int i=0; i<=1000;i+=sampleInt)
        {
            timeBreakPoints->append(QPointF(segd->getTrace(timeBreak.traceNb-1)->getTraceData().value(i/sampleInt),i));
            confirmedTimeBreakPoints->append(QPointF(segd->getTrace(confirmedTimeBreak.traceNb-1)->getTraceData().value(i/sampleInt),i));
            if (i >= 800)
            {
            upholeTimePoints->append(QPointF(segd->getTrace(upholeTime.traceNb-1)->getTraceData().value(i/sampleInt),i));
            }
        }
        if (correctTimeBreak && correctConfrimedTimeBreak &&correctUpholeTime)
        {
            fileAttributes.append(qMakePair(countedUphole,true));
        }
        else
        {
            fileAttributes.append(qMakePair(countedUphole,false));
        }
        emit sendExplAuxes(segd->getGeneralThree().getExtendedFileNumber(), timeBreakPoints,correctTimeBreak,confirmedTimeBreakPoints,correctConfrimedTimeBreak,upholeTimePoints,correctUpholeTime,countedUphole,segd->getExtendedHeader().getUpholeTime()/1000.0);
    }
}

void BaseWorker::checkingTests(SegdFile *segd)
{

    //fileAttributes.append(segd->checkTests(&logStream,2,2));
    fileAttributes.append(segd->checkTests(&logStream,2,2,testMap));
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

int BaseWorker::countUpholeTime(QVector<float> traceData)
{
    float noise =fabs(traceData.at(0)*1.5);
    int i=0;
    float maxElement =fabs(*std::max_element(traceData.begin(),traceData.end(),[](float a, float b) {return fabs(a)<fabs(b);}))/6.0;
    while (fabs(traceData.at(i))<maxElement)
    {
        i++;
    }
    while(fabs(i>0 && traceData.at(i)/traceData.at(i-1)) > 1.4 && fabs(traceData.at(i)) > noise)
    {
        i--;
    }
    return i-1;
}


void BaseWorker::messaging(const QString &message, const QColor &color)
{
    logStream <<QDateTime::currentDateTime().toString("ddd dd.MMMM.yyyy hh:mm:ss").append(message)<<"\n";
    emit sendInfoMessage(QDateTime::currentDateTime().toString("hh:mm:ss").append(message),color);
}

void BaseWorker::maxNumOfFilesReached()
{
    fileCount=0;
    QString newPath = paths.value("OutPath");
    newPath.insert(newPath.lastIndexOf(QDir::separator())+1,'_');
    paths.insert("OutPath",QString(newPath));
    newPath = paths.value("AuxPath");
    newPath.insert(newPath.lastIndexOf(QDir::separator())+1,'_');
    paths.insert("AuxPath",QString(newPath));
}

TimeBreakSettings::TimeBreakSettings()
{
    check =false;
    traceNb=1;
    nbOfDiscret=1;
    maxTime=1;
    maxAmpl=1.0;
}

AkfSettings::AkfSettings()
{
    traceNb=1;
    frqLvl=-10;
    maxTime=0;
    minFrq=0;
    maxFrq=0;
    maxAmpl=0.0;
}

