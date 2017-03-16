#include "segytrace.h"
#include "segyfile.h"
#include "math.h"
#include <QIODevice>
#include <QFile>
#include <QDataStream>
#include <QLocale>
#include "limits.h"
#include "math.h"
#include <QDir>
#include <QMessageBox>
#include <math.h>

segyTrace::segyTrace(QObject *parent) : QObject(parent)
{

}
segyTrace::segyTrace(const QByteArray &bArr, const int &dataType, QObject *parent):QObject(parent)
{

    traceSequenceNumWithinLine=SegyFile::toUint(bArr.mid(0,4));
    traceSequenceNumWithinReel=SegyFile::toUint(bArr.mid(4,4));
    ffid=SegyFile::toUint(bArr.mid(8,4));
    traceNumWithinFieldRecord=SegyFile::toUint(bArr.mid(12,4));
    sp=SegyFile::toUint(bArr.mid(16,4));
    cdp=SegyFile::toUint(bArr.mid(20,4));
    traceNumber=SegyFile::toUint(bArr.mid(24,4));
    traceIdCode=SegyFile::toUShort(bArr.mid(28,2));
    numOfVerticallySummedTraces=SegyFile::toUShort(bArr.mid(30,2));
    numOfHorizontallyStackedTraces=SegyFile::toUShort(bArr.mid(32,2));
    dataUse=SegyFile::toUShort(bArr.mid(34,2));
    offset=SegyFile::toUint(bArr.mid(36,4));
    receiverElevation=SegyFile::toUint(bArr.mid(40,4));
    sourceElevation=SegyFile::toUint(bArr.mid(44,4));
    sourceDepth=SegyFile::toUint(bArr.mid(48,4));
    receiverDatumElevation=SegyFile::toUint(bArr.mid(52,4));
    sourceDatumElevation=SegyFile::toUint(bArr.mid(56,4));
    sourceWaterDepth=SegyFile::toUint(bArr.mid(60,4));
    receiverWaterDepth=SegyFile::toUint(bArr.mid(64,4));
    scallerToElevetionsDepths=SegyFile::toShort(bArr.mid(68,2));
    scallerToCoordinates=SegyFile::toShort(bArr.mid(70,2));
    sourceX=SegyFile::toUint(bArr.mid(72,4));
    sourceY=SegyFile::toUint(bArr.mid(76,4));
    receiverX=SegyFile::toUint(bArr.mid(80,4));
    receiverY=SegyFile::toUint(bArr.mid(84,4));
    coordinateUnits=SegyFile::toUShort(bArr.mid(88,2));
    weatheringVelocity=SegyFile::toUShort(bArr.mid(90,2));
    subweatheringVelocity=SegyFile::toUShort(bArr.mid(92,2));
    upholeTimeSource=SegyFile::toUShort(bArr.mid(94,2));
    upholeTimeReceiver=SegyFile::toUShort(bArr.mid(96,2));
    sourceStaticCorrection=SegyFile::toUShort(bArr.mid(98,2));
    receiverStaticCorrection=SegyFile::toUShort(bArr.mid(100,2));
    totalStaticApplied=SegyFile::toUShort(bArr.mid(102,2));
    lagTimeA=SegyFile::toUShort(bArr.mid(104,2));
    lagTimeB=SegyFile::toUShort(bArr.mid(106,2));
    delayRecordingTime=SegyFile::toUShort(bArr.mid(108,2));
    muteTimeStart=SegyFile::toUShort(bArr.mid(110,2));
    muteTimeEnd=SegyFile::toUShort(bArr.mid(112,2));
    numOfSamples=SegyFile::toUShort(bArr.mid(114,2));
    sampleInterval=SegyFile::toUShort(bArr.mid(116,2));
    gainType=SegyFile::toUShort(bArr.mid(118,2));
    instrumentGain=SegyFile::toUShort(bArr.mid(120,2));
    instrumentEarlyGainInDb=SegyFile::toUShort(bArr.mid(122,2));;
    correlated=SegyFile::toUShort(bArr.mid(124,2));
    sweepFreqAtStart=SegyFile::toUShort(bArr.mid(126,2));
    sweepFreqAtEnd=SegyFile::toUShort(bArr.mid(128,2));
    sweepLength=SegyFile::toUShort(bArr.mid(130,2));
    sweepType=SegyFile::toUShort(bArr.mid(132,2));
    sweepTraceTaperLenghtAtStart=SegyFile::toUShort(bArr.mid(134,2));
    sweepTraceTaperLenghtAtEnd=SegyFile::toUShort(bArr.mid(136,2));
    taperType=SegyFile::toUShort(bArr.mid(138,2));
    aliasFilterFreq=SegyFile::toUShort(bArr.mid(140,2));
    aliasFilterSlope=SegyFile::toUShort(bArr.mid(142,2));
    notchFilterFreq=SegyFile::toUShort(bArr.mid(144,2));
    notchFilterSlope=SegyFile::toUShort(bArr.mid(146,2));
    lowCutFreq=SegyFile::toUShort(bArr.mid(148,2));
    highCutFreq=SegyFile::toUShort(bArr.mid(150,2));
    lowCutSlope=SegyFile::toUShort(bArr.mid(152,2));
    highCutSlope=SegyFile::toUShort(bArr.mid(154,2));
    year=SegyFile::toUShort(bArr.mid(156,2));
    day=SegyFile::toUShort(bArr.mid(158,2));
    hour=SegyFile::toUShort(bArr.mid(160,2));
    minute=SegyFile::toUShort(bArr.mid(162,2));
    second=SegyFile::toUShort(bArr.mid(164,2));
    timeBasis=SegyFile::toUShort(bArr.mid(166,2));
    traceWeighting=SegyFile::toUShort(bArr.mid(168,2));
    geophoneGroupNumberOfRoll=SegyFile::toUShort(bArr.mid(170,2));
    geophoneGroupNumberOfTrace1=SegyFile::toUShort(bArr.mid(172,2));
    geophoneGroupNumberOfLastTrace=SegyFile::toUShort(bArr.mid(174,2));
    gapSize=SegyFile::toUShort(bArr.mid(176,2));
    overtravelAssocTaper=SegyFile::toUShort(bArr.mid(178,2));
    cdpX=SegyFile::toUint(bArr.mid(180,4));
    cdpY=SegyFile::toUint(bArr.mid(184,4));
    inlineNum=SegyFile::toUint(bArr.mid(188,4));
    crosslineNum=SegyFile::toUint(bArr.mid(192,4));
    shotPointNum=SegyFile::toUint(bArr.mid(196,4));
    shotPointScalar=SegyFile::toUShort(bArr.mid(200,2));
    traceValueMeasurementUnit=SegyFile::toUShort(bArr.mid(202,2));
    trunsductionConstant=SegyFile::toUint(bArr.mid(204,4));
    trunsductionUnits=SegyFile::toUShort(bArr.mid(210,2));
    deviceTraceIdentifier=SegyFile::toUShort(bArr.mid(212,2));
    scalarToTimes=SegyFile::toUShort(bArr.mid(214,2));
    sourceTypeOrientation=SegyFile::toUShort(bArr.mid(216,2));
    sourceEnergyDirection=SegyFile::toUint(bArr.mid(218,4));
    sourceMeasurement=SegyFile::toUint(bArr.mid(224,4));
    sourceMeasurementUnit=SegyFile::toUShort(bArr.mid(230,2));
    for (int i =0; i<numOfSamples;i++)
    {
        switch (dataType) {
        case 2:
            //traceData.append(IbmToFloat(bArr.mid(240+i*4,4)));
            traceData.append(SegyFile::toInt(bArr.mid(240+i*4,4)));
            break;
        case 3:
            traceData.append(SegyFile::toInt(bArr.mid(240+i*2,2)));
            break;
        case 4:
            traceData.append(toFloat(bArr.mid(240+i*4,4)));
            break;
        case 1:
            traceData.append(IbmToFloat(bArr.mid(240+i*4,4)));
            break;
        case 8:
            traceData.append(SegyFile::toInt(bArr.mid(240+i*1,1)));
            break;
        default:
            break;
        }
        //traceData.append(SegyFile::toInt(bArr.mid(240+i*4,4)));
    }
}

