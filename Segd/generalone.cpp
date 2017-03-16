#include "generalone.h"
#include "segdfile.h"
GeneralOne::GeneralOne()
{

}
GeneralOne::GeneralOne(const QByteArray &arrIn)
{
    //int tmp;
    fileNumber = SegdFile::BCDtoUint(arrIn.mid(0,2));
    formatCode = SegdFile::BCDtoUint(arrIn.mid(2,2));
    generalConstant = arrIn.mid(4,6);
    year = SegdFile::BCDtoUint(arrIn.mid(10,1));
    //tmp = SegdFile::toUint(arrIn.mid(11,2));
    //numOfAddBlocks=SegdFile::BCDtoUintFirstFourBit(arrIn.mid(11,1));
    day=SegdFile::BCDtoUintWithoutFirstFourBit(arrIn.mid(11,2));
    numOfAddBlocks =SegdFile::BCDtoUintFirstFourBit(arrIn.mid(11,1));
    hour=SegdFile::BCDtoUint(arrIn.mid(13,1));
    minute=SegdFile::BCDtoUint(arrIn.mid(14,1));
    second=SegdFile::BCDtoUint(arrIn.mid(15,1));
    manufacturersCode=SegdFile::BCDtoUint(arrIn.mid(16,1));
    manufacturersSerial=SegdFile::BCDtoUint(arrIn.mid(17,2));
    bytesPerSnimok=SegdFile::BCDtoUint(arrIn.mid(19,3));
    baseScanInterval=SegdFile::BCDtoUint(arrIn.mid(22,1));
    //tmp = SegdFile::toUint(arrIn.mid(23,2));
    polarity=SegdFile::BCDtoUintFirstFourBit(arrIn.mid(23,1));
    recordType=SegdFile::BCDtoUintFirstFourBit(arrIn.mid(25,1));
    recordLength=SegdFile::BCDtoUintWithoutFirstFourBit(arrIn.mid(25,2));
    scanType=SegdFile::BCDtoUint(arrIn.mid(27,1));
    chanSets=SegdFile::BCDtoUint(arrIn.mid(28,1));
    skewBlocks=SegdFile::BCDtoUint(arrIn.mid(29,1));
    extendedHeaderBlocks=SegdFile::BCDtoUint(arrIn.mid(30,1));
    externalHeaderBlocks=SegdFile::BCDtoUint(arrIn.mid(31,1));

}
quint16 GeneralOne::getDay()
{
    return day;
}
quint16 GeneralOne::getYear()
{
    return year;
}
quint8 GeneralOne::getHour()
{
    return hour;
}
quint8 GeneralOne::getMinute()
{
    return minute;
}
quint8 GeneralOne::getSecond()
{
    return second;
}
