#include "segyfile.h"
#include <QFile>
#include <QDataStream>
#include <SUB/exclusion.h>
SegyFile::SegyFile(QObject *parent) : QObject(parent)
{

}
//конструктор, на входе путь к файлу segy
SegyFile::SegyFile(const QString &file, QObject *parent):QObject(parent)
{
    correctFile=false;
    QFile *mskSegy = new QFile;
    mskSegy->setFileName(file);
    if (!mskSegy->open(QIODevice::ReadOnly))
    {
        return;
    }
    path =file;
    QByteArray *tmpArr = new QByteArray;
    *tmpArr=mskSegy->read(3200);
    textHeader = EBDICtoASCII(*tmpArr);
    *tmpArr=mskSegy->read(4);
    jobId=toUint(*tmpArr);
    *tmpArr=mskSegy->read(4);
    lineNumber=toUint(*tmpArr);
    *tmpArr=mskSegy->read(4);
    reelNumber=toUint(*tmpArr);
    *tmpArr=mskSegy->read(2);
    numOfDataTraces=toUShort(*tmpArr);
    *tmpArr=mskSegy->read(2);
    numOfAuxiliaryTraces=toUShort(*tmpArr);
    *tmpArr=mskSegy->read(2);
    sampleInterval=toUShort(*tmpArr);
    *tmpArr=mskSegy->read(2);
    sampleIntervalOfOriginalRecording=toUShort(*tmpArr);
    *tmpArr=mskSegy->read(2);
    numOfSamples=toUShort(*tmpArr);
    *tmpArr=mskSegy->read(2);
    numOfSamplesOfOriginalRecording=toUShort(*tmpArr);
    *tmpArr=mskSegy->read(2);
    dataFormatCode=toUShort(*tmpArr);
    *tmpArr=mskSegy->read(2);
    ensembleFold=toUShort(*tmpArr);
    *tmpArr=mskSegy->read(2);
    traceSortingCode=toUShort(*tmpArr);
    *tmpArr=mskSegy->read(2);
    verticalSumeCode=toUShort(*tmpArr);
    *tmpArr=mskSegy->read(2);
    sweepFreqAtStart=toUShort(*tmpArr);
    *tmpArr=mskSegy->read(2);
    sweepFreqAtEnd=toUShort(*tmpArr);
    *tmpArr=mskSegy->read(2);
    sweepLenght=toUShort(*tmpArr);
    *tmpArr=mskSegy->read(2);
    sweepTypeCode=toUShort(*tmpArr);
    *tmpArr=mskSegy->read(2);
    traceNumOfSweepChannel=toUShort(*tmpArr);
    *tmpArr=mskSegy->read(2);
    sweepTraceTaperLengthAtStart=toUShort(*tmpArr);
    *tmpArr=mskSegy->read(2);
    sweepTraceTaperLengthAtEnd=toUShort(*tmpArr);
    *tmpArr=mskSegy->read(2);
    taperType=toUShort(*tmpArr);
    *tmpArr=mskSegy->read(2);
    correlatedDataTraces=toUShort(*tmpArr);
    *tmpArr=mskSegy->read(2);
    binaryGainRecovered=toUShort(*tmpArr);
    *tmpArr=mskSegy->read(2);
    amplitudeRecovertyMethod=toUShort(*tmpArr);
    *tmpArr=mskSegy->read(2);
    measurementSystem=toUShort(*tmpArr);
    *tmpArr=mskSegy->read(2);
    impulseSignalPolarity=toUShort(*tmpArr);
    *tmpArr=mskSegy->read(2);
    vibratoryPolatityCode=toUShort(*tmpArr);

    mskSegy->seek(3500);
    *tmpArr=mskSegy->read(2);
    segyFormatRevisionNumber=toUShort(*tmpArr);
    *tmpArr=mskSegy->read(2);
    fixedLenghtTraceFlag=toUShort(*tmpArr);
    *tmpArr=mskSegy->read(2);
    numberOf3200Byte=toUShort(*tmpArr);

    int bytePerSample=1;
    switch (dataFormatCode) {
    case 1:
        bytePerSample=4;
        correctFile=true;
        break;
    case 2:
        bytePerSample=4;
        correctFile=true;
        break;
    case 3:
        bytePerSample=2;
        correctFile=true;
        break;
    case 4:
        correctFile=false;
        break;
    case 5:
        bytePerSample=4;
        correctFile=true;
        break;
    case 6:
        correctFile=false;
        break;
    case 7:
        correctFile=false;
        break;
    case 8:
        bytePerSample=1;
        correctFile=true;
        break;
    default:
        correctFile=false;
        break;
    }
    //mskSegy->seek(3600)
    if (correctFile)
        {
            //for (int i = 0; i< numOfDataTraces+numOfAuxiliaryTraces;i++)
            int i=0;
            while (!mskSegy->atEnd())
            {
                mskSegy->seek(3600+(240+4*numOfSamples)*i);
                *tmpArr=mskSegy->read(240+bytePerSample*numOfSamples);
                //segyTrace sTr(*tmpArr,this);
                segyTraces.append(new segyTrace(*tmpArr,dataFormatCode,this));
                ++i;
            }
        }
    mskSegy->close();
}

