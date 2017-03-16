#include "generalthree.h"
#include "segdfile.h"
GeneralThree::GeneralThree()
{

}
GeneralThree::GeneralThree(const QByteArray &arrIn)
{
    extendedFileNumber=SegdFile::toUint(arrIn.mid(0,3));
    sourceLineNumber=SegdFile::toBinAsFloat(arrIn.mid(3,5));
    sourcePointNumber=SegdFile::toBinAsFloat(arrIn.mid(8,5));
    sourcePointIndex=SegdFile::toUint(arrIn.mid(13,1));
    phaseControl=SegdFile::toUint(arrIn.mid(14,1));
    vibeType=SegdFile::toUint(arrIn.mid(15,1));
    phaseAngle=SegdFile::toUint(arrIn.mid(16,2));
    generalHeaderBlockNum=SegdFile::toUint(arrIn.mid(18,1));
    sourceSetNum=SegdFile::toUint(arrIn.mid(19,1));
}

float GeneralThree::getLineNumber()
{
    return sourceLineNumber;
}
float GeneralThree::getPointNumber()
{
    return sourcePointNumber;
}
int GeneralThree::getExtendedFileNumber()
{
    return extendedFileNumber;
}
quint8 GeneralThree::getPointIndex()
{
    return sourcePointIndex;
}
