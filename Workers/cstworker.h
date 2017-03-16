#ifndef CSTWORKER_H
#define CSTWORKER_H

#include <Workers/baseworker.h>
class CstFile;
class CstWorker : public BaseWorker
{
    Q_OBJECT
public:

    explicit CstWorker(QObject *parent = 0);

signals:

protected:
    void countAttributes(CstFile *cst, const int currentRow);
    bool convertOneFile(const QString &filePath);



public slots:

    virtual void Converting();
};

#endif // CSTWORKER_H
