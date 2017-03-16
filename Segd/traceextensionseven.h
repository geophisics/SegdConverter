#ifndef TRACEEXTENSIONSEVEN_H
#define TRACEEXTENSIONSEVEN_H
#include <QtCore>



class TraceExtensionSeven
{
private:
    quint8 controlUnitType;
    quint32 controlUnitSerialNumber;
    quint8 channelGainScale;
    quint8 channelFilter;
    quint8 overscaled;
    quint8 channelEditedStatus;
    float sampleToMvFactor;
    quint8 numberOfStacksNoisy;
    quint8 numberOfStacksLow;
    quint8 channelType;
    quint8 channelProcess;
    float traceMaxValue;
    quint32 traceMaxTime;
    quint32 numberOfInterpolations;
    quint32 dcOffset;
    friend class SegdTrace;
public:
    TraceExtensionSeven();
    TraceExtensionSeven(const QByteArray &arrIn);
    quint8 getChannelType();
    float getSampleToMvFactor();
};

#endif // TRACEEXTENSIONSEVEN_H