segyTrace::segyTrace(SegdFile *segd, const int &traceNum, QObject *parent) : QObject(parent)
{
    traceSequenceNumWithinLine = segd->getTrace(traceNum)->getExtensionOne()->getReceiverLineNum();// traceNum;
    traceSequenceNumWithinReel = segd->getTrace(traceNum)->getExtensionOne()->getReceiverPointNum();
    ffid=segd->getTrace(traceNum)->getTraceHeader()->getExtendedFileNum();
    traceNumWithinFieldRecord = traceNum+1;
    sp = segd->getGeneralThree().getLineNumber();
    cdp = 0;
    traceNumber = traceNum+1;
    if (segd->getTrace(traceNum)->getTraceHeader()->getTraceEdit() == 2)
    {
        traceIdCode = 2;
    }
    else
    {
        if (segd->getTrace(traceNum)->getExtensionSeven()->getChannelType() == 9)
        {
            traceIdCode = 9;
        }
        else {
            traceIdCode = 1;
        }
    }
    numOfVerticallySummedTraces =1;
    numOfHorizontallyStackedTraces = 1;
    dataUse =1;
    receiverElevation = segd->getTrace(traceNum)->getExtensionTwo()->getZ();
    sourceElevation =segd->getExtendedHeader().getZ();
    sourceDepth =0;
    sourceDatumElevation =0;
    receiverDatumElevation =0;
    sourceWaterDepth =0;
    receiverWaterDepth =0;
    scallerToElevetionsDepths = 1;
    scallerToCoordinates =1;
    sourceX =segd->getExtendedHeader().getX();
    sourceY =segd->getExtendedHeader().getY();
    receiverX = segd->getTrace(traceNum)->getExtensionTwo()->getX();
    receiverY =segd->getTrace(traceNum)->getExtensionTwo()->getY();
    coordinateUnits =1;
    weatheringVelocity = 0;
    subweatheringVelocity =0;
    upholeTimeSource = segd->getExtendedHeader().getUpholeTime();
    upholeTimeReceiver = 0;
    sourceStaticCorrection =0;
    receiverStaticCorrection =0;
    totalStaticApplied =0;
    lagTimeA =0;
    lagTimeB =0;
    delayRecordingTime =0;
    muteTimeStart =0;
    muteTimeEnd =0;
    numOfSamples = segd->getTrace(traceNum)->getExtensionOne()->getSamplesPerTrace();
    sampleInterval = segd->getExtendedHeader().getSampleRate();
    gainType = 0;
    instrumentGain =0;
    correlated =0;
    sweepFreqAtEnd=0;
    sweepFreqAtStart = 0;
    sweepLength = segd->getExtendedHeader().getSweepLength();
    sweepType =0;
    sweepTraceTaperLenghtAtStart =0;
    sweepTraceTaperLenghtAtEnd =0;
    taperType =0;
    aliasFilterFreq=0;
    aliasFilterSlope =0;
    notchFilterFreq=0;
    notchFilterSlope=0;
    lowCutFreq=0;
    lowCutSlope=0;
    highCutFreq=0;
    highCutSlope=0;
    year = segd->getGeneralOne().getYear();
    day = segd->getGeneralOne().getDay();
    hour = segd->getGeneralOne().getHour();
    minute= segd->getGeneralOne().getMinute();
    second = segd->getGeneralOne().getSecond();
    timeBasis = 2;
    traceWeighting =0;
    geophoneGroupNumberOfRoll =0;
    geophoneGroupNumberOfTrace1 =0;
    geophoneGroupNumberOfLastTrace =0;
    gapSize = 0;
    overtravelAssocTaper = 0;
    shotPointNum = segd->getGeneralThree().getPointNumber();
    shotPointScalar =0;
    traceValueMeasurementUnit=0;
    trunsductionConstant =0;
    trunsductionUnits =0;
    //------------------------------------------------
    // если плохие тесты Resistance или Tilt (Res = 9999.0; Tilt =100.0), то deviceTraceIdentifier =1 иначе 0
    if (segd->getTrace(traceNum)->getExtensionThree()->getResistanceValue()==9999.0 || segd->getTrace(traceNum)->getExtensionThree()->getTiltValue()==100.0)
    {
        deviceTraceIdentifier =1;
    }
    else
    {
        deviceTraceIdentifier =0;
    }
    //--------------------------------------------------
    scalarToTimes =0;
    sourceEnergyDirection =0;
    sourceTypeOrientation=0;
    sourceMeasurementUnit=0;
    sourceMeasurement = 0;
    offset=0;
    inlineNum =0;
    crosslineNum=0;
    cdpX=0;
    cdpY =0;
    traceData = segd->getTrace(traceNum)->getTraceData();
}






