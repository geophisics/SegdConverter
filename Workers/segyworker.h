#ifndef SEGYWORKER_H
#define SEGYWORKER_H

#include <Workers/baseworker.h>

class SegyFile;
class SegyWorker : public BaseWorker
{
    Q_OBJECT
public:
    explicit SegyWorker(volatile bool *running,CountedAttributes *attr) : BaseWorker(running,attr) {}

protected:
    bool writeFileHeaders; //требуется или нет писать заголовки segy файла
    void countAttributesFromFile(SegyFile *sgy);
    bool convertOneFile(const QString &filePath);

public slots:
    virtual void Converting();

private slots:
    void segdDirChanged(QString string);

};

#endif // SEGYWORKER_H
