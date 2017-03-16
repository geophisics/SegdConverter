#ifndef GENERALTHREE_H
#define GENERALTHREE_H
#include <QtCore>

class GeneralThree
{
private:
   int extendedFileNumber;
   float sourceLineNumber;
   float sourcePointNumber;
   quint8 sourcePointIndex;
   quint8 phaseControl;
   quint8 vibeType;
   quint8 phaseAngle;
   quint8 generalHeaderBlockNum;
   quint8 sourceSetNum;

public:
    GeneralThree();
    GeneralThree(const QByteArray &arrIn);

    float getLineNumber();
    float getPointNumber();
    int getExtendedFileNumber();
    quint8 getPointIndex();
};

#endif // GENERALTHREE_H