SegyFile::SegyFile(SegdFile *segd, QObject *parent) : QObject(parent)
{
    jobId = segd->getExtendedHeader().getTapeNumber();
    lineNumber = segd->getGeneralThree().getLineNumber();
    reelNumber = segd->getGeneralThree().getPointNumber();
    numOfDataTraces =segd->getExtendedHeader().getNumberOfSeis();
    numOfAuxiliaryTraces=segd->getExtendedHeader().getNumberOfAuxes();
    sampleInterval=segd->getExtendedHeader().getSampleRate();
    sampleIntervalOfOriginalRecording=segd->getExtendedHeader().getSampleRate();
    numOfSamples=segd->getExtendedHeader().getSamplesPerTrace();
    numOfSamplesOfOriginalRecording=segd->getExtendedHeader().getSamplesPerTrace();
    dataFormatCode = 5; //IEEE плавающая запятая
    ensembleFold = 0;
    traceSortingCode = 1;
    verticalSumeCode = 1;
    sweepFreqAtStart = 0;
    sweepFreqAtEnd = 0;
    sweepLenght = segd->getExtendedHeader().getSweepLength();
    sweepTypeCode =0;
    traceNumOfSweepChannel =0;
    sweepTraceTaperLengthAtStart =0;
    sweepTraceTaperLengthAtEnd = 0;
    taperType = 0;
    correlatedDataTraces = 0;
    binaryGainRecovered = 0;
    correlatedDataTraces =0;
    amplitudeRecovertyMethod =0;
    measurementSystem =1;
    impulseSignalPolarity = 0;
    vibratoryPolatityCode = 0;
    segyFormatRevisionNumber = 0;
    fixedLenghtTraceFlag =0;
    numberOf3200Byte =0;
    for (int i=0; i< numOfAuxiliaryTraces+numOfDataTraces;++i)
    {
        segyTraces.append(new segyTrace(segd,i,this));
    }
}

SegyFile::~SegyFile()
{
    qDeleteAll(segyTraces);
}

QVector<int> SegyFile::getFirstBreaks(const int &percent, const bool &polar, const QVector<int> &traces)
{
    QVector<int> result;
    if (!segyTraces.isEmpty())
    {
        foreach (int trNum, traces) {
        result.append(segyTraces.value(trNum)->getFirstBrake(percent, polar));
        }
    }
    else {
        result.fill(0,traces.count());
    }
    return result;
}
int SegyFile::getFileNumFirstTrace()
{
    if (segyTraces.isEmpty())
    {
        return 0;
    }
    else
    {
        return segyTraces.value(0)->getFileNum();
    }
}
// конвертация текстового заголовка из EBDIC в ASCII
QString SegyFile::EBDICtoASCII(const QByteArray &bArr)
{
    QString result;
    uchar tmp;
    foreach (uchar ch, bArr) {
        tmp = e2a[(int)ch];
        result.append(tmp);
    }
    return result;
}
QByteArray SegyFile::ASCIItoEBDIC(const QString &str)
{
    QByteArray result;
    QByteArray tmpArr = str.toLocal8Bit();
    uchar tmp;
    foreach (uchar ch, tmpArr) {
        tmp =a2e[(int)ch];
        result.append(tmp);
    }
    return result;
}

// создаем пустой текстовый заголовок

QByteArray SegyFile::createEBDICHeader()
{
    QString tmpString;
    QByteArray result;
    for (int i=1; i<41; ++i)
    {
        tmpString = "C"+QString::number(i).rightJustified(2,'0');
        tmpString = tmpString.leftJustified(80,' ');
        result.append(ASCIItoEBDIC(tmpString));
    }
    return result;
}


