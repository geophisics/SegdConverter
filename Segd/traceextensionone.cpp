#include "traceextensionone.h"
#include "segdfile.h"
TraceExtensionOne::TraceExtensionOne()
{

}
TraceExtensionOne::TraceExtensionOne(const QByteArray &arrIn)
{
    bool *ok= new bool;
    receiverLineNum = arrIn.mid(0,3).toHex().toUInt(ok,16);
    receiverPointNum = arrIn.mid(3,3).toHex().toUInt(ok,16);
    receiverPointIndex=arrIn.mid(6,1).toHex().toUInt(ok,16);
    samplesPerTrace=arrIn.mid(7,3).toHex().toUInt(ok,16);
    extendedReceiverLineNumber=arrIn.mid(10,5).toHex().toUInt(ok,16);
    extendedReceiverPointNumber=arrIn.mid(15,5).toHex().toUInt(ok,16);
    sensorType=arrIn.mid(20,1).toHex().toUInt(ok,16);
    delete ok;
    //----------------------------------------------
    /*receiverLineNum=SegdFile::toUint(arrIn.mid(0,3));
    receiverPointNum=SegdFile::toUint(arrIn.mid(3,3));
    receiverPointIndex=SegdFile::toUint(arrIn.mid(6,1));
    samplesPerTrace=SegdFile::toUint(arrIn.mid(7,3));
    extendedReceiverLineNumber=SegdFile::toUint(arrIn.mid(10,5));
    extendedReceiverPointNumber=SegdFile::toUint(arrIn.mid(15,5));
    sensorType=SegdFile::toUint(arrIn.mid(20,1));*/
    //-----------------------------------------------
}
int TraceExtensionOne::getLinePointNum()
{
    return receiverLineNum*10000+receiverPointNum;
}
quint32 TraceExtensionOne::getReceiverLineNum()
{
    return receiverLineNum;
}
quint32 TraceExtensionOne::getReceiverPointNum()
{
    return receiverPointNum;
}
quint32 TraceExtensionOne::getSamplesPerTrace()
{
    return samplesPerTrace;
}
