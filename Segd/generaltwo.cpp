#include "generaltwo.h"
#include "segdfile.h"
GeneralTwo::GeneralTwo()
{


}
GeneralTwo::GeneralTwo(const QByteArray &arrIn)
{
    extendedFileNumber=SegdFile::toUint(arrIn.mid(0,3));
    extendedChanSets=SegdFile::toUint(arrIn.mid(3,2));
    extendedHeaderBlocks=SegdFile::toUint(arrIn.mid(5,2));
    externalHeaderBlocks=SegdFile::toUint(arrIn.mid(7,2));
    segdRevision=SegdFile::toBinAsFloat(arrIn.mid(10,2));
    generalTrailerBlocks=SegdFile::toUint(arrIn.mid(12,2));
    extendedRecordLength=SegdFile::toUint(arrIn.mid(14,3));
    generalHeaderBlockNum=SegdFile::toUint(arrIn.mid(18,1));
}

int GeneralTwo::getExtendedRecordLength()
{
    return extendedRecordLength;
}
