#ifndef ATTRIBUTEWINDOW_H
#define ATTRIBUTEWINDOW_H

#include <QString>


struct AttributeWindow
{
    explicit AttributeWindow();
    AttributeWindow(const AttributeWindow &other);
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

struct Relation
{
    explicit Relation();
    explicit Relation(const QString &div, const QString &dev, const float &value) : dividend(div),devider(dev),minValue(value) {}
    Relation(const Relation &other);
    QString dividend;
    QString devider;
    float minValue;
};


#endif // ATTRIBUTEWINDOW_H