//перевод из массива байт в беззнаковое целое
uint SegyFile::toUint(const QByteArray &bArr)
{
    QString str=QString(bArr.toHex());
    bool ok;
    uint i = str.toUInt(&ok,16);
    return i;
}
// перевод из массива байт в беззнаковое целое (2байта)
ushort SegyFile::toUShort(const QByteArray &bArr)
{
    QString str=QString(bArr.toHex());
    bool ok;
    uint i = str.toUShort(&ok,16);
    return i;
}
//перевод из массива байт в целое
int SegyFile::toInt(const QByteArray &bArr)
{
    int result;
    QByteArray tmpArr=bArr;
    QDataStream ds(&tmpArr,QIODevice::ReadOnly);
    ds>>result;
    return result;
}
//перевод из массива байт в целое (2 байте)
short SegyFile::toShort(const QByteArray &bArr)
{
    short result;
    QByteArray tmpArr=bArr;
    QDataStream ds(&tmpArr,QIODevice::ReadOnly);
    ds>>result;
    return result;
}

void SegyFile::writeHeaders(const QString &filePath)
{
    QFile *file = new QFile (filePath);
    file->open(QIODevice::WriteOnly);
    QDataStream dStream(file);
    QByteArray txtHeader = createEBDICHeader();
    file->write(txtHeader);
    //dStream<<txtHeader;
    dStream<<jobId;
    dStream<<(int)0;
    dStream<<(int)0;
    dStream<<(quint16)0;
    dStream<<(quint16)0;
    dStream<<sampleInterval;
    dStream<<sampleIntervalOfOriginalRecording;
    dStream<<numOfSamples;
    dStream<<numOfSamplesOfOriginalRecording;
    dStream<<(quint16)5; //IEEE плавающая запятая
    for (int i=0; i<374; ++i)
    {
        dStream<<(quint8)0;
    }
    file->close();
}

void SegyFile::writeTraces(const QString &filePath, const bool &muted)
{
    QFile fileOut;
    fileOut.setFileName(filePath);
    //fileOut.open(QIODevice::Append);
    QVector<segyTrace*>::iterator traceIt = segyTraces.begin();
    segyTrace* trace;
    quint32 traceNum=1;
    for ( ;traceIt!=segyTraces.end();++traceIt)
    {
        trace = *traceIt;
        if (trace->getTraceIdCode()!=9)
        {
            if (!muted)
            {
                if (trace->getTraceIdCode()!=2)
                {
                    trace->writeSegyTrace(fileOut,5,traceNum);
                    traceNum++;
                }
            }
            else {
                trace->writeSegyTrace(fileOut,5,traceNum);
                traceNum++;
            }
        }
    }
    //fileOut.close();
}

void SegyFile::writeTraces(const QString &filePath, const bool &muted, const bool &missed)
{
    QFile fileOut;
    fileOut.setFileName(filePath);
    //fileOut.open(QIODevice::Append);
    QVector<segyTrace*>::iterator traceIt = segyTraces.begin();
    segyTrace* trace;
    quint32 traceNum=1;
    for ( ;traceIt!=segyTraces.end();++traceIt)
    {
        trace = *traceIt;
        if (trace->getTraceIdCode()!=9)
        {
                if (muted && missed)
                {
                    trace->writeSegyTrace(fileOut,5,traceNum);
                    traceNum++;
                }
                if (muted && !missed && trace->getTraceIdCode()!=3)
                {
                    trace->writeSegyTrace(fileOut,5,traceNum);
                    traceNum++;
                }
                if (!muted && missed && trace->getTraceIdCode()!=2)
                {
                    trace->writeSegyTrace(fileOut,5,traceNum);
                    traceNum++;
                }
                if (!muted && !missed && trace->getTraceIdCode()!=2 && trace->getTraceIdCode()!=3)
                {
                    trace->writeSegyTrace(fileOut,5,traceNum);
                    traceNum++;
                }
        }
//        if (trace->getTraceIdCode()!=9)
//        {
//            if (!muted)
//            {
//                if (trace->getTraceIdCode()!=2)
//                {
//                    trace->writeSegyTrace(fileOut,5,traceNum);
//                    traceNum++;
//                }
//            }
//            else {
//                trace->writeSegyTrace(fileOut,5,traceNum);
//                traceNum++;
//            }
//        }
    }
    //fileOut.close();
}