int segyTrace::getFileNum()
{
    return ffid;
}
int segyTrace::getFirstBrake(const int &percent, const bool &polar)
{
    float maxValue;
    maxValue=getMaxValue();
    /*for (int i=0; i<numOfSamples;++i)
    {
        if (maxValue<abs(traceData.value(i)))
            maxValue=abs(traceData.value(i));
    }*/
    maxValue = maxValue*percent/100;
    int i =0;
    if (polar)
    {
        maxValue = -1*maxValue;
        while (maxValue<traceData.value(i)) {
            i++;
        }
        /*while (traceData.value(i)<0)
        {
            i--;
        }*/
    }
    else
    {
        while (maxValue>traceData.value(i))
        {
            i++;
        }
        /*while (traceData.value(i)>0) {
            i--;
        }*/
    }
    float n=getNoise()*percent/10.0;
    while (abs(traceData.value(i-1))>n && !(changeSign(traceData.value(i),traceData.value(i-1)))) {
        i--;
    }
    return i*sampleInterval/1000;
}
int segyTrace::getNoise(const int &sampleNum)
{
    int value = traceData.value(sampleNum);
    int samplePerehod =sampleNum;

    if (value<0)
    {
        while (traceData.value(samplePerehod) <0) {
            samplePerehod--;
        }
    }
    else {
        while (traceData.value(samplePerehod)>0) {
            samplePerehod--;
        }
    }
    int maxNoise = 0;
    for (int i=0;i<samplePerehod;i++)
    {
        if (abs(traceData.value(i))>maxNoise)
            maxNoise=abs(traceData.value(i));
    }
    return maxNoise;
}
int segyTrace::getNoise()
{
    int avg=0;
    QVector<float>::iterator it1 = traceData.begin();
    for (; it1!=traceData.end();++it1)
    {
        avg+=abs(*it1);
    }
    avg = avg/numOfSamples;
    return abs(avg);
}
bool segyTrace::changeSign(const int &a, const int &b)
{
    if ( (a>0 && b<0) || (a<0 && b>0) )
    {
        return true;
    }
    else {
        return false;
    }
}
void segyTrace::writeSegyTrace(QFile &fileOut, const int dataFormat)
{

    fileOut.open(QIODevice::Append);
    QDataStream dStream(&fileOut);
    dStream<<traceSequenceNumWithinLine<<traceSequenceNumWithinReel<<ffid<<traceNumWithinFieldRecord<<sp
              <<cdp<<traceNumber<<traceIdCode<<numOfVerticallySummedTraces<<numOfHorizontallyStackedTraces<<dataUse
              <<offset<<receiverElevation<<sourceElevation<<sourceDepth<<receiverDatumElevation<<sourceDatumElevation
              <<sourceWaterDepth<<receiverWaterDepth<<scallerToElevetionsDepths<<scallerToCoordinates<<sourceX<<sourceY
              <<receiverX<<receiverY<<coordinateUnits<<weatheringVelocity<<subweatheringVelocity<<upholeTimeSource
              <<upholeTimeReceiver<<sourceStaticCorrection<<receiverStaticCorrection<<totalStaticApplied<<lagTimeA
              <<lagTimeB<<delayRecordingTime<<muteTimeStart<<muteTimeEnd<<numOfSamples<<sampleInterval<<gainType
              <<instrumentGain<<instrumentEarlyGainInDb<<correlated<<sweepFreqAtStart<<sweepFreqAtEnd<<sweepLength<<sweepType
              <<sweepTraceTaperLenghtAtStart<<sweepTraceTaperLenghtAtEnd<<taperType<<aliasFilterFreq<<aliasFilterSlope
              <<notchFilterFreq<<notchFilterSlope<<lowCutFreq<<highCutFreq<<lowCutSlope<<highCutSlope<<year<<day<<hour
              <<minute<<second<<timeBasis<<traceWeighting<<geophoneGroupNumberOfRoll<<geophoneGroupNumberOfTrace1<<geophoneGroupNumberOfLastTrace
              <<gapSize<<overtravelAssocTaper<<cdpX<<cdpY<<inlineNum<<crosslineNum<<shotPointNum<<shotPointScalar<<traceValueMeasurementUnit
              <<trunsductionConstant<<trunsductionUnits<<deviceTraceIdentifier<<scalarToTimes<<sourceTypeOrientation
              <<sourceEnergyDirection<<(quint16)0<<sourceMeasurement<<(quint16)0<<sourceMeasurementUnit;
    int i=0;
    for(i=0;i<10;i++)
    {
        dStream<<quint8(0);
    }
    switch (dataFormat) {
    case 2:
        for(i=0;i<traceData.count();i++)
        {
            dStream<<(int)traceData.value(i);
        }
        break;
    default:
        dStream.setFloatingPointPrecision(QDataStream::SinglePrecision);
        for(i=0;i<traceData.count();i++)
        {
            dStream<<traceData.value(i);
        }
        break;
    }
    fileOut.close();
}

