#ifndef TRACEHEADER_H
#define TRACEHEADER_H
#include <QtCore>

class TraceHeader
{
private:
    quint16 fileNumber;
    quint8 scanTypeNumber;
    quint8 channelSetNumber;
    quint16 traceNumber;
    quint16 firstTimingWord;
    quint8 traceHeaderExtensions;
    quint8 sampleSkew;
    quint8 traceEdit;
    quint16 extendedChannelSetNumber;
    quint32 extendedFileNumber;

public:
    TraceHeader();
    TraceHeader(const QByteArray &arrIn);
    quint32 getExtendedFileNum();
    quint16 getTraceNumber();
    quint8 getTraceEdit();
    quint8 getChannelSetNumber();
};

#endif // TRACEHEADER_H
