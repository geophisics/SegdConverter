#include "traceextensionseven.h"
#include "segdfile.h"
TraceExtensionSeven::TraceExtensionSeven()
{

}
TraceExtensionSeven::TraceExtensionSeven(const QByteArray &arrIn)
{
    bool *ok= new bool;
    controlUnitType=arrIn.mid(0,1).toHex().toUInt(ok,16);
    controlUnitSerialNumber=arrIn.mid(1,3).toHex().toUInt(ok,16);
    channelGainScale=arrIn.mid(4,1).toHex().toUInt(ok,16);
    channelFilter=arrIn.mid(5,1).toHex().toUInt(ok,16);
    overscaled=arrIn.mid(6,1).toHex().toUInt(ok,16);
    channelEditedStatus=arrIn.mid(7,1).toHex().toUInt(ok,16);
    /* controlUnitType=SegdFile::toUint(arrIn.mid(0,1));
    controlUnitSerialNumber=SegdFile::toUint(arrIn.mid(1,3));
    channelGainScale=SegdFile::toUint(arrIn.mid(4,1));
    channelFilter=SegdFile::toUint(arrIn.mid(5,1));
    overscaled=SegdFile::toUint(arrIn.mid(6,1));
    channelEditedStatus=SegdFile::toUint(arrIn.mid(7,1));*/
    sampleToMvFactor=SegdFile::toFloat(arrIn.mid(8,4));
    //qDebug()<<QString::number(sampleToMvFactor,'f',40);
    numberOfStacksNoisy=arrIn.mid(12,1).toHex().toUInt(ok,16);
    numberOfStacksLow=arrIn.mid(13,1).toHex().toUInt(ok,16);
    channelType=arrIn.mid(14,1).toHex().toUInt(ok,16);
    channelProcess=arrIn.mid(15,1).toHex().toUInt(ok,16);
    /*numberOfStacksNoisy=SegdFile::toUint(arrIn.mid(12,1));
    numberOfStacksLow=SegdFile::toUint(arrIn.mid(13,1));
    channelType=SegdFile::toUint(arrIn.mid(14,1));
    channelProcess=SegdFile::toUint(arrIn.mid(15,1));*/
    traceMaxValue=SegdFile::toFloat(arrIn.mid(16,4));
    traceMaxTime=arrIn.mid(20,4).toHex().toUInt(ok,16);
    numberOfInterpolations=arrIn.mid(24,4).toHex().toUInt(ok,16);
    dcOffset=arrIn.mid(28,4).toHex().toUInt(ok,16);
    /*traceMaxTime=SegdFile::toUint(arrIn.mid(20,4));
    numberOfInterpolations=SegdFile::toUint(arrIn.mid(24,4));
    dcOffset=SegdFile::toUint(arrIn.mid(28,4));*/
    delete ok;
}

quint8 TraceExtensionSeven::getChannelType()
{
    return channelType;
}
float TraceExtensionSeven::getSampleToMvFactor()
{
    return sampleToMvFactor;
}
