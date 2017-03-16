#ifndef CSTFILE_H
#define CSTFILE_H

#include <QObject>
#include <Cst/csttrace.h>
#include <Segd/segdfile.h>
#include <point.h>
#include "exclusion.h"
class CstFile : public QObject
{
    Q_OBJECT
public:
    explicit CstFile(QObject *parent = 0);
    explicit CstFile(SegdFile *segd, QObject *parent =0);
    int getFfid();
    int getLine();
    int getPoint();
    int getSampleRate();
    int getLength();
    double getEasting();
    double getNorthing();
    float getElevation();

    float getXorigin();
    float getYorigin();
    float getdX();
    float getdY();
    float getAzX();
    float getAzY();

    int getI0();
    int getJ0();

    void setReceiverCoordinats(QMap<QString,Point*> coordinates);
    void setSourceCoordinats(QMap<QString, Point*> coordinates);
    void setGeometry();
    void writeAuxTraces(const QString &filePath);
    void writeTraces(const QString &filePath, const bool &muted);
    void writeTraces(const QString &filePath, const bool &muted, const bool &missed);
    QStringList deleteTracesInExclusions(QVector<Exclusion*> excl);
    QVector<QVector<float> > getDataInWindow(const int &minOffset, const int &maxOffset, const int &minTime, const int &maxTime, const bool muted);

    QVector<QVector<float> > getDataInWindow(QTextStream *logStr, const uint &minOffset, const uint &maxOffset, const uint &minTime, const uint &maxTime, const bool &muted, const bool &tests, const float &minA);
    QVector<QVector<float> > getDataInWindow(QTextStream *logStr, const uint &minOffset, const uint &maxOffset, const uint &minTime, const uint &maxTime, const bool &muted, const bool &tests, const float &minA,QVector<int> exclReceivers);
    QVector<QVector<float> > getDataInWindow(QTextStream *logStr, const uint &minOffset, const uint &maxOffset, const uint &minTime, const uint &maxTime, const bool &muted, const bool &tests, const float &minA, QVector<Exclusion*> exclVec);

    bool setTemplates(XFile *xps);
    bool traceInExclusion(CstTrace* tr,QVector<Exclusion*> excls);
private:
    QSettings *settings;
    //параметры бинирования
    float xOrigin, yOrigin, dX, dY, azX, azY;
    int i0, j0;


private:
    QVector<CstTrace*> traces;
    int line;
    int point;
    int ffid;
    //int sampleInterval;
    int sampleRate;
    int length;
    uint numOfAuxes;
    uint numOfSeis;
    double easting;
    double northing;
    float elevation;


    void readSettings();

signals:

public slots:
};

#endif // CSTFILE_H
