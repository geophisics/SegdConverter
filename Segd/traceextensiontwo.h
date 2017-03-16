#ifndef TRACEEXTENSIONTWO_H
#define TRACEEXTENSIONTWO_H
#include <QtCore>
//#include "segdtrace.h"

class TraceExtensionTwo
{
private:
    double receiverPointEasting;
    double receiverPointNorthing;
    float receiverPointElevation;
    quint8 sensorType;
    quint32 extendedTraceNumber;

public:
    TraceExtensionTwo();
    TraceExtensionTwo(const QByteArray &arrIn);
    double getX();
    double getY();
    float getZ();
};

#endif // TRACEEXTENSIONTWO_H
