#include "testviewdialog.h"
#include "ui_testviewdialog.h"
#include "QValueAxis"

QT_CHARTS_USE_NAMESPACE


TestViewDialog::TestViewDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TestViewDialog)
{
    ui->setupUi(this);
    p_TestModel = new TestModel(this);
    p_TestModel->setTestMap(&testMap);

    p_badFilterTestModel = new TestSortFilterModel(this);
    p_badFilterTestModel->setTestType(TestSortFilterModel::badTest);
    p_badFilterTestModel->setSourceModel(p_TestModel);
    ui->badPointsTableView->setModel(p_badFilterTestModel);

    p_goodFilterTestModel = new TestSortFilterModel(this);
    p_goodFilterTestModel->setTestType(TestSortFilterModel::okTest);
    p_goodFilterTestModel->setSourceModel(p_TestModel);
    ui->goodPointsTableView->setModel(p_goodFilterTestModel);




    QChart *chart = new QChart;
    chart->setAnimationOptions(QChart::NoAnimation);

    p_badScatterSeries = new QScatterSeries;
    p_badScatterSeries->setMarkerSize(8.0);
    p_badScatterSeries->setColor(Qt::red);
    p_badModelMappper = new QVXYModelMapper(this);
    p_badModelMappper->setXColumn(2);
    p_badModelMappper->setYColumn(3);
    p_badModelMappper->setSeries(p_badScatterSeries);
    p_badModelMappper->setModel(p_badFilterTestModel);


    p_goodScatterSeries = new QScatterSeries;
    p_goodScatterSeries->setMarkerSize(4.0);
    p_goodScatterSeries->setColor(Qt::green);
    p_goodModelMapper = new QVXYModelMapper(this);
    p_goodModelMapper->setXColumn(2);
    p_goodModelMapper->setYColumn(3);
    p_goodModelMapper->setSeries(p_goodScatterSeries);
    p_goodModelMapper->setModel(p_goodFilterTestModel);

    chart->addSeries(p_badScatterSeries);
    chart->addSeries(p_goodScatterSeries);

    chart->createDefaultAxes();
    QValueAxis *yAxis = qobject_cast<QValueAxis*>(chart->axisY());
    yAxis->setRange(6719000.0,6726400.0);
    yAxis=qobject_cast<QValueAxis*>(chart->axisX());
    yAxis->setRange(612565.0,616500.0);

    ui->pointsChartView->setRenderHint(QPainter::Antialiasing);
    ui->pointsChartView->setChart(chart);



}

TestViewDialog::~TestViewDialog()
{
    delete ui;
}

TestMap* TestViewDialog::getTestMap()
{
    return &testMap;
}

XFileMap* TestViewDialog::getXMap()
{
    return &xMap;
}



void TestViewDialog::newTestReceived()
{
    emit p_TestModel->layoutChanged();
   // p_filterTestModel->invalidate();
 //   modelMappper->setFirstRow(0);
 //   modelMappper->setRowCount(testMap.count());

    //ui->widget->chart()->axisX()->setRange(547870,551790);
    //ui->widget->chart()->axisY()->setRange(5912175,5914630);
}

void TestViewDialog::showAuxesByFfid(const uint &f)
{
    p_badFilterTestModel->setXFile(xMap.value(f));
    p_goodFilterTestModel->setXFile(xMap.value(f));

    p_badModelMappper->setFirstRow(0);
    p_badModelMappper->setRowCount(p_badFilterTestModel->rowCount());

    p_goodModelMapper->setFirstRow(0);
    p_goodModelMapper->setRowCount(p_goodFilterTestModel->rowCount());
}
