#include "traceextensiontwo.h"
#include "segdfile.h"
TraceExtensionTwo::TraceExtensionTwo()
{

}
TraceExtensionTwo::TraceExtensionTwo(const QByteArray &arrIn)
{
    bool *ok = new bool;
    receiverPointEasting=SegdFile::toDouble(arrIn.mid(0,8));
    receiverPointNorthing=SegdFile::toDouble(arrIn.mid(8,8));
    receiverPointElevation=SegdFile::toFloat(arrIn.mid(16,4));
    sensorType=arrIn.mid(20,1).toHex().toUInt(ok,16);
    extendedTraceNumber=arrIn.mid(28,4).toHex().toUInt(ok,16);
    delete ok;
    /*sensorType=SegdFile::toUint(arrIn.mid(20,1));
    extendedTraceNumber=SegdFile::toUint(arrIn.mid(28,4));*/
}
double TraceExtensionTwo::getX()
{
    return receiverPointEasting;
}
double TraceExtensionTwo::getY()
{
    return receiverPointNorthing;
}
float TraceExtensionTwo::getZ()
{
    return receiverPointElevation;
}
