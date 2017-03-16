#ifndef SEGDTRACE_H
#define SEGDTRACE_H
#include <QObject>
#include "traceheader.h"
#include "traceextensionone.h"
#include "traceextensiontwo.h"
#include "traceextensionthree.h"
#include "traceextensionfour.h"
#include "traceextensionfive.h"
#include "traceextensionsix.h"
#include "traceextensionseven.h"

class SegdTrace : public QObject
{
    Q_OBJECT
private:
    TraceHeader *generalTraceHeader;
    TraceExtensionOne *extHeader1;
    TraceExtensionTwo *extHeader2;
    TraceExtensionThree *extHeader3;
    TraceExtensionFour *extHeader4;
    TraceExtensionFive *extHeader5;
    TraceExtensionSix *extHeader6;
    TraceExtensionSeven *extHeader7;
    QVector<float> *traceData;
public:
    explicit SegdTrace(QObject *parent = 0);
    explicit SegdTrace(const QByteArray &arrIn, QObject *parent=0);
    explicit SegdTrace(const QByteArray &arrIn, const float &mpFactor, QObject *parent=0);
    TraceHeader* getTraceHeader();
    TraceExtensionOne* getExtensionOne();
    TraceExtensionThree* getExtensionThree();
    TraceExtensionFive* getExtensionFive();
    TraceExtensionTwo* getExtensionTwo();
    TraceExtensionSeven* getExtensionSeven();
    QVector<float> getTraceData();
    float getMaxA();
    QString getTests();
    quint8 getChannelType();
    ~SegdTrace();
};

#endif // SEGDTRACE_H
