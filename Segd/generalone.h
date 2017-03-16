#ifndef GENERALONE_H
#define GENERALONE_H
#include <QByteArray>

class GeneralOne
{
private:
    quint16 fileNumber;
    quint16 formatCode;
    QByteArray generalConstant;
    quint16 year;
    quint8 numOfAddBlocks;
    quint16 day;
    quint8 hour;
    quint8 minute;
    quint8 second;
    quint8 manufacturersCode;
    quint16 manufacturersSerial;
    quint8 bytesPerSnimok;
    quint8 baseScanInterval;
    quint8 polarity;
    quint8 recordType;
    quint8 recordLength;
    quint8 scanType;
    quint8 chanSets;
    quint8 skewBlocks;
    quint8 extendedHeaderBlocks;
    quint16 externalHeaderBlocks;


public:
    GeneralOne();
    GeneralOne(const QByteArray &arrIn);
    quint16 getYear();
    quint16 getDay();
    quint8 getHour();
    quint8 getMinute();
    quint8 getSecond();
};

#endif // GENERALONE_H
