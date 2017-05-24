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

class AuxesWidget;
struct AuxData{

    QVector<QVector<QPointF>* > auxData;
    QVector<bool> checkData;
    float headerUphole;
    float countedUphole;
    ~AuxData();
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
    quint32 minFreq, maxFreq, maxTimeAkf;
    int frqLvl;

    QSettings *settings;
    QSound *alert;
    QtCharts::QChartView *firstView;
    QtCharts::QChartView *secondView;
    QtCharts::QChartView *thirdView;

    QList<QtCharts::QChartView*> views;


    QtCharts::QChart *firstChart, *secondChart, *thirdChart;
    QtCharts::QLineSeries *firstSeries,*secondSeries,*thirdSeries, *countedUpholeSeries, *headerUpholeSeries;
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
    void setAlarmSound(const int &i);
};



class AuxesWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AuxesWidget(QWidget *parent = Q_NULLPTR);
    void paintAuxes(const int &ffid, const AuxData &data);
    void unselectChartViews();
    ~AuxesWidget();
private:
    //QHBoxLayout *viewLayout;
    QGridLayout *viewLayout;
    QStatusBar *statBar;
    QLabel *coordinatesLabel;
    QChart::ChartTheme badTheme,goodTheme;
public slots:
    void setGoodTheme(const int &i);
    void setBadTheme(const int &i);
private slots:
    void recieveCoordinates(const QString &coordinates);
    //QtCharts::QChart::ChartTheme badTheme, goodTheme;
    //QChart::ChartTheme

};

class AuxChartView : public QChartView
{
    Q_OBJECT
public:
    AuxChartView(QChart *chart, QWidget *parent = Q_NULLPTR);
    AuxChartView(QWidget *parent = Q_NULLPTR);
protected:
   // void mouseDoubleClickEvent(QMouseEvent *event);
   // bool viewportEvent(QEvent *event);
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
public:
      explicit AuxChart(QGraphicsItem *parent = 0, Qt::WindowFlags wFlags = 0);
      ~AuxChart();
      void SetYRangeLimits(const float &min,const float &max);
      void yZoomIn();
      void yZoomOut();
      void yScroll(qreal dy);


      void yScrollUp();
      void yScrollDown();

      QString getCoordinatesForTime(const float &t) const;
      //void resizeEvent(QGraphicsSceneResizeEvent *event);
  protected:
     // bool sceneEvent(QEvent *event);

  private:
      //bool gestureEvent(QGestureEvent *event);
      float scrollFactor;
      QPair<float,float> yRangeLimits;

  private:
};
#endif // AUXVIEWDIALOG_H
