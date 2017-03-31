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
    explicit SegyWorker(CountedAttributes *attr) : BaseWorker(attr) {}
signals:
protected:
    bool writeFileHeaders; //требуется или нет писать заголовки segy файла
    void countAttributesFromFile(SegyFile *sgy);
    bool convertOneFile(const QString &filePath, const bool &writeHeaders);
public slots:
    virtual void Converting();
};

#endif // SEGYWORKER_H
