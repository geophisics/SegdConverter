#include "extendedheader.h"
#include "segdfile.h"
ExtendedHeader::ExtendedHeader()
{

}

ExtendedHeader::ExtendedHeader(const QByteArray &arrIn)
{
    acquisitionLenght=SegdFile::toUint(arrIn.mid(0,4));
    sampleRate=SegdFile::toUint(arrIn.mid(4,4));
    totalNumberOfTraces=SegdFile::toUint(arrIn.mid(8,4));
    numberOfAuxes=SegdFile::toUint(arrIn.mid(12,4));
    numberOfSeisTraces=SegdFile::toUint(arrIn.mid(16,4));
    numberOfDeadSeisTraces=SegdFile::toUint(arrIn.mid(20,4));
    numberOfLiveSeisTraces=SegdFile::toUint(arrIn.mid(24,4));
    typeOfSource=SegdFile::toUint(arrIn.mid(28,4));
    samplesPerTrace=SegdFile::toUint(arrIn.mid(32,4));
    shotNumber=SegdFile::toUint(arrIn.mid(36,4));
    tbWindow=SegdFile::toFloat(arrIn.mid(40,4));
    testRecordType=SegdFile::toUint(arrIn.mid(44,4));
    swathFirstLine=SegdFile::toUint(arrIn.mid(48,4));
    swathFirstNumber=SegdFile::toUint(arrIn.mid(52,4));
    spreadNumber=SegdFile::toUint(arrIn.mid(56,4));
    spreadType=SegdFile::toUint(arrIn.mid(60,4));
    timebreak=SegdFile::toUint(arrIn.mid(64,4));
    upholeTime=SegdFile::toUint(arrIn.mid(68,4));
    blasterId=SegdFile::toUint(arrIn.mid(72,4));
    blasterStatus=SegdFile::toUint(arrIn.mid(76,4));
    refractionDelay=SegdFile::toUint(arrIn.mid(80,4));
    tbToT0Time=SegdFile::toUint(arrIn.mid(84,4));
    internalTimeBreak=SegdFile::toUint(arrIn.mid(88,4));
    prestackWithinFieldUnits=SegdFile::toUint(arrIn.mid(92,4));
    noiseEliminationType=SegdFile::toUint(arrIn.mid(96,4));
    lowTracePercentage=SegdFile::toUint(arrIn.mid(100,4));
    lowTraceValue=SegdFile::toUint(arrIn.mid(104,4));
    numberOfWindows=SegdFile::toUint(arrIn.mid(108,4));
    historicEditingType=SegdFile::toUint(arrIn.mid(112,4));
    noisyTracePercentage=SegdFile::toUint(arrIn.mid(116,4));
    historicRange=SegdFile::toUint(arrIn.mid(120,4));
    historicTaperLength=SegdFile::toUint(arrIn.mid(124,4));
    threshold=SegdFile::toUint(arrIn.mid(128,4));
    historicThresholdInitValue=SegdFile::toUint(arrIn.mid(132,4));
    historicZeroingLength=SegdFile::toUint(arrIn.mid(136,4));
    typeOfProcess=SegdFile::toUint(arrIn.mid(140,4));
    for (int i=0;i<32;i++)
    {
        acquisitionTypeTables[i]=SegdFile::toUint(arrIn.mid(144+i*4,4));
    }
    for (int i=0;i<32;i++)
    {
        thresholdTypeTables[i]=SegdFile::toUint(arrIn.mid(272+i*4,4));
    }
    stackingFold=SegdFile::toUint(arrIn.mid(400,4));
    recordLength=SegdFile::toUint(arrIn.mid(484,4));
    autocorrelationPeakTime=SegdFile::toUint(arrIn.mid(488,4));
    correlationPilotNum=SegdFile::toUint(arrIn.mid(496,4));
    pilotLength=SegdFile::toUint(arrIn.mid(500,4));
    sweepLength=SegdFile::toUint(arrIn.mid(504,4));
    acquisitionNumber=SegdFile::toUint(arrIn.mid(508,4));
    maxOfMaxAux=SegdFile::toFloat(arrIn.mid(512,4));
    maxOfMaxSeis=SegdFile::toFloat(arrIn.mid(516,4));
    dumpStackingFold=SegdFile::toUint(arrIn.mid(520,4));
    tapeLabel=QString(arrIn.mid(524,16));
    tapeNumber=SegdFile::toUint(arrIn.mid(540,4));
    softwareVersion=QString(arrIn.mid(544,16));
    date=QString(arrIn.mid(560,12));
    sourceEasting=SegdFile::toDouble(arrIn.mid(572,8));
    sourceNorthing=SegdFile::toDouble(arrIn.mid(580,8));
    sourceElevation=SegdFile::toFloat(arrIn.mid(588,4));
    slipSweepModeUsed=SegdFile::toUint(arrIn.mid(592,4));
    filesPerTape=SegdFile::toUint(arrIn.mid(596,4));
    fileCount=SegdFile::toUint(arrIn.mid(600,4));
    acquisitionErrorDesc=QString(arrIn.mid(604,160));
    filterType=SegdFile::toUint(arrIn.mid(764,4));
    stackIsDumped=SegdFile::toUint(arrIn.mid(768,4));
    stackSign=SegdFile::toUint(arrIn.mid(772,4));
    prmTiltCorrectionUsed=SegdFile::toUint(arrIn.mid(776,4));
    swathName=QString(arrIn.mid(780,64));
    operatingMode=SegdFile::toUint(arrIn.mid(844,4));
    noLog=SegdFile::toUint(arrIn.mid(852,4));
    listeningTime=SegdFile::toUint(arrIn.mid(856,4));
    typeOfDump=SegdFile::toUint(arrIn.mid(860,4));
    swathId=SegdFile::toUint(arrIn.mid(868,4));
    dcOffsetRemovalDisabled=SegdFile::toUint(arrIn.mid(872,4));
    gpsTimebreak=SegdFile::toUint64(arrIn.mid(876,8));
}
int ExtendedHeader::getSamplesPerTrace()
{
    return samplesPerTrace;
}

int ExtendedHeader::getUpholeTime()
{
    return upholeTime;
}
double ExtendedHeader::getX()
{
    return sourceEasting;
}
double ExtendedHeader::getY()
{
    return sourceNorthing;
}
double ExtendedHeader::getZ()
{
    return sourceElevation;
}
quint32 ExtendedHeader::getTypeOfSource()
{
    return typeOfSource;
}
quint32 ExtendedHeader::getTapeNumber()
{
    return tapeNumber;
}
quint32 ExtendedHeader::getDumpStackingFold()
{
    return dumpStackingFold;
}
int ExtendedHeader::getSampleRate()
{
    return sampleRate;
}
int ExtendedHeader::getLength()
{
    return acquisitionLenght;
}
int ExtendedHeader::getNumberOfAuxes()
{
    return numberOfAuxes;
}
int ExtendedHeader::getNumberOfSeis()
{
    return numberOfSeisTraces;
}
int ExtendedHeader::getSweepLength()
{
    return sweepLength;
}
quint32 ExtendedHeader::getTypeOfProcess()
{
    return typeOfProcess;
}
