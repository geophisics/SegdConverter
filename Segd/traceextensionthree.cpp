#include "traceextensionthree.h"
#include "segdfile.h"
TraceExtensionThree::TraceExtensionThree()
{

}
TraceExtensionThree::TraceExtensionThree(const QByteArray &arrIn)
{
    bool *ok = new bool;
    resistanceLowLimit=SegdFile::toFloat(arrIn.mid(0,4));
    resistanceHighLimit=SegdFile::toFloat(arrIn.mid(4,4));
    resistanceValue=SegdFile::toFloat(arrIn.mid(8,4));
    tiltLimit=SegdFile::toFloat(arrIn.mid(12,4));
    tiltValue=SegdFile::toFloat(arrIn.mid(16,4));
    resistanceError=arrIn.mid(20,1).toHex().toUInt(ok,16);
    tiltError=arrIn.mid(21,1).toHex().toUInt(ok,16);
    delete ok;
    /*resistanceError=SegdFile::toUint(arrIn.mid(20,1));
    tiltError=SegdFile::toUint(arrIn.mid(21,1));*/
}
float TraceExtensionThree::getResistanceValue()
{
    return resistanceValue;
}
float TraceExtensionThree::getTiltValue()
{
    return tiltValue;
}
quint8 TraceExtensionThree::getTiltError()
{
    return tiltError;
}
quint8 TraceExtensionThree::getResistanceError()
{
    return resistanceError;
}
