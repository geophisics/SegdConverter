#ifndef TRACEEXTENSIONONE_H
#define TRACEEXTENSIONONE_H
#include <QtCore>

class TraceExtensionOne
{
private:
    quint32 receiverLineNum;
    quint32 receiverPointNum;
    quint32 receiverPointIndex;
    quint32 samplesPerTrace;
    quint32 extendedReceiverLineNumber;
    quint32 extendedReceiverPointNumber;
    quint32 sensorType;
    friend class SegdTrace;
public:
    TraceExtensionOne();
    TraceExtensionOne(const QByteArray &arrIn);
    int getLinePointNum();
    quint32 getReceiverLineNum();
    quint32 getReceiverPointNum();
    quint32 getSamplesPerTrace();
};

#endif // TRACEEXTENSIONONE_H
