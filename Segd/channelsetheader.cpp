#include "channelsetheader.h"
#include "segdfile.h"
ChannelSetHeader::ChannelSetHeader()
{

}

ChannelSetHeader::ChannelSetHeader(const QByteArray &arr)
{
    scanType=SegdFile::BCDtoUint(arr.mid(0,1));
    chanSet=SegdFile::BCDtoUint(arr.mid(1,1));
    startTime=SegdFile::toUint(arr.mid(2,2));
    endTime=SegdFile::toUint(arr.mid(4,2));
    descaleMultiplier=arr.mid(6,2).toHex();
    numOfChannels=SegdFile::BCDtoUint(arr.mid(8,2));
    channelType=SegdFile::BCDtoUintFirstFourBit(arr.mid(10,1));
    channelGainMethod=SegdFile::BCDtoUintWithoutFirstFourBit(arr.mid(11,1));
    numSubscansExponent=SegdFile::BCDtoUintFirstFourBit(arr.mid(11,1));
    aliasFilterFreq=SegdFile::BCDtoUint(arr.mid(12,2));
    aliasFilterScope=SegdFile::BCDtoUint(arr.mid(14,2));
    lowCutFilterFreq=SegdFile::BCDtoUint(arr.mid(16,2));
    luwCutFilterSlope=SegdFile::BCDtoUint(arr.mid(18,2));
    firstNotchFilter=SegdFile::toUint(arr.mid(20,2));
    secondNotchFilter=SegdFile::toUint(arr.mid(22,2));
    thirdNotchFilter=SegdFile::toUint(arr.mid(24,2));
    extendedChannelSetNumber=SegdFile::toUint(arr.mid(26,2));
    extendedHeaderFlag=0;
    traceHeaderExtensions=7;
    verticalStack=SegdFile::toUint(arr.mid(29,1));
    streamerNum=SegdFile::toUint(arr.mid(30,1));
    arrayForming=SegdFile::toUint(arr.mid(31,1));;
}

float ChannelSetHeader::getDescaleMultiplier()
{
    if (descaleMultiplier == "6daf")
    {
        return 0.000269683;
    }
    if (descaleMultiplier == "6db7")
    {
        return 0.000067419998231343924999237060546875;
    }
    return 1;
}
