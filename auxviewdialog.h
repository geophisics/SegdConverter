#ifndef AUXVIEWDIALOG_H
#define AUXVIEWDIALOG_H

#include <QDialog>

#include <QtCharts/QChartView>
#include <QtCharts/QSplineSeries>
#include "QtCharts/QValueAxis"
#include <QtCharts/QLineSeries>
#include <Segd/segdtrace.h>
#include <QSound>

namespace Ui {
class AuxViewDialog;
}

class AuxViewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AuxViewDialog(QWidget *parent = 0);
    ~AuxViewDialog();


private:
    Ui::AuxViewDialog *ui;
    quint32 minFreq, maxFreq, maxTimeAkf;
    int frqLvl;

    QSettings *settings;
    QSound *alert;
    QtCharts::QChartView *firstView;
    QtCharts::QChartView *secondView;
    QtCharts::QChartView *thirdView;
    QtCharts::QChart *firstChart, *secondChart, *thirdChart;
    QtCharts::QLineSeries *firstSeries,*secondSeries,*thirdSeries;



    void readSettings();
    void saveSettings();

public slots:
    void receiveVectors(QVector<QPointF> *trace, const bool &traceStatus, QVector<QPointF> *spectrum, const bool &spectrumStatus, const int &ffid);
    void receiveExplAuxes(QVector<QPointF> *timeBreakTrace, const bool &timeBreakStatus, QVector<QPointF> *confirmedTimeBreakTrace, const bool &confirmedTimeBreakStatus,QVector<QPointF> *upholeTimeTrace, const bool &uphleTimeStatus);
};

#endif // AUXVIEWDIALOG_H