void segyTrace::writeSegyTrace(QFile &fileOut, const int dataFormat, const quint32 num)
{

    fileOut.open(QIODevice::Append);
    QDataStream dStream(&fileOut);
    dStream<<traceSequenceNumWithinLine<<traceSequenceNumWithinReel<<ffid<<traceNumWithinFieldRecord<<sp
              <<cdp<<num<<traceIdCode<<numOfVerticallySummedTraces<<numOfHorizontallyStackedTraces<<dataUse
              <<offset<<receiverElevation<<sourceElevation<<sourceDepth<<receiverDatumElevation<<sourceDatumElevation
              <<sourceWaterDepth<<receiverWaterDepth<<scallerToElevetionsDepths<<scallerToCoordinates<<sourceX<<sourceY
              <<receiverX<<receiverY<<coordinateUnits<<weatheringVelocity<<subweatheringVelocity<<upholeTimeSource
              <<upholeTimeReceiver<<sourceStaticCorrection<<receiverStaticCorrection<<totalStaticApplied<<lagTimeA
              <<lagTimeB<<delayRecordingTime<<muteTimeStart<<muteTimeEnd<<numOfSamples<<sampleInterval<<gainType
              <<instrumentGain<<instrumentEarlyGainInDb<<correlated<<sweepFreqAtStart<<sweepFreqAtEnd<<sweepLength<<sweepType
              <<sweepTraceTaperLenghtAtStart<<sweepTraceTaperLenghtAtEnd<<taperType<<aliasFilterFreq<<aliasFilterSlope
              <<notchFilterFreq<<notchFilterSlope<<lowCutFreq<<highCutFreq<<lowCutSlope<<highCutSlope<<year<<day<<hour
              <<minute<<second<<timeBasis<<traceWeighting<<geophoneGroupNumberOfRoll<<geophoneGroupNumberOfTrace1<<geophoneGroupNumberOfLastTrace
              <<gapSize<<overtravelAssocTaper<<cdpX<<cdpY<<inlineNum<<crosslineNum<<shotPointNum<<shotPointScalar<<traceValueMeasurementUnit
              <<trunsductionConstant<<trunsductionUnits<<deviceTraceIdentifier<<scalarToTimes<<sourceTypeOrientation
              <<sourceEnergyDirection<<(quint16)0<<sourceMeasurement<<(quint16)0<<sourceMeasurementUnit;
    int i=0;
    for(i=0;i<10;i++)
    {
        dStream<<quint8(0);
    }
    switch (dataFormat) {
    case 2:
        for(i=0;i<traceData.count();i++)
        {
            dStream<<(int)traceData.value(i);
        }
        break;
    default:
        dStream.setFloatingPointPrecision(QDataStream::SinglePrecision);
        for(i=0;i<traceData.count();i++)
        {
            dStream<<traceData.value(i);
        }
        break;
    }
    fileOut.close();
}

