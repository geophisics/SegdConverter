#ifndef EXTENDEDHEADER_H
#define EXTENDEDHEADER_H
#include <QtCore>

class ExtendedHeader
{
private:
   quint32 acquisitionLenght;
   quint32 sampleRate;
   quint32 totalNumberOfTraces;
   quint32 numberOfAuxes;
   quint32 numberOfSeisTraces;
   quint32 numberOfDeadSeisTraces;
   quint32 numberOfLiveSeisTraces;
   quint32 typeOfSource;
   quint32 samplesPerTrace;
   quint32 shotNumber;
   float tbWindow;
   quint32 testRecordType;
   quint32 swathFirstLine;
   quint32 swathFirstNumber;
   quint32 spreadNumber;
   quint32 spreadType;
   quint32 timebreak;
   quint32 upholeTime;
   quint32 blasterId;
   quint32 blasterStatus;
   quint32 refractionDelay;
   int tbToT0Time;
   quint32 internalTimeBreak;
   quint32 prestackWithinFieldUnits;
   quint32 noiseEliminationType;
   quint32 lowTracePercentage;
   quint32 lowTraceValue;
   quint32 numberOfWindows;
   quint32 historicEditingType;
   quint32 noisyTracePercentage;
   quint32 historicRange;
   quint32 historicTaperLength;
   quint32 threshold;
   quint32 historicThresholdInitValue;
   quint32 historicZeroingLength;
   quint32 typeOfProcess;
   int acquisitionTypeTables[32];
   int thresholdTypeTables[32];
   quint32 stackingFold;
   quint32 recordLength;
   quint32 autocorrelationPeakTime;
   quint32 correlationPilotNum;
   quint32 pilotLength;
   quint32 sweepLength;
   quint32 acquisitionNumber;
   float maxOfMaxAux;
   float maxOfMaxSeis;
   quint32 dumpStackingFold;
   QString tapeLabel;
   quint32 tapeNumber;
   QString softwareVersion;
   QString date;
   double sourceEasting;
   double sourceNorthing;
   float sourceElevation;
   quint32 slipSweepModeUsed;
   quint32 filesPerTape;
   quint32 fileCount;
   QString acquisitionErrorDesc;
   quint32 filterType;
   quint32 stackIsDumped;
   quint32 stackSign;
   quint32 prmTiltCorrectionUsed;
   QString swathName;
   quint32 operatingMode;
   quint32 noLog;
   quint32 listeningTime;
   quint32 typeOfDump;
   quint32 swathId;
   quint32 dcOffsetRemovalDisabled;
   quint64 gpsTimebreak;

public:
    ExtendedHeader();
    ExtendedHeader(const QByteArray &arrIn);
    int getSampleRate();
    int getLength();
    int getSamplesPerTrace();
    int getUpholeTime();
    int getNumberOfAuxes();
    int getNumberOfSeis();
    int getSweepLength();
    quint32 getTypeOfProcess();
    quint32 getTypeOfSource();
    double getX();
    double getY();
    double getZ();
    quint32 getTapeNumber();
    quint32 getDumpStackingFold();

};

#endif // EXTENDEDHEADER_H
