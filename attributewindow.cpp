#include "attributewindow.h"
#include <QVector>
#include <math.h>
#include <QFile>
#include <QDebug>
AttributeWindow::AttributeWindow()
{
    minOffset = 0;
    maxOffset=0;
    minTime=0;
    maxTime=0;
    minAmpl=0;
    minRms=0;
    minFreq=0;
    minDfr=0;
    minEnergy=0;
    countAmpl=false;
    countRms=false;
    countFreq=false;
    countEnergy=false;
    countDfr=false;
    writeSpectrum=false;
}

AttributeWindow::AttributeWindow(const AttributeWindow &other)
{
    minOffset = other.minOffset;
    maxOffset=other.maxOffset;
    minTime=other.minTime;
    maxTime=other.maxTime;
    minAmpl=other.minAmpl;
    minRms=other.minRms;
    minFreq=other.minFreq;
    minDfr=other.minDfr;
    minEnergy=other.minEnergy;
    countAmpl=other.countAmpl;
    countRms=other.countRms;
    countFreq=other.countFreq;
    countEnergy=other.countEnergy;
    countDfr=other.countDfr;
    writeSpectrum=other.writeSpectrum;
}

/*void AttributeWindow::setMinOffset(int offset)
{
    minOffset = offset;
}

int AttributeWindow::getMinOffset()
{
    return minOffset;
}

void AttributeWindow::setMaxOffset(int offset)
{
    maxOffset = offset;
}
int AttributeWindow::getMaxOffset()
{
    return maxOffset;
}
void AttributeWindow::setMinTime(int time)
{
    minTime = time;
}
int AttributeWindow::getMinTime()
{
    return minTime;
}
void AttributeWindow::setMaxTime(int time)
{
    maxTime = time;
}
int AttributeWindow::getMaxTime()
{
    return maxTime;
}
void AttributeWindow::setCountAmpl(bool count)
{
    countAmpl = count;
}
bool AttributeWindow::getCountAmpl()
{
    return countAmpl;
}
void AttributeWindow::setCountFreq(bool count)
{
    countFreq=count;
}
bool AttributeWindow::getCountFreq()
{
    return countFreq;
}
void AttributeWindow::setCountRms(bool count)
{
    countRms = count;
}
bool AttributeWindow::getCountRms()
{
    return countRms;
}
void AttributeWindow::setCountDfr(bool count)
{
    countDfr= count;
}
bool AttributeWindow::getCountDfr()
{
    return countDfr;
}
void AttributeWindow::setCountEnergy(bool count)
{
    countEnergy=count;
}
bool AttributeWindow::getCountEnergy()
{
    return countEnergy;
}
void AttributeWindow::setWriteSpectrum(bool write)
{
    writeSpectrum=write;
}
bool AttributeWindow::getWriteSpectrum()
{
    return writeSpectrum;
}
void AttributeWindow::setMinAmpl(float ampl)
{
    minAmpl=ampl;
}
float AttributeWindow::getMinAmpl()
{
    return minAmpl;
}
void AttributeWindow::setMinRms(float ampl)
{
    minRms=ampl;
}
float AttributeWindow::getMinRms()
{
    return minRms;
}
void AttributeWindow::setMinFreq(float freq)
{
    minFreq=freq;
}
float AttributeWindow::getMinFreq()
{
    return minFreq;
}
void AttributeWindow::setMinDfr(float dfr)
{
    minDfr=dfr;
}
float AttributeWindow::getMinDfr()
{
    return minDfr;
}
void AttributeWindow::setMinEnergy(float energy)
{
    minEnergy=energy;
}
float AttributeWindow::getMinEnergy()
{
    return minEnergy;
}*/
bool AttributeWindow::isCorrect()
{
    if (maxOffset>minOffset && maxTime>minTime)
    {
        return true;
    }
    else
    {
        return false;
    }
}


/*FfidData::FfidData()
{
    ffid = 0;
    X=0.0;
    Y=0.0;
    Z=0.0;
    line =0;
    source =0;
}
FfidData::FfidData(int f, int l, int p, float x, float y, float z)
{
    ffid = f;
    line = l;
    source = p;
    X=x;
    Y=y;
    Z=z;
}

FfidData::FfidData(const FfidData &other)
{
    ffid = other.ffid;
    line = other.line;
    source = other.source;
    X= other.X;
    Y= other.Y;
    Z= other.Z;
}
FfidData::~FfidData()
{

}
SeisAttributes::SeisAttributes()
{
    ampl=0.0;
    correctAmpl = true;
    rms = 0.0;
    correctRms = true;
    freq =0.0;
    correctFreq = true;
    energy = 0.0;
    correctEnergy =true;
    dfr = 0.0;
    correctDfr = true;

}
SeisAttributes::SeisAttributes(float a, float r, float en, float fr, float d)
{
    ampl = a;
    correctAmpl = true;
    rms = r;
    correctAmpl = true;
    energy = en;
    correctEnergy = true;
    freq = fr;
    correctFreq = true;
    dfr = d;
    correctDfr = true;
}
SeisAttributes::SeisAttributes(const SeisAttributes &other)
{
    ampl = other.ampl;
    correctAmpl = other.correctAmpl;
    rms = other.rms;
    correctAmpl = other.correctRms;
    energy = other.energy;
    correctEnergy = other.correctEnergy;
    freq = other.freq;
    correctFreq = other.correctFreq;
    dfr = other.dfr;
    correctDfr = other.correctDfr;
}
SeisAttributes::~SeisAttributes()
{

}
*/
