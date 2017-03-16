#ifndef SEGYWORKERONLINE_H
#define SEGYWORKERONLINE_H

#include <QObject>
#include "Workers/segyworker.h"
class SegyWorkerOnline : public SegyWorker
{
    Q_OBJECT
public:
    explicit SegyWorkerOnline(QObject *parent = 0);

signals:

private slots:
    void segdDirChanged(QString string);
private:

    bool convertOneFileOnline(const QString &filePath, const bool &writeHeaders);
    QFileSystemWatcher *watcher;

    //int fileForConvertingNum;
    int fileForConvertingNum; // порядковый номер файла в папке для конвертации
    int fileCount; // счетчик файлов в сводном файле


public slots:

    void Converting();
};

#endif // SEGYWORKERONLINE_H
