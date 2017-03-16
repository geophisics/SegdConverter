#ifndef TRACEEXTENSIONSIX_H
#define TRACEEXTENSIONSIX_H
#include <QtCore>

class TraceExtensionSix
{
private:
    quint8 unitType;
    quint32 unitSerialNumber;
    quint8 channelNumber;
    quint8 assemblyType;
    quint32 assemblySerialNum;
    quint8 locationInAssembly;
    quint8 fduUnitType;
    quint8 channelType;
    float sensorSensitivity;

public:
    TraceExtensionSix();
    TraceExtensionSix(const QByteArray &arrIn);
};

#endif // TRACEEXTENSIONSIX_H