void SegyFile::writeAuxTraces(const QString &filePath)
{
    QFile fileOut;
    fileOut.setFileName(filePath);
    //fileOut.open(QIODevice::Append);
    QVector<segyTrace*>::iterator traceIt = segyTraces.begin();
    segyTrace* trace;
    for ( ;traceIt!=segyTraces.end();++traceIt)
    {
        trace = *traceIt;
        if (trace->getTraceIdCode()==9)
        {
            trace->writeSegyTrace(fileOut,5);
        }
    }
    //fileOut.close();
}

void SegyFile::setReceiverCoordinats(QMap<QString, Point> coordinates)
{
    QVector<segyTrace*>::iterator traceIt = segyTraces.begin();
    segyTrace *trace;
    QString linePoint;
    for (; traceIt!=segyTraces.end();++traceIt)
    {
           trace = *traceIt;
           linePoint = trace->getReceiverLinePoint();
           if (coordinates.contains(linePoint))
           {
               trace->setReceiverX(coordinates.value(linePoint).getX());
               trace->setReceiverY(coordinates.value(linePoint).getY());
               trace->setReceiverZ(coordinates.value(linePoint).getZ());
           }
    }
}
int SegyFile::setReceiverCoordinats(QTextStream *logStr, QMap<QString, Point> coordinates)
{
    QVector<segyTrace*>::iterator traceIt = segyTraces.begin();
    segyTrace *trace;
    int notInRPS=0;
    //QString linePoint;
    for (; traceIt!=segyTraces.end();++traceIt)
    {
           trace = *traceIt;
           if (trace->getTraceIdCode()!=9 && trace->getTraceIdCode()!=3)
           {
               if (coordinates.contains(trace->getReceiverLinePoint()))
               {
                   trace->setReceiverX(coordinates.value(trace->getReceiverLinePoint()).getX());
                   trace->setReceiverY(coordinates.value(trace->getReceiverLinePoint()).getY());
                   trace->setReceiverZ(coordinates.value(trace->getReceiverLinePoint()).getZ());
               }
               else
               {
                   notInRPS++;
                   *logStr<<QString("В R файле не содержится координата для ПП %1 \n").arg(trace->getReceiverLinePoint());
               }
           }
    }
    return notInRPS;
}
bool SegyFile::setSourceCoordinats(QMap<QString, Point> coordinates)
{
    QVector<segyTrace*>::iterator traceIt = segyTraces.begin();
    segyTrace *trace =*traceIt;
    if (coordinates.contains(trace->getSourceLinePoint()))
    {
        for (;traceIt!=segyTraces.end();++traceIt)
        {
               trace = *traceIt;
               trace->setSourceX(coordinates.value(trace->getSourceLinePoint()).getX());
               trace->setSourceY(coordinates.value(trace->getSourceLinePoint()).getY());
               trace->setSourceZ(coordinates.value(trace->getSourceLinePoint()).getZ());
        }
        return true;
    }
    else {
        return false;
    }
}

void SegyFile::setGeometry()
{
    QVector<segyTrace*>::iterator traceIt = segyTraces.begin();
    segyTrace *trace;
    for (; traceIt!=segyTraces.end();++traceIt)
    {
        trace = *traceIt;
        if (trace->getTraceIdCode()!=9 && trace->getTraceIdCode()!=3)
        {
            trace->countGeometry();
        }
    }
}

void SegyFile::writeFile(const QString &filePath, const bool &writeFileHeader, const int &traceToRecord, const int &numOfTraces)
{
    QFile fileOut;
    fileOut.setFileName(filePath);
    if (writeFileHeader)
    {
        fileOut.open(QIODevice::WriteOnly);
        QByteArray bArr = ASCIItoEBDIC(textHeader);
        fileOut.write(bArr);
        if (dataFormatCode!=2)
        {
            dataFormatCode=5;
        }
        QDataStream dStream(&fileOut);
        dStream<<jobId<<lineNumber<<reelNumber<<(quint16)numOfTraces<<(quint16)0<<sampleInterval
              <<sampleIntervalOfOriginalRecording<<numOfSamples<<numOfSamplesOfOriginalRecording<<dataFormatCode
              <<ensembleFold<<traceSortingCode<<verticalSumeCode<<sweepFreqAtStart<<sweepFreqAtEnd<<sweepLenght
              <<sweepTypeCode<<traceNumOfSweepChannel<<sweepTraceTaperLengthAtStart<<sweepTraceTaperLengthAtEnd
              <<taperType<<correlatedDataTraces<<binaryGainRecovered<<amplitudeRecovertyMethod<<measurementSystem
              <<impulseSignalPolarity<<vibratoryPolatityCode;
        for (int i=0; i<240; i++)
        {
            dStream<<(quint8)0;
        }
        dStream<<segyFormatRevisionNumber<<fixedLenghtTraceFlag<<numberOf3200Byte;
        for (int i=0; i<94; i++)
        {
            dStream<<(quint8)0;
        }
        fileOut.close();
    }
    if (segyTraces.count()>traceToRecord)
    {
        segyTraces.value(traceToRecord)->writeSegyTrace(fileOut,dataFormatCode);
    }
}

