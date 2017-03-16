#ifndef CSTTRACE_H
#define CSTTRACE_H

#include <QObject>
#include <Segd/segdtrace.h>
class CstTrace : public QObject
{
    Q_OBJECT
private:

    QVector<int> headers;
    QVector<float> trace;
    float maxAmpl;
    double xRotating(const float azX, const float azY);
    double yRotating(const float azX, const float azY);

    double xRotating(const double x, const double y, const float azX, const float azY);
    double yRotating(const double x, const double y, const float azX, const float azY);

public:

    explicit CstTrace(QObject *parent = 0);
    explicit CstTrace(SegdTrace *segdTr, QObject *parent=0);

    int getReceiverLinePoint();
    int getTraceNb();
    int getTraceStatus();

    void setReceiverX(const uint &val);
    void setReceiverY(const uint &val);
    void setSourceX(const uint &val);
    void setSourceY(const uint &val);
    void setSourceLinePoint(const uint &lp);
    void setReceiverLinePoint(const uint &lp);
    void countGeometry();
    void writeTrace(QFile &fileOut);
    int getReceiverX();
    int getReceiverY();
    int getFfid();
    int getOffset();
    int getTestsStatus();
    float getMaxAmpl();
    QVector<float> getData();

signals:

public slots:
};

#endif // CSTTRACE_H