QVector<float> segyTrace::getData()
{
    return traceData;
}
float segyTrace::getMaxValue()
{
    float maxValue;
    maxValue=0.0;

    for (int i=0; i<numOfSamples;++i)
    {
        if (maxValue<fabs(traceData.value(i)))
            maxValue=fabs(traceData.value(i));
    }
    return maxValue;
}

quint16 segyTrace::getTraceIdCode()
{
    return traceIdCode;
}

float segyTrace::toFloat(const QByteArray &arr)
{
    float result;
    QByteArray tmpArr=arr;
    QDataStream ds(&tmpArr,QIODevice::ReadOnly);
    ds.setFloatingPointPrecision(QDataStream::SinglePrecision);
    ds>>result;
    return result;
}

float segyTrace::IbmToFloat(const QByteArray &arr)
{
    unsigned char c[4];
    for (int i=0; i<arr.count();++i)
    {
        c[i]=arr[i];
    }

       // CONVERT TO FLOAT
       long IntMantissa =
            ((long)c[1] << 16)
          + ((long)c[2] << 8)
          + c[3];

       float  Mantissa  = float(IntMantissa) / float(0x1000000);
       float  PosResult = Mantissa * pow( 16.0, double((c[0] & 0x7F) - 64) );

       if (c[0] & 0x80)
          return -PosResult;
       else
          return PosResult;
}
double segyTrace::xRotating(const float &azX, const int &X, const int &Y)
{
    return X*cos(azX)+Y*sin(azX);
}
double segyTrace::yRotating(const float &azY, const int &X, const int &Y)
{
    return Y*cos(azY)-X*sin(azY);
}

