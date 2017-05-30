#ifndef AUXVIEWDIALOG_H
#define AUXVIEWDIALOG_H

#include <QDialog>

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QSplineSeries>
#include "QtCharts/QValueAxis"
#include <QtCharts/QLineSeries>
#include <Segd/segdtrace.h>
#include <QSound>
#include <QHBoxLayout>
#include <QGestureEvent>
#include <QStatusBar>
#include <QLabel>

QT_CHARTS_USE_NAMESPACE

//typedef QPair<float,float> AxisRange;


class AuxesWidget;
struct AuxData{

    QVector<QVector<QPointF>* > auxData;
    QVector<bool> checkData;
    float headerUphole;
    float countedUphole;
    ~AuxData();
};

struct AxisRange
{
    AxisRange(const float &min, const float &max):minValue(min),
        maxValue(max) {}
    AxisRange(){minValue=0.0;maxValue=1000.0;}
    float minValue;
    float maxValue;
};


namespace Ui {
class AuxViewDialog;
}

class AuxViewDialog : public QWidget
{
    Q_OBJECT

public:
    explicit AuxViewDialog(QWidget *parent = 0);
    ~AuxViewDialog();
    bool showAuxesByFfid(const int &fileNum);
    void clearData();
private:
    Ui::AuxViewDialog *ui;
    AuxesWidget *auxWidget;
    QStatusBar *statBar;
    QSettings *settings;
    QSound *alarmSound;

    QMap<int,AuxData> auxes;
    QMap<int,AuxData>::iterator auxIterator;

    void readSettings();
    void saveSettings();

public slots:
    void receiveVibAuxes(QVector<QPointF> *trace, const bool &traceStatus, QVector<QPointF> *spectrum, const bool &spectrumStatus, const int &ffid);
    void receiveExplAuxes(const int &ffid, QVector<QPointF> *timeBreakTrace, const bool &timeBreakStatus, QVector<QPointF> *confirmedTimeBreakTrace, const bool &confirmedTimeBreakStatus, QVector<QPointF> *upholeTimeTrace,const bool &upholeTimeStatus, const float &countUphole, const float &headUphole);
private slots:
    void previousButtonClicked();
    void nextButtonClicked();
    void setAlarmSound(const int &i, const bool &b = true);


};



class AuxesWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AuxesWidget(QWidget *parent = Q_NULLPTR);
    void paintAuxes(const int &ffid, const AuxData &data);
    QMap<QString,AxisRange > getRanges();
    ~AuxesWidget();
    void setFrqLvl(const int &i);
    void setTvOffset(const int &i);
    void setTbOffset(const int &i);
    int getFrqLvl();
private:
    //QHBoxLayout *viewLayout;
    QGridLayout *viewLayout;
    QStatusBar *statBar;
    QLabel *coordinatesLabel;
    QChart::ChartTheme badTheme,goodTheme;
    int frqLvl;
    int tvOffset;
    int tbOffset;
    QMap<QString,AxisRange > ranges;
    void themeChanged();





public slots:
    void setGoodTheme(const int &i);
    void setBadTheme(const int &i);
    void setRange(const float &min, const float &max, const QString &chartName);
    void setRanges(QMap<QString,AxisRange > r);
private slots:
    void recieveCoordinates(const QString &coordinates);
};

class AuxChartView : public QChartView
{
    Q_OBJECT
public:
    AuxChartView(QChart *chart, QWidget *parent = Q_NULLPTR);
    AuxChartView(QWidget *parent = Q_NULLPTR);
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
    void resizeEvent(QResizeEvent *event);
private:
    bool mousePressed;
    float yOld;
signals:
    void sendCoordinates(QString);
};

class AuxChart : public QChart
{
      Q_OBJECT
public:
      explicit AuxChart(QGraphicsItem *parent = 0, Qt::WindowFlags wFlags = 0);
      ~AuxChart();
      void SetYRangeLimits(const float &min,const float &max);
      void setYRange(const AxisRange &range);
      void yZoomIn();
      void yZoomOut();
      void yScroll(qreal dy);
      void yScrollUp();
      void yScrollDown();
      QString getCoordinatesForTime(const float &t) const;
      QString getLevelForFreq(const float &f) const ;
  private:
      float scrollFactor;
      QPair<float,float> yRangeLimits;
signals:
      void sendNewRange(float,float,QString);
};
#endif // AUXVIEWDIALOG_H
