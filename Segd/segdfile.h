#ifndef SEGDFILE_H
#define SEGDFILE_H

#include <QObject>

#include "generalone.h"
#include "generaltwo.h"
#include "generalthree.h"
#include "channelsetheader.h"
#include "extendedheader.h"
#include "segdtrace.h"
#include "../SUB/general.h"

class SegdFile : public QObject
{
    Q_OBJECT
private:
    bool correct;
    GeneralOne *genHeader1;
    GeneralTwo *genHeader2;
    GeneralThree *genHeader3;
    ChannelSetHeader* channelHeaders[16];
    ExtendedHeader *extHeader;
    QString *externalHeader;
    QVector<SegdTrace*> tracesOfSegd;
    QList<int> getLPs();
    QVector<SegdTrace*> getLineTraces(const uint &lineNb);
    bool checkLine(QVector<SegdTrace *> tracesOnLine, const int badPodryad, const int badPercent);
public:
    explicit SegdFile(QObject *parent = 0);
    explicit SegdFile (const QString &path ,QObject *parent=0);
    explicit SegdFile (const QString &path, const bool &onlyAuxes, QObject *parent);
    explicit SegdFile (const QString &path ,const float &mpFactor,QObject *parent=0);
    ~SegdFile();
    static uint toUint(const QByteArray &arr);
    static quint64 toUint64(const QByteArray &arr);
    static uint BCDtoUint(const QByteArray &arr);
    static float toFloat(const QByteArray &arr);
    static double toDouble(const QByteArray &arr);
    static quint8 BCDtoUintFirstFourBit(const QByteArray &arr);
    static quint16 BCDtoUintWithoutFirstFourBit(const QByteArray &arr);
    static float toBinAsFloat(const QByteArray &arrIn);
    QVector<int> getVectorOfTraces();
    SegdTrace* getTrace(const int &traceNum);
    ExtendedHeader getExtendedHeader();
    GeneralThree getGeneralThree();
    GeneralTwo getGeneralTwo();
    GeneralOne getGeneralOne();
    ChannelSetHeader* getChannelSetHeader(const int &numHeader);
    QStringList getHeaderTests();

    QVector<SegdTrace*> getTraces();
    QStringList getXFile();
    QStringList getXLine(QVector<SegdTrace*> traces);
    float checkTests();
    QPair<QVariant,bool> checkTests(QTextStream *p_stream, const int &maxInRow, const int &maxInLine);
    QPair<QVariant,bool> checkTests(QTextStream *p_stream, const int &maxInRow, const int &maxInLine, TestMap *tMap);
    QPair<QVariant,bool> checkTests(QTextStream *p_stream, const TestLimits &limits, TestMap *tMap);
    void analyzingTestsHeaders(const int &badTraces, const int &badTracesPercent, const int &badLinesPercent);
    bool getFileState();
    void deleteMuteTraces();
    void deleteUnworkedTraces();
    void deleteMuteTraces(const QString &logFile);
    void deleteUnworkedTraces(const QString &logFile);
signals:
    void incorrectFile();
    void badChannelDetected(QString channel);
    void badLineDetected(QString line);
    void goodLineDetected(QString line);
    void badFfidDetected(QString txt);
    void goodFfidDetected(QString txt);
    void normalFfidDetected(QString txt);
public slots:
};

#endif // SEGDFILE_H
