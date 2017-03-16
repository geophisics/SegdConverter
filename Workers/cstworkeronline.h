#ifndef CSTWORKERONLINE_H
#define CSTWORKERONLINE_H
#include "Workers/cstworker.h"
#include <QObject>
class CstWorkerOnline : public CstWorker
{
    Q_OBJECT
public:
    explicit CstWorkerOnline(QObject *parent = 0);

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
