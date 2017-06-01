#include "testviewdialog.h"
#include "ui_testviewdialog.h"
#include "QValueAxis"
#include <QDebug>
QT_CHARTS_USE_NAMESPACE


TestViewDialog::TestViewDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TestViewDialog)
{


    ui->setupUi(this);
    this->setWindowFlags(Qt::Window);
    p_TestModel = new TestModel(this);
    p_TestModel->setTestMap(&testMap);

    p_badFilterTestModel = new TestSortFilterModel(this);
    p_badFilterTestModel->setTestType(TestSortFilterModel::badTest);
    p_badFilterTestModel->setSourceModel(p_TestModel);
    //ui->badPointsTableView->setModel(p_badFilterTestModel);

    p_goodFilterTestModel = new TestSortFilterModel(this);
    p_goodFilterTestModel->setTestType(TestSortFilterModel::okTest);
    p_goodFilterTestModel->setSourceModel(p_TestModel);
    //ui->goodPointsTableView->setModel(p_goodFilterTestModel);




    QChart *chart = new QChart;
    chart->setAnimationOptions(QChart::NoAnimation);
    ui->pointsChartView->setRubberBand(QChartView::RectangleRubberBand);

    p_goodScatterSeries = new QScatterSeries;
    p_goodScatterSeries->setMarkerSize(6.0);
    p_goodScatterSeries->setColor(Qt::green);
    p_goodScatterSeries->setBorderColor(Qt::green);
    p_goodScatterSeries->setName("Пикеты в допусках");
    p_badScatterSeries = new QScatterSeries;
    p_badScatterSeries->setMarkerSize(6.0);
    p_badScatterSeries->setColor(Qt::red);
    p_badScatterSeries->setBorderColor(Qt::red);
    p_badScatterSeries->setName("Пикеты вне допусков");
    //p_badModelMappper = new QVXYModelMapper(this);
    //p_badModelMappper->setXColumn(2);
    //p_badModelMappper->setYColumn(3);
    //p_badModelMappper->setSeries(p_badScatterSeries);
    //p_badModelMappper->setModel(p_badFilterTestModel);



    //p_goodModelMapper = new QVXYModelMapper(this);
    //p_goodModelMapper->setXColumn(2);
    //p_goodModelMapper->setYColumn(3);
    //p_goodModelMapper->setSeries(p_goodScatterSeries);
    //p_goodModelMapper->setModel(p_goodFilterTestModel);
    chart->addSeries(p_goodScatterSeries);
    chart->addSeries(p_badScatterSeries);


    chart->createDefaultAxes();

    ui->pointsChartView->setRenderHint(QPainter::Antialiasing);
    ui->pointsChartView->setChart(chart);

    connect(ui->viewAllPointsRadioButton,SIGNAL(toggled(bool)),this,SLOT(setAllScatterVisible(bool)));
    connect(ui->viewBadPointsRadioButton,SIGNAL(toggled(bool)),this,SLOT(setBadScatterVisible(bool)));
    connect(ui->viewGoodPointsRadioButton,SIGNAL(toggled(bool)),this,SLOT(setGoodScatterVisible(bool)));
    connect(ui->goodPointsSizeSpinBox,SIGNAL(valueChanged(int)),this,SLOT(setGoodScatterSize(int)));
    connect(ui->badPointsSizeSpinBox,SIGNAL(valueChanged(int)),this,SLOT(setBadScatterSize(int)));

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

//устанавливаем видимость для "хороших" пикетов
void TestViewDialog::setGoodScatterVisible(const bool &b)
{
    if (b)
    {
        p_goodScatterSeries->setVisible(true);
        p_badScatterSeries->setVisible(false);
    }
}

void TestViewDialog::setBadScatterVisible(const bool &b)
{

    if (b)
    {
        p_goodScatterSeries->setVisible(false);
        p_badScatterSeries->setVisible(true);
    }
}
void TestViewDialog::setAllScatterVisible(const bool &b)
{
    if (b)
    {
        p_goodScatterSeries->setVisible(true);
        p_badScatterSeries->setVisible(true);
    }
}


void TestViewDialog::setBadScatterSize(const int &size)
{
    p_badScatterSeries->setMarkerSize(size);
}

void TestViewDialog::setGoodScatterSize(const int &size)
{
    p_goodScatterSeries->setMarkerSize(size);
}

void TestViewDialog::showAuxesByFfid(const uint &f)
{



    XFile xfile = xMap.value(f);

    pointsInTemplate.clear();
    uint counter;
    foreach (Template t, xfile.getTemplates()) {
        counter =0;
        for (uint i = t.firstChannel; i<=t.lastChannel;++i)
        {
            pointsInTemplate.push_back(t.receiverLine*10000+t.firstReceiver+counter);
            counter++;
        }
    }
    //qDebug()<<pointsInTemplate.count();
    setBadScatterSeries();
 //   qDebug()<<ui->pointsChartView->chart()->contentsRect();
 //   setGoodScatterSeries();

 //   p_badFilterTestModel->setXFile(xMap.value(f));
 //   p_goodFilterTestModel->setXFile(xMap.value(f));

    /*p_badModelMappper->setFirstRow(0);
    p_badModelMappper->setRowCount(p_badFilterTestModel->rowCount());

    p_goodModelMapper->setFirstRow(0);
    p_goodModelMapper->setRowCount(p_goodFilterTestModel->rowCount());*/

 // qDebug()<<p_goodFilterTestModel->rows123;
  //qDebug()<<p_badFilterTestModel->rows123;
}

void TestViewDialog::setBadScatterSeries()
{
    QVector<QPointF> badPoints;
    QVector<QPointF> goodPoints;
    float minX = std::numeric_limits<float>::max();
    float maxX = 0.0;
    float minY = std::numeric_limits<float>::max();
    float maxY = 0.0;
    TestPoint tPoint;
    uint currentLine;
    uint pointsInLine;
    uint badPointsInLine;

    QVector<uint>::iterator vecIt = pointsInTemplate.begin();

    while (vecIt!=pointsInTemplate.end())
    {
        tPoint = testMap.value(*vecIt);
        currentLine = tPoint.getLine();
        qDebug()<<"Line"<<currentLine;
        pointsInLine = 0;
        badPointsInLine = 0;
        while (currentLine == tPoint.getLine() && vecIt!=pointsInTemplate.end())
        {
            if (tPoint.getTestError())
            {
                badPointsInLine++;
                badPoints.append(QPointF(tPoint.getX(),tPoint.getY()));
            }
            else
            {
                goodPoints.append(QPointF(tPoint.getX(),tPoint.getY()));
            }
            vecIt++;
            pointsInLine++;
            minX = tPoint.getX() > minX ? minX : tPoint.getX();
            maxX = tPoint.getX() > maxX ? tPoint.getX() : maxX;
            minY = tPoint.getY() > minY ? minY : tPoint.getY();
            maxY = tPoint.getY() > maxY ? tPoint.getY() : maxY;
            tPoint = testMap.value(*vecIt);
        }
        qDebug()<<"PointsInLine"<<pointsInLine;
        qDebug()<<"BadPointsInLine"<<badPointsInLine;
    }

/*
    for (int i=0; i<pointsInTemplate.count();i++)
    {
        tPoint = testMap.value(pointsInTemplate.at(i));
        //currentLine = tPoint.getLine();

        if (tPoint.getTestError())
        {
            badPoints.append(QPointF(tPoint.getX(),tPoint.getY()));
        }
        else
        {
            goodPoints.append(QPointF(tPoint.getX(),tPoint.getY()));
        }

    }
*/
    QValueAxis *yAxis = qobject_cast<QValueAxis*>(ui->pointsChartView->chart()->axisY());
    yAxis->setRange(minY*0.9999, maxY*1.0001);
    yAxis=qobject_cast<QValueAxis*>(ui->pointsChartView->chart()->axisX());
    yAxis->setRange(minX*0.9999,maxX*1.0001);
    p_badScatterSeries->replace(badPoints);
    p_goodScatterSeries->replace(goodPoints);

}

void TestViewDialog::setGoodScatterSeries()
{
    QVector<QPointF> goodPoints;
    TestPoint tPoint;
    for (int i=0; i<pointsInTemplate.count();i++)
    {
        tPoint = testMap.value(pointsInTemplate.at(i));
        if (!tPoint.getTestError())
        {
            goodPoints.append(QPointF(tPoint.getX(),tPoint.getY()));
        }
    }
    qDebug()<<"GoodPOints"<<goodPoints.count();
    p_goodScatterSeries->replace(goodPoints);
}



TestChartView::TestChartView(QWidget *parent):QChartView(parent)
{

}

TestChartView::TestChartView(QChart *chart, QWidget *parent):QChartView(chart,parent)
{

}
