#ifndef AUXVIEWDIALOG_H
#define AUXVIEWDIALOG_H

#include <QDialog>

#include <QtCharts/QChartView>
#include <QtCharts/QSplineSeries>
#include "QtCharts/QValueAxis"
#include <QtCharts/QLineSeries>
#include <Segd/segdtrace.h>
#include <QSound>

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

class AuxViewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AuxViewDialog(QWidget *parent = 0);
    ~AuxViewDialog();
    bool showAuxesByFfid(const int &fileNum);
    void clearData();


private:
    Ui::AuxViewDialog *ui;
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
    void receiveVectors(QVector<QPointF> *trace, const bool &traceStatus, QVector<QPointF> *spectrum, const bool &spectrumStatus, const int &ffid);

    //void receiveExplAuxes(QVector<QPointF> *timeBreakTrace, const bool &timeBreakStatus, QVector<QPointF> *confirmedTimeBreakTrace, const bool &confirmedTimeBreakStatus,QVector<QPointF> *upholeTimeTrace, const bool &uphleTimeStatus);
    void receiveExplAuxes(const int &ffid, QVector<QPointF> *timeBreakTrace, const bool &timeBreakStatus, QVector<QPointF> *confirmedTimeBreakTrace, const bool &confirmedTimeBreakStatus, QVector<QPointF> *upholeTimeTrace, const bool &uphleTimeStatus);
    void receiveExplAuxes(const int &ffid, QVector<QPointF> *timeBreakTrace, const bool &timeBreakStatus, QVector<QPointF> *confirmedTimeBreakTrace, const bool &confirmedTimeBreakStatus, QVector<QPointF> *upholeTimeTrace,const bool &upholeTimeStatus, const float &countUphole, const float &headUphole);
    void showAuxes();
private slots:
    void previousButtonClicked();
    void nextButtonClicked();
};

#endif // AUXVIEWDIALOG_H
