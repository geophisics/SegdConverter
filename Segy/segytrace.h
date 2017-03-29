#ifndef SEGYTRACE_H
#define SEGYTRACE_H

#include <QObject>
#include <QVector>
#include <Segd/segdfile.h>
#include <SUB/point.h>
class QFile;
class segyTrace : public QObject
{
    Q_OBJECT
public:
    explicit segyTrace(QObject *parent = 0);
    explicit segyTrace(const QByteArray &bArr,const int &dataType, QObject *parent =0);
    explicit segyTrace (SegdFile *segd, const int &traceNum, QObject *parent =0);

    int getFirstBrake(const int &percent, const bool &polar);
    int getFileNum();
    void writeSegyTrace(QFile &fileOut, const int dataFormat);
    void writeSegyTrace(QFile &fileOut, const int dataFormat, const quint32 num);
    //void writeSegyTrace(QFile &fileOut, const int dataFormat,);
private:
    quint32 traceSequenceNumWithinLine;
    quint32 traceSequenceNumWithinReel;
    quint32 ffid;
    quint32 traceNumWithinFieldRecord;
    quint32 sp;
    quint32 cdp;
    quint32 traceNumber;
    quint16 traceIdCode;
    quint16 numOfVerticallySummedTraces;
    quint16 numOfHorizontallyStackedTraces;
    quint16 dataUse;
    uint offset;
    uint receiverElevation;
    uint sourceElevation;
    uint sourceDepth;
    uint receiverDatumElevation;
    uint sourceDatumElevation;
    uint sourceWaterDepth;
    uint receiverWaterDepth;
    qint16 scallerToElevetionsDepths;
    qint16 scallerToCoordinates;
    uint sourceX;
    uint sourceY;
    uint receiverX;
    uint receiverY;
    quint16 coordinateUnits;
    quint16 weatheringVelocity;
    quint16 subweatheringVelocity;
    quint16 upholeTimeSource;
    quint16 upholeTimeReceiver;
    quint16 sourceStaticCorrection;
    quint16 receiverStaticCorrection;
    quint16 totalStaticApplied;
    quint16 lagTimeA;
    quint16 lagTimeB;
    quint16 delayRecordingTime;
    quint16 muteTimeStart;
    quint16 muteTimeEnd;
    quint16 numOfSamples;
    quint16 sampleInterval;
    quint16 gainType;
    quint16 instrumentGain;
    quint16 instrumentEarlyGainInDb;
    quint16 correlated;
    quint16 sweepFreqAtStart;
    quint16 sweepFreqAtEnd;
    quint16 sweepLength;
    quint16 sweepType;
    quint16 sweepTraceTaperLenghtAtStart;
    quint16 sweepTraceTaperLenghtAtEnd;
    quint16 taperType;
    quint16 aliasFilterFreq;
    quint16 aliasFilterSlope;
    quint16 notchFilterFreq;
    quint16 notchFilterSlope;
    quint16 lowCutFreq;
    quint16 highCutFreq;
    quint16 lowCutSlope;
    quint16 highCutSlope;
    quint16 year;
    quint16 day;
    quint16 hour;
    quint16 minute;
    quint16 second;
    quint16 timeBasis;
    quint16 traceWeighting;
    quint16 geophoneGroupNumberOfRoll;
    quint16 geophoneGroupNumberOfTrace1;
    quint16 geophoneGroupNumberOfLastTrace;
    quint16 gapSize;
    quint16 overtravelAssocTaper;
    uint cdpX;
    uint cdpY;
    uint inlineNum;
    uint crosslineNum;
    uint shotPointNum;
    quint16 shotPointScalar;
    quint16 traceValueMeasurementUnit;
    uint trunsductionConstant;
    quint16 trunsductionUnits;
    quint16 deviceTraceIdentifier;
    quint16 scalarToTimes;
    quint16 sourceTypeOrientation;
    uint sourceEnergyDirection;
    uint sourceMeasurement;
    quint16 sourceMeasurementUnit;
    QVector<float> traceData;
signals:

public slots:
private:
   int  getNoise(const int &sampleNum);
   int getNoise();
   bool changeSign(const int &a, const int &b);
   float toFloat(const QByteArray &arr);
   float IbmToFloat(const QByteArray &arr);
   double xRotating(const float &azX, const int &X, const int &Y);
   double yRotating(const float &azY, const int &X, const int &Y);
public:
   QVector<float> getData();
   float getMaxValue();
   quint16 getTraceIdCode();
   uint getOffset();
   QString getReceiverLinePoint();
   QString getSourceLinePoint();
   int getSP();
   int getShotPointNum();
   int getLinePoint();
   void setSP(const uint &sourceLine);
   void setShotPointNum(const uint &sourcePoint );
   void setTraceSequenceNumWithinLine(const uint &receiverLine);
   void setTraceSequenceNumWithinReel(const uint &receiverPoint);
   void setTraceId(const quint16 &id);
   uint getSourceX();
   uint getSourceY();
   uint getSourceZ();
   uint getReceiverX();
   uint getReceiverY();
   uint getReceiverZ();
   void setReceiverX(uint x);
   void setReceiverY(uint y);
   void setReceiverZ(uint z);
   void setSourceX(uint x);
   void setSourceY(uint y);
   void setSourceZ(uint z);
   void countGeometry();
   quint16 getDeviceTraceIdentifier();

   //----------------------------------------------------------
   //QString noiseSpectrum();
   //----------------------------------------------------------
};

#endif // SEGYTRACE_H
