#ifndef TRACEEXTENSIONFIVE_H
#define TRACEEXTENSIONFIVE_H
#include <QtCore>

class TraceExtensionFive
{
private:

    float leakageLimit;
    float leakageValue;
    quint8 leakageError;

public:
    TraceExtensionFive();
    TraceExtensionFive(const QByteArray &arrIn);
    float getLeakageValue();
    quint8 getLeakageError();
};

#endif // TRACEEXTENSIONFIVE_H
