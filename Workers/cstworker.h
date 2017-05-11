#ifndef CSTWORKER_H
#define CSTWORKER_H

#include <Workers/baseworker.h>
class CstFile;
class CstWorker : public BaseWorker
{
    Q_OBJECT
public:
    explicit CstWorker(volatile bool *running, CountedAttributes *attr) : BaseWorker(running,attr) {}
    explicit CstWorker(volatile bool *running, CountedAttributes *attr,TestMap *tMap) : BaseWorker(running,attr,tMap) {}


protected:
    void countAttributes(CstFile *cst);
    bool convertOneFile(const QString &filePath);
public slots:

    virtual void Converting();

private slots:
    void segdDirChanged(QString string);

};

#endif // CSTWORKER_H
