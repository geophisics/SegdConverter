#ifndef CHANNELSETHEADER_H
#define CHANNELSETHEADER_H
#include <QtCore>

class ChannelSetHeader
{
private:
    quint8 scanType;
    quint8 chanSet;
    quint16 startTime;
    quint16 endTime;
    QString descaleMultiplier;
    quint16 numOfChannels;
    quint8 channelType;
    quint8 channelGainMethod;
    quint8 numSubscansExponent;
    quint16 aliasFilterFreq;
    quint16 aliasFilterScope;
    quint16 lowCutFilterFreq;
    quint16 luwCutFilterSlope;
    quint16 firstNotchFilter;
    quint16 secondNotchFilter;
    quint16 thirdNotchFilter;
    quint16 extendedChannelSetNumber;
    quint8 extendedHeaderFlag;
    quint8 traceHeaderExtensions;
    quint8 verticalStack;
    quint8 streamerNum;
    quint8 arrayForming;




public:
    ChannelSetHeader();
    ChannelSetHeader(const QByteArray &arr);
    float getDescaleMultiplier();
};

#endif // CHANNELSETHEADER_H
