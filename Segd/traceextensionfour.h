#ifndef TRACEEXTENSIONFOUR_H
#define TRACEEXTENSIONFOUR_H
#include <QtCore>

class TraceExtensionFour
{
private:
    float capacitanceLowLimit;
    float capacitanceHighLimit;
    float capacitanceValue;
    float cutOffLowLimit;
    float cutOffHighLimit;
    float cutOffValue;
    quint8 capacitanceError;
    quint8 cutOffError;

public:
    TraceExtensionFour();
    TraceExtensionFour(const QByteArray &arrIn);
};

#endif // TRACEEXTENSIONFOUR_H
