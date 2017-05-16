#include "testviewdialog.h"
#include "ui_testviewdialog.h"

QT_CHARTS_USE_NAMESPACE


TestViewDialog::TestViewDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TestViewDialog)
{
    ui->setupUi(this);
    p_TestModel = new TestModel(this);
    p_TestModel->setTestMap(&testMap);

//    p_filterTestModel = new TestSortFilterModel(this);
//    p_filterTestModel->setTestType(TestSortFilterModel::badTest);
//    p_filterTestModel->setSourceModel(p_TestModel);
    ui->tableView->setModel(p_TestModel);

    QChart *chart = new QChart;
    chart->setAnimationOptions(QChart::AllAnimations);

    s_series = new QScatterSeries;
    s_series->setMarkerSize(8.0);
    modelMappper = new QVXYModelMapper(this);

    modelMappper->setXColumn(2);
    modelMappper->setYColumn(3);

    modelMappper->setSeries(s_series);
    //modelMappper->setModel(p_TestModel);
    modelMappper->setModel(p_TestModel);

    chart->addSeries(s_series);
    chart->createDefaultAxes();

    ui->widget->setRenderHint(QPainter::Antialiasing);
    ui->widget->setChart(chart);



}

TestViewDialog::~TestViewDialog()
{
    delete ui;
}

TestMap* TestViewDialog::getTestMap()
{
    return &testMap;
}

void TestViewDialog::newTestReceived()
{
    emit p_TestModel->layoutChanged();
   // p_filterTestModel->invalidate();
    modelMappper->setFirstRow(0);
    modelMappper->setRowCount(testMap.count());

    ui->widget->chart()->axisX()->setRange(547870,551790);
    ui->widget->chart()->axisY()->setRange(5912175,5914630);
}
