#include "traceextensionsix.h"
#include "segdfile.h"
TraceExtensionSix::TraceExtensionSix()
{

}
TraceExtensionSix::TraceExtensionSix(const QByteArray &arrIn)
{
    bool *ok = new bool;
    unitType=arrIn.mid(0,1).toHex().toUInt(ok,16);
    unitSerialNumber=arrIn.mid(1,3).toHex().toUInt(ok,16);
    channelNumber=arrIn.mid(4,1).toHex().toUInt(ok,16);
    assemblyType=arrIn.mid(8,1).toHex().toUInt(ok,16);
    assemblySerialNum=arrIn.mid(9,3).toHex().toUInt(ok,16);
    locationInAssembly=arrIn.mid(12,1).toHex().toUInt(ok,16);
    fduUnitType=arrIn.mid(16,1).toHex().toUInt(ok,16);
    channelType=arrIn.mid(17,1).toHex().toUInt(ok,16);
    /*unitType=SegdFile::toUint(arrIn.mid(0,1));
    unitSerialNumber=SegdFile::toUint(arrIn.mid(1,3));
    channelNumber=SegdFile::toUint(arrIn.mid(4,1));
    assemblyType=SegdFile::toUint(arrIn.mid(8,1));
    assemblySerialNum=SegdFile::toUint(arrIn.mid(9,3));
    locationInAssembly=SegdFile::toUint(arrIn.mid(12,1));
    fduUnitType=SegdFile::toUint(arrIn.mid(16,1));
    channelType=SegdFile::toUint(arrIn.mid(17,1));*/
    sensorSensitivity=SegdFile::toFloat(arrIn.mid(20,4));
    delete ok;
}