int SegyFile::getNumOfSamples()
{
    return numOfSamples;
}
int SegyFile::getNumOfTraces()
{
    return segyTraces.count();
}
int SegyFile::getSampleInterval()
{
    return sampleInterval;
}
QVector<float> SegyFile::getTraceData(const int &numOfTrace)
{
    return segyTraces.value(numOfTrace)->getData();
}
float SegyFile::getMaxValueOfTrace(const int &numOfTrace)
{
    return segyTraces.value(numOfTrace)->getMaxValue();
}
bool SegyFile::isCorrect()
{
    return correctFile;
}
QString SegyFile::getPath()
{
    return path;
}


//получем QVector c данными трасс, попадающих в окно

/*QVector<QVector<float> > SegyFile::getDataInWindow(const uint &minOffset, const uint &maxOffset, const uint &minTime, const uint &maxTime, const bool &muted, const double minA)
{
    QVector<segyTrace*>::iterator tracesIterator = segyTraces.begin();
    int firstPos;
    int length;
    firstPos = minTime/(sampleInterval/1000);
    length = (maxTime-minTime)/(sampleInterval/1000);
    QVector< QVector<float> > result;
    segyTrace *trace;
    for (; tracesIterator!=segyTraces.end();++tracesIterator)
    {
        trace = *tracesIterator;
        if (muted)
        {
            if (trace->getOffset() > minOffset && trace->getOffset() < maxOffset &&trace->getTraceIdCode()!=9 && trace->getTraceIdCode()!=2)
            {
                result.append(trace->getData().mid(firstPos,length));
            }
        }
        else
        {
            if (trace->getOffset() > minOffset && trace->getOffset() < maxOffset &&trace->getTraceIdCode()!=9)
            {
                result.append(trace->getData().mid(firstPos,length));
            }
        }


    }
    return result;
}*/

QVector<QVector<float> > SegyFile::getDataInWindow(QTextStream *logStr,const uint &minOffset, const uint &maxOffset, const uint &minTime, const uint &maxTime, const bool &muted, const bool &tests, const float &minA)
{
    QVector<segyTrace*>::iterator tracesIterator = segyTraces.begin();
    int firstPos;
    int length;
    firstPos = minTime/(sampleInterval/1000);
    length = (maxTime-minTime)/(sampleInterval/1000);
    QVector< QVector<float> > result;
    segyTrace *trace;
    int nbOfMuted=0;
    int nbOfTest=0;
    int nbOfA =0;
    *logStr <<QString("Исключенные трассы: \n").toUtf8();
    *logStr <<QString("Пикет\t Причина \n").toUtf8();
    for (; tracesIterator!=segyTraces.end();++tracesIterator)
    {
        trace = *tracesIterator;
        if (trace->getOffset() > minOffset && trace->getOffset() < maxOffset &&trace->getTraceIdCode()!=9)
        {
            if (muted && trace->getTraceIdCode()==2)
            {
                nbOfMuted++;
                *logStr << trace->getReceiverLinePoint()<<QString("\t Muted\n");
                continue;
            }
            if (tests && trace->getDeviceTraceIdentifier()==1)
            {
                nbOfTest++;
                *logStr << trace->getReceiverLinePoint()<<QString("\t Bad Tests\n");
                continue;
            }
            if (trace->getMaxValue() < minA)
            {
                *logStr << trace->getReceiverLinePoint()<<QString("\t Min Ampl\n");
                nbOfA++;
                continue;
            }
            result.append(trace->getData().mid(firstPos,length));
        }
    }
    *logStr<<QString("Muted: %1; BadTests: %2; MinA: %3; All Bad: %4; For Count: %5\n").arg(nbOfMuted).arg(nbOfTest).arg(nbOfA).arg(nbOfMuted+nbOfTest+nbOfA).arg(result.size());
    return result;
}

