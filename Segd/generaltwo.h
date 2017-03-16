#ifndef GENERALTWO_H
#define GENERALTWO_H
#include <QtCore>

class GeneralTwo
{
private:
    int extendedFileNumber;
    quint8 extendedChanSets;
    quint8 extendedHeaderBlocks;
    quint8 externalHeaderBlocks;
    float segdRevision;
    quint8 generalTrailerBlocks;
    int extendedRecordLength;
    quint8 generalHeaderBlockNum;
public:
    GeneralTwo();
    GeneralTwo(const QByteArray &arrIn);
    int getExtendedRecordLength();
};

#endif // GENERALTWO_H
