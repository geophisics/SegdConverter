#include "traceextensionfour.h"
#include "segdfile.h"
TraceExtensionFour::TraceExtensionFour()
{

}
TraceExtensionFour::TraceExtensionFour(const QByteArray &arrIn)
{
    bool *ok= new bool;
    capacitanceLowLimit=SegdFile::toFloat(arrIn.mid(0,4));
    capacitanceHighLimit=SegdFile::toFloat(arrIn.mid(4,4));
    capacitanceValue=SegdFile::toFloat(arrIn.mid(8,4));
    cutOffLowLimit=SegdFile::toFloat(arrIn.mid(12,4));
    cutOffHighLimit=SegdFile::toFloat(arrIn.mid(16,4));
    capacitanceError=arrIn.mid(24,1).toHex().toUInt(ok,16);
    cutOffError=arrIn.mid(25,4).toHex().toUInt(ok,16);
    delete ok;
    /*capacitanceError=SegdFile::toUint(arrIn.mid(24,1));
    cutOffError=SegdFile::toUint(arrIn.mid(25,4));*/
}
