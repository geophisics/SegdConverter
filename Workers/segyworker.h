#ifndef SEGYWORKER_H
#define SEGYWORKER_H

#include <QObject>
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
    explicit SegyWorker(volatile bool *running,CountedAttributes *attr) : BaseWorker(running,attr) {}
signals:

protected:
    bool writeFileHeaders; //требуется или нет писать заголовки segy файла
    void countAttributesFromFile(SegyFile *sgy);
    bool convertOneFile(const QString &filePath, const bool &writeHeaders);

public slots:
    virtual void Converting();

private slots:
    void segdDirChanged(QString string);

};

#endif // SEGYWORKER_H
