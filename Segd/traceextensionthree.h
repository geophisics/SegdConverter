#ifndef TRACEEXTENSIONTHREE_H
#define TRACEEXTENSIONTHREE_H
#include <QtCore>

class TraceExtensionThree
{
private:
  float resistanceLowLimit;
  float resistanceHighLimit;
  float resistanceValue;
  float tiltLimit;
  float tiltValue;
  quint8 resistanceError;
  quint8 tiltError;

public:
    TraceExtensionThree();
    TraceExtensionThree(const QByteArray &arrIn);
    float getResistanceValue();
    float getTiltValue();
    quint8 getResistanceError();
    quint8 getTiltError();

};

#endif // TRACEEXTENSIONTHREE_H
