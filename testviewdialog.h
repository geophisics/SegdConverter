#ifndef TESTVIEWDIALOG_H
#define TESTVIEWDIALOG_H

#include <QDialog>
#include <SUB/general.h>
#include <Models/testmodel.h>
#include <QtCharts/QChartView>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QVXYModelMapper>
#include <QSignalMapper>
#include <QGraphicsSimpleTextItem>

QT_CHARTS_USE_NAMESPACE

namespace Ui {
class TestViewDialog;
}

class TestViewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TestViewDialog(QWidget *parent = 0);
    ~TestViewDialog();
    TestMap* getTestMap();
    XFileMap* getXMap();

private:
    Ui::TestViewDialog *ui;
    TestMap testMap;
    XFileMap xMap;

    TestModel *p_TestModel;
    TestSortFilterModel *p_badFilterTestModel;
    TestSortFilterModel *p_goodFilterTestModel;
    QtCharts::QScatterSeries *p_badScatterSeries;
    QtCharts::QScatterSeries *p_goodScatterSeries;
    QtCharts::QVXYModelMapper *p_badModelMappper;
    QtCharts::QVXYModelMapper *p_goodModelMapper;

    QVector<uint> pointsInTemplate;

private:
    void setBadScatterSeries();
    void setGoodScatterSeries();

public slots:
    void newTestReceived();
    void showAuxesByFfid(const uint &f);

    void setGoodScatterVisible(const bool &b);
    void setBadScatterVisible(const bool &b);
    void setAllScatterVisible(const bool &b);


    void setGoodScatterSize(const int &size);
    void setBadScatterSize(const int &size);

};

class TestChartView : public QChartView
{
    Q_OBJECT
public:
    TestChartView(QWidget *parent =0);
    TestChartView(QChart *chart, QWidget *parent = 0);

    void addLineText(const QString &txt);

private:
    QList<QGraphicsSimpleTextItem*> lineNumbers;
};

#endif // TESTVIEWDIALOG_H
