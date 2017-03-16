#include "traceextensionfive.h"
#include "segdfile.h"
TraceExtensionFive::TraceExtensionFive()
{

}
TraceExtensionFive::TraceExtensionFive(const QByteArray &arrIn)
{
    bool *ok= new bool;
    leakageLimit=SegdFile::toFloat(arrIn.mid(0,4));
    leakageValue=SegdFile::toFloat(arrIn.mid(4,4));
    leakageError=arrIn.mid(24,1).toHex().toUInt(ok,16);
    delete ok;
    //leakageError=SegdFile::toUint(arrIn.mid(24,1));
}

float TraceExtensionFive::getLeakageValue()
{
    return leakageValue;
}
quint8 TraceExtensionFive::getLeakageError()
{
    return leakageError;
}