QVector<QVector<float> > SegyFile::getDataInWindow(QTextStream *logStr,const uint &minOffset, const uint &maxOffset, const uint &minTime, const uint &maxTime, const bool &muted, const bool &tests, const float &minA,QVector<int> exclReceivers)
{
    QVector<segyTrace*>::iterator tracesIterator = segyTraces.begin();


    int firstPos;
    int length;
    firstPos = minTime/(sampleInterval/1000);
    length = (maxTime-minTime)/(sampleInterval/1000);
    QVector< QVector<float> > result;
    segyTrace *trace;
    int nbOfMuted=0;
    int nbOfTest=0;
    int nbOfA =0;
    int nbOfExcl=0;
    *logStr <<QString("Исключенные трассы: \n").toUtf8();
    *logStr <<QString("Пикет\t Причина \n").toUtf8();
    for (; tracesIterator!=segyTraces.end();++tracesIterator)
    {
        trace = *tracesIterator;
        if (trace->getOffset() > minOffset && trace->getOffset() < maxOffset &&trace->getTraceIdCode()!=9)
        {
            if (exclReceivers.indexOf(trace->getReceiverLinePoint().toInt())!=-1)
            {
                nbOfExcl++;
                *logStr << trace->getReceiverLinePoint()<<QString("\t Exclusion\n");
                continue;
            }
            if (muted && trace->getTraceIdCode()==2)
            {
                nbOfMuted++;
                *logStr << trace->getReceiverLinePoint()<<QString("\t Muted\n");
                continue;
            }
            if (tests && trace->getDeviceTraceIdentifier()==1)
            {
                nbOfTest++;
                *logStr << trace->getReceiverLinePoint()<<QString("\t Bad Tests\n");
                continue;
            }
            if (trace->getMaxValue() < minA)
            {
                *logStr << trace->getReceiverLinePoint()<<QString("\t Min Ampl\n");
                nbOfA++;
                continue;
            }
            result.append(trace->getData().mid(firstPos,length));
        }
    }
    *logStr<<QString("Muted: %1; Bad Tests: %2; Min Ampl: %3; Exclusion:%6 All Bad: %4; For Count: %5\n").arg(nbOfMuted).arg(nbOfTest).arg(nbOfA).arg(nbOfMuted+nbOfTest+nbOfA+nbOfExcl).arg(result.size()).arg(nbOfExcl);

    return result;
}

QVector<QVector<float> > SegyFile::getDataInWindow(QTextStream *logStr, const uint &minOffset, const uint &maxOffset, const uint &minTime, const uint &maxTime, const bool &muted, const bool &tests, const float &minA,QVector<Exclusion*> exclVec)
{
    QVector<segyTrace*>::iterator tracesIterator = segyTraces.begin();
    int firstPos;
    int length;
    firstPos = minTime/(sampleInterval/1000);
    length = (maxTime-minTime)/(sampleInterval/1000);
    QVector< QVector<float> > result;
    segyTrace *trace;
    int nbOfMuted=0;
    int nbOfTest=0;
    int nbOfA =0;
    int nbOfExcl=0;
    *logStr <<QString("Исключенные трассы: \n").toUtf8();
    *logStr <<QString("Пикет\t Причина \n").toUtf8();
    for (; tracesIterator!=segyTraces.end();++tracesIterator)
    {
        trace = *tracesIterator;
        if (trace->getOffset() > minOffset && trace->getOffset() < maxOffset &&trace->getTraceIdCode()!=9)
        {
            if (traceInExclusion(trace,exclVec))
            {
                nbOfExcl++;
                *logStr << trace->getReceiverLinePoint()<<QString("\t Exclusion\n");
                continue;
            }
            if (muted && trace->getTraceIdCode()==2)
            {
                nbOfMuted++;
                *logStr << trace->getReceiverLinePoint()<<QString("\t Muted\n");
                continue;
            }
            if (tests && trace->getDeviceTraceIdentifier()==1)
            {
                nbOfTest++;
                *logStr << trace->getReceiverLinePoint()<<QString("\t Bad Tests\n");
                continue;
            }
            if (trace->getMaxValue() < minA)
            {
                *logStr << trace->getReceiverLinePoint()<<QString("\t Min Ampl\n");
                nbOfA++;
                continue;
            }
            result.append(trace->getData().mid(firstPos,length));
        }
    }
    *logStr<<QString("Muted: %1; Bad Tests: %2; Min Ampl: %3; Exclusion:%6 All Bad: %4; For Count: %5\n").arg(nbOfMuted).arg(nbOfTest).arg(nbOfA).arg(nbOfMuted+nbOfTest+nbOfA+nbOfExcl).arg(result.size()).arg(nbOfExcl);
    return result;
}



