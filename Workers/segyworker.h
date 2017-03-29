#ifndef SEGYWORKER_H
#define SEGYWORKER_H

#include <QObject>
#include <QSettings>
#include <QtXlsx>
#include <attributewindow.h>
#include <SUB/point.h>
#include <SUB/exclusion.h>
#include <Workers/baseworker.h>

#include "aquila/global.h"

class SegyFile;
class SegyWorker : public BaseWorker
{
    Q_OBJECT
public:
    explicit SegyWorker(QObject *parent = 0);


    //void setFirstFile();
signals:


protected:

    bool writeFileHeaders; //требуется или нет писать заголовки segy файла

    void countAttributes(SegyFile *sgy);
    bool convertOneFile(const QString &filePath, const bool &writeHeaders);




    //std::vector<double>  getAKF(QVector<float> vec);


    //double getFreq(QVector<std::vector<double> > faks);
    //std::vector<double> getSpectrum (QVector<std::vector<double> > akfs);
    //std::vector<double> getSpectrum (std::vector<float> vec);

public slots:

    virtual void Converting();
};

#endif // SEGYWORKER_H
