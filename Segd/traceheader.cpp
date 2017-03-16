#include "traceheader.h"
#include "segdfile.h"
TraceHeader::TraceHeader()
{

}

TraceHeader::TraceHeader(const QByteArray &arrIn)
{
    bool *ok = new bool;
    fileNumber=SegdFile::BCDtoUint(arrIn.mid(0,2));
    scanTypeNumber=SegdFile::BCDtoUint(arrIn.mid(2,1));
    channelSetNumber=SegdFile::BCDtoUint(arrIn.mid(3,1));
    traceNumber=SegdFile::BCDtoUint(arrIn.mid(4,2));
    firstTimingWord=arrIn.mid(6,3).toHex().toUInt(ok,16);
    traceHeaderExtensions=arrIn.mid(9,1).toHex().toUInt(ok,16);
    sampleSkew=arrIn.mid(10,1).toHex().toUInt(ok,16);
    traceEdit=arrIn.mid(11,1).toHex().toUInt(ok,16);
    extendedChannelSetNumber=arrIn.mid(15,2).toHex().toUInt(ok,16);
    extendedFileNumber=arrIn.mid(17,3).toHex().toUInt(ok,16);
    delete ok;

}
quint32 TraceHeader::getExtendedFileNum()
{
    return extendedFileNumber;
}
quint16 TraceHeader::getTraceNumber()
{
    return traceNumber;
}

quint8 TraceHeader::getTraceEdit()
{
    return traceEdit;
}
quint8 TraceHeader::getChannelSetNumber()
{
    return channelSetNumber;
}