/*QVector<QVector<float> > SegyFile::getDataInWindow(const uint &minOffset, const uint &maxOffset, const uint &minTime, const uint &maxTime, const uint &v1, const uint &v2, const bool muted)
{
    QVector<segyTrace*>::iterator tracesIterator = segyTraces.begin();
    int firstPos;
    int lastPos;
    int length;
    QVector< QVector<float> > result;
    segyTrace *trace;
    for (; tracesIterator!=segyTraces.end();++tracesIterator)
    {
        trace = *tracesIterator;
        if (trace->getOffset() > minOffset && trace->getOffset()<maxOffset && trace->getTraceIdCode()!=9)
        {
            if (v1!=0)
            {
                firstPos = (minTime + (trace->getOffset() - minOffset)/v1)/(sampleInterval/1000);
            }
            else
            {
                firstPos = minTime/(sampleInterval/1000);
            }

            if (v2!=0)
            {
                lastPos = (maxTime + (trace->getOffset() - minOffset)/v2)/(sampleInterval/1000);
            }
            else
            {
                lastPos = maxTime/(sampleInterval/1000);
            }

            length = lastPos-firstPos+1;
            if (muted && trace->getTraceIdCode()!=2)
            {
                result.append(trace->getData().mid(firstPos,length));
            }
            else if (!muted)
            {
                result.append(trace->getData().mid(firstPos,length));
            }

        }
    }
    return result;
}*/

//проверка трасс на эксклюзивную зону
QStringList SegyFile::deleteFileInExclusions(QVector<Exclusion*> excl)
{
    QStringList result;
    QVector<segyTrace*>::iterator traceIterator = segyTraces.begin();
    segyTrace *trace;
    for (; traceIterator!=segyTraces.end();++traceIterator)
    {
        trace = *traceIterator;
        foreach (Exclusion *ex, excl) {
            if (ex->checkPoint(trace->getReceiverX(),trace->getReceiverY()))
            {

                result.append(QString("%1\t%2\t%3\t%4\t\%5\n").arg(trace->getFileNum()).arg(trace->getReceiverLinePoint()).arg(trace->getReceiverX()).arg(trace->getReceiverY()).arg(trace->getReceiverZ()));
                traceIterator = segyTraces.erase(traceIterator);
                traceIterator--;
                break;
            }
        }
     }
     return result;

}

//проверяем находится ли трасса в экслюзивной зоне
bool SegyFile::traceInExclusion(segyTrace *tr, QVector<Exclusion *> excl)
{
    foreach (Exclusion *ex, excl) {
        if (ex->checkPoint(tr->getReceiverX(),tr->getReceiverY()))
        {
            return true;
        }
    }
    return false;
}


//удаляем пп в эксклюзивной зоне по списку
QStringList SegyFile::deleteReceiversInExclusions(QVector<int> exclReceivers)
{
    QStringList result;
    QVector<segyTrace*>::iterator traceIterator = segyTraces.begin();
    segyTrace *trace;
    for (; traceIterator!=segyTraces.end();++traceIterator)
    {
        trace = *traceIterator;
        trace->getShotPointNum();

        if (exclReceivers.indexOf(trace->getReceiverLinePoint().toInt())!=-1)
        {
            result.append(QString("%1\t%2\t%3\t%4\t\%5\n").arg(trace->getFileNum()).arg(trace->getReceiverLinePoint()).arg(trace->getReceiverX()).arg(trace->getReceiverY()).arg(trace->getReceiverZ()));
            traceIterator = segyTraces.erase(traceIterator);
            traceIterator--;
        }
     }
     return result;
}

//удаляем трассы в соответсвии с заданными настройками
void SegyFile::deleteBadTraces(const bool &muted, const bool &badTests, const double minAmpl)
{
    QVector<segyTrace*>::iterator traceIterator = segyTraces.begin();
    segyTrace *trace;
    for (; traceIterator!=segyTraces.end();++traceIterator)
    {
        trace = *traceIterator;
        if (trace->getTraceIdCode()==9)
        {
            traceIterator = segyTraces.erase(traceIterator);
            traceIterator--;
            continue;
        }
        if (muted && trace->getTraceIdCode()==2)
        {
            traceIterator = segyTraces.erase(traceIterator);
            traceIterator--;
            continue;
        }
        if (badTests && (trace->getDeviceTraceIdentifier()==1))
        {
            traceIterator = segyTraces.erase(traceIterator);
            traceIterator--;
            continue;
        }
        if (trace->getMaxValue()<minAmpl)
        {
            traceIterator = segyTraces.erase(traceIterator);
            traceIterator--;
            continue;
        }
    }
}
// получаем ПВ