uint segyTrace::getOffset()
{
    return offset;
}

int segyTrace::getSP()
{
    return sp;
}
int segyTrace::getShotPointNum()
{
    return shotPointNum;
}
int segyTrace::getLinePoint()
{
    return sp*10000+shotPointNum;
}
void segyTrace::setSP(const uint &sourceLine)
{
    sp=sourceLine;
}
void segyTrace::setShotPointNum(const uint &sourcePoint)
{
    shotPointNum = sourcePoint;
}
void segyTrace::setTraceSequenceNumWithinLine(const uint &receiverLine)
{
    traceSequenceNumWithinLine = receiverLine;
}
void segyTrace::setTraceSequenceNumWithinReel(const uint &receiverPoint)
{
    traceSequenceNumWithinReel=receiverPoint;
}
void segyTrace::setTraceId(const quint16 &id)
{
    traceIdCode = id;
}
uint segyTrace::getSourceX()
{
    return sourceX;
}
uint segyTrace::getSourceY()
{
    return sourceY;
}
uint segyTrace::getSourceZ()
{
    return sourceElevation;
}

uint segyTrace::getReceiverX()
{
    return receiverX;
}
uint segyTrace::getReceiverY()
{
    return receiverY;
}
uint segyTrace::getReceiverZ()
{
    return receiverElevation;
}
void segyTrace::setSourceX(uint x)
{
    sourceX = x;
}
void segyTrace::setSourceY(uint y)
{
    sourceY = y;
}
void segyTrace::setSourceZ(uint z)
{
    sourceElevation = z;
}
void segyTrace::setReceiverX(uint x)
{
    receiverX = x;
}
void segyTrace::setReceiverY(uint y)
{
    receiverY =y;
}
void segyTrace::setReceiverZ(uint z)
{
    receiverElevation = z;
}

QString segyTrace::getReceiverLinePoint()
{
    return QString::number(traceSequenceNumWithinLine)+QString::number(traceSequenceNumWithinReel);
}
QString segyTrace::getSourceLinePoint()
{
    return QString::number(sp)+QString::number(shotPointNum);
}
quint16 segyTrace::getDeviceTraceIdentifier()
{
    return deviceTraceIdentifier;
}

void segyTrace::countGeometry()
{
    int xDif = sourceX-receiverX;
    xDif = pow(xDif,2);
    int yDif =sourceY - receiverY;
    yDif = pow(yDif,2);
    offset = sqrt(xDif + yDif);
    QSettings *settings = new QSettings (QCoreApplication::applicationDirPath()+QDir::separator()+"config.ini",QSettings::IniFormat,this);
    settings->beginGroup("/BinningSettings");
    QString str = settings->value("/Xo","0").toString();
    double xOrigin = str.toDouble();
    str = settings->value("/Yo","0").toString();
    double yOrigin = str.toDouble();
    str = settings->value("/dX","25").toString();
    double dX = str.toFloat();
    str = settings->value("/dY","25").toString();
    double dY = settings->value("/dY","25").toFloat();
    int i0 = settings->value("/Io","1001").toInt();
    int j0= settings->value("/Jo","1001").toFloat();
    str=settings->value("/azX","0").toString();
    double azX = str.toDouble();
    str = settings->value("/azY","0").toString();
    double azY = str.toDouble();
    settings->endGroup();
    int xSourceReceiver = (sourceX + receiverX)/2;
    int ySourceReceiver = (sourceY + receiverY)/2;
    azX = M_PI_2 - azX;
    azY = M_PI_2 - azY;
    inlineNum =roundl((((xSourceReceiver-xOrigin)/sin(azX) - (ySourceReceiver-yOrigin)/cos(azX))/(sin(azY)/sin(azX)-cos(azY)/cos(azX)))/dY) + i0;
    crosslineNum =roundl((((xSourceReceiver-xOrigin)/sin(azY) - (ySourceReceiver-yOrigin)/cos(azY))/(sin(azX)/sin(azY)-cos(azX)/cos(azY)))/dX) +j0;
    cdpX = (crosslineNum-j0)*dX*sin(azX)+(inlineNum-i0)*dY*sin(azY)+xOrigin;
    cdpY = (crosslineNum-j0)*dX*cos(azX)+(inlineNum-i0)*dY*cos(azY)+yOrigin;
}


