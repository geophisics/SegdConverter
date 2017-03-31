#ifndef CSTWORKERONLINE_H
#define CSTWORKERONLINE_H
#include "Workers/cstworker.h"
#include <QObject>
class CstWorkerOnline : public CstWorker
{
    Q_OBJECT
public:
     explicit CstWorkerOnline(CountedAttributes *attr) : CstWorker(attr) {
        watcher  = new QFileSystemWatcher(this);
        connect(watcher,SIGNAL(directoryChanged(QString)),this,SLOT(segdDirChanged(QString)),Qt::DirectConnection);
    }
private slots:
    void segdDirChanged(QString string);

private:
    bool convertOneFileOnline(const QString &filePath);
    QFileSystemWatcher *watcher;

    int fileForConvertingNum;
    int fileCount;

public slots:
    void Converting();
};

#endif // CSTWORKERONLINE_H