int SegyFile::getSP()
{
    return segyTraces.value(0)->getSP();
}
int SegyFile::getgetShotPointNum()
{
    return segyTraces.value(0)->getShotPointNum();
}

uint SegyFile::getSourceX(const uint &trNb)
{
    return segyTraces.value(trNb)->getSourceX();
}
uint SegyFile::getSourceY(const uint &trNb)
{
    return segyTraces.value(trNb)->getSourceY();
}
uint SegyFile::getSourceZ(const uint &trNb)
{
    return segyTraces.value(trNb)->getSourceZ();
}
/*void SegyFile::writeNoiseDomFreq(const QString &filePath)
{
    QFile outFile;
    outFile.setFileName(filePath);
    outFile.open(QIODevice::Append|QIODevice::WriteOnly);
    QTextStream stream(&outFile);
    foreach (segyTrace *trace, segyTraces) {
        if (trace->getTraceIdCode()!=9)
        stream<<trace->getFileNum()<<"\t"<<trace->getSourceLinePoint()<<"\t"<<trace->getReceiverLinePoint()<<"\t"<<trace->getSourceX()<<"\t"<<trace->getSourceY() <<"\t"<<trace->getReceiverX()<<"\t"<<trace->getReceiverY()<<"\t"<<trace->noiseSpectrum()<<"\n";
    }
    outFile.close();
}*/

bool SegyFile::setTemplates(XFile *xps)
{
    if (xps->getTemplates()->last().lastChannel > numOfDataTraces)
    {
        return false;
    }
    else
    {
        uint currentTrace= numOfAuxiliaryTraces-1;
        uint currentReceiver;
        uint currentLine;
        Template templ;// = xps->getTemplates()->dequeue();
        for (currentTrace=0; currentTrace<numOfAuxiliaryTraces;++currentTrace)
        {
            segyTraces.value(currentTrace)->setSP(xps->getLine());
            segyTraces.value(currentTrace)->setShotPointNum(xps->getPoint());
        }
        while (!xps->getTemplates()->isEmpty()) {
            templ = xps->getTemplates()->dequeue();
            currentLine =templ.receiverLine;
            currentReceiver =templ.firstReceiver;
            for(;currentTrace<templ.firstChannel+numOfAuxiliaryTraces-1;++currentTrace)
            {
                segyTraces.value(currentTrace)->setSP(xps->getLine());
                segyTraces.value(currentTrace)->setShotPointNum(xps->getPoint());
                segyTraces.value(currentTrace)->setTraceSequenceNumWithinLine(0);
                segyTraces.value(currentTrace)->setTraceSequenceNumWithinReel(0);
                segyTraces.value(currentTrace)->setTraceId(3);
                segyTraces.value(currentTrace)->setReceiverX(0);
                segyTraces.value(currentTrace)->setReceiverY(0);
                segyTraces.value(currentTrace)->setReceiverZ(0);
            }
            for (;currentTrace<templ.lastChannel+numOfAuxiliaryTraces;++currentTrace)
            {
                segyTraces.value(currentTrace)->setSP(xps->getLine());
                segyTraces.value(currentTrace)->setShotPointNum(xps->getPoint());
                segyTraces.value(currentTrace)->setTraceSequenceNumWithinLine(currentLine);
                segyTraces.value(currentTrace)->setTraceSequenceNumWithinReel(currentReceiver);
                currentReceiver++;
            }
        }
        for(;currentTrace<numOfAuxiliaryTraces+numOfDataTraces;++currentTrace)
        {
            segyTraces.value(currentTrace)->setSP(xps->getLine());
            segyTraces.value(currentTrace)->setShotPointNum(xps->getPoint());
            segyTraces.value(currentTrace)->setTraceSequenceNumWithinLine(0);
            segyTraces.value(currentTrace)->setTraceSequenceNumWithinReel(0);
            segyTraces.value(currentTrace)->setTraceId(3);
            segyTraces.value(currentTrace)->setReceiverX(0);
            segyTraces.value(currentTrace)->setReceiverY(0);
            segyTraces.value(currentTrace)->setReceiverZ(0);
        }
        return true;
    }
}
