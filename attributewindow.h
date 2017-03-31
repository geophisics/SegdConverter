#ifndef ATTRIBUTEWINDOW_H
#define ATTRIBUTEWINDOW_H

struct AttributeWindow
{

public:
    explicit AttributeWindow();
    AttributeWindow(const AttributeWindow &other);

/*    void setMinOffset(int offset);
    void setMaxOffset(int offset);
    void setMinTime(int time);
    void setMaxTime(int time);
    void setMinAmpl(float ampl);
    void setMinRms(float ampl);
    void setMinFreq(float freq);
    void setMinDfr(float dfr);
    void setMinEnergy(float energy);
    void setCountAmpl(bool count);
    void setCountRms(bool count);
    void setCountFreq(bool count);
    void setCountEnergy(bool count);
    void setCountDfr(bool count);
    void setWriteSpectrum(bool write);

    int getMinOffset();
    int getMaxOffset();
    int getMinTime();
    int getMaxTime();
    float getMinAmpl();
    float getMinRms();
    float getMinFreq();
    float getMinDfr();
    float getMinEnergy();
    bool getCountAmpl();
    bool getCountRms();
    bool getCountFreq();
    bool getCountEnergy();
    bool getCountDfr();
    bool getWriteSpectrum();*/
    bool isCorrect();

    int minOffset;
    int maxOffset;
    int minTime;
    int maxTime;
    float minAmpl;
    float minRms;
    float minFreq;
    float minDfr;
    float minEnergy;
    bool countAmpl;
    bool countRms;
    bool countFreq;
    bool countEnergy;
    bool countDfr;
    bool writeSpectrum;
};


/*struct SeisAttributes
{
    float ampl;
    bool correctAmpl;
    float rms;
    bool correctRms;
    float energy;
    bool correctEnergy;
    float freq;
    bool correctFreq;
    float dfr;
    bool correctDfr;
public:
    SeisAttributes();
    SeisAttributes(float a, float r, float en, float fr, float d);
    SeisAttributes(const SeisAttributes &other);
    ~SeisAttributes();
};

struct FfidData
{
    int ffid;
    int line;
    int source;
    float X;
    float Y;
    float Z;
public:
    FfidData();
    FfidData (int f, int l, int p, float x, float y, float z);
    FfidData(const FfidData &other);
    ~FfidData();
};*/
#endif // ATTRIBUTEWINDOW_H
