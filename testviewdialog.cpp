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


 /*   p_lastPointScatterSeries = new QScatterSeries;
    p_lastPointScatterSeries->setMarkerSize(4.0);
    p_lastPointScatterSeries->setColor(Qt::blue);
    p_lastPointScatterSeries->setBorderColor(Qt::blue);*/
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

 //   chart->addSeries(p_lastPointScatterSeries);

    chart->createDefaultAxes();

    ui->pointsChartView->setRenderHint(QPainter::Antialiasing);
    ui->pointsChartView->setChart(chart);

    connect(ui->viewAllPointsRadioButton,SIGNAL(toggled(bool)),this,SLOT(setAllScatterVisible(bool)));
    connect(ui->viewBadPointsRadioButton,SIGNAL(toggled(bool)),this,SLOT(setBadScatterVisible(bool)));
    connect(ui->viewGoodPointsRadioButton,SIGNAL(toggled(bool)),this,SLOT(setGoodScatterVisible(bool)));
    connect(ui->goodPointsSizeSpinBox,SIGNAL(valueChanged(int)),this,SLOT(setGoodScatterSize(int)));
    connect(ui->badPointsSizeSpinBox,SIGNAL(valueChanged(int)),this,SLOT(setBadScatterSize(int)));
    connect(this,SIGNAL(sendLineLabel(QPointF,QString)),ui->pointsChartView,SLOT(addLineLabel(QPointF,QString)));

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

    //p_lastPointScatterSeries->clear();
    ui->pointsChartView->removeLineLabels();
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
    QPointF pointF;


    while (vecIt!=pointsInTemplate.end())
    {
        tPoint = testMap.value(*vecIt);
        currentLine = tPoint.getLine();
        qDebug()<<"Line"<<currentLine;
        pointsInLine = 0;
        badPointsInLine = 0;

        while (currentLine == tPoint.getLine() && vecIt!=pointsInTemplate.end())
        {
            pointF= QPointF(tPoint.getX(),tPoint.getY());
            if (tPoint.getTestError())
            {
                badPointsInLine++;
                badPoints.append(QPointF(pointF));
            }
            else
            {
                goodPoints.append(QPointF(pointF));
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
        qDebug()<<pointF;
        //p_lastPointScatterSeries->append(pointF);
        emit sendLineLabel(pointF,QString("ЛП %1\n(%2%)").arg(currentLine).arg(QString::number(badPointsInLine*100.0/pointsInLine,'g',2)));
    }
    QValueAxis *yAxis = qobject_cast<QValueAxis*>(ui->pointsChartView->chart()->axisY());
    yAxis->setRange(minY*0.9999, maxY*1.0001);
    yAxis=qobject_cast<QValueAxis*>(ui->pointsChartView->chart()->axisX());
    yAxis->setRange(minX*0.9999,maxX*1.0001);
    p_badScatterSeries->replace(badPoints);
    p_goodScatterSeries->replace(goodPoints);
    ui->pointsChartView->recountLabelPositions();

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
void TestChartView::addLineLabel(const QPointF coordinates,const QString &txt)
{
    lineLabels.append(new LineLabelTextItem(coordinates,txt,this->chart()));
    LineLabelRect *labelRect = new LineLabelRect(this->chart());
    labelRect->setAnchor(this->chart()->mapToPosition(coordinates));
    labelRect->setText(txt);
    labelRect->setPos(this->chart()->mapToPosition(coordinates) + QPoint(-20, -50));
    labelRect->setZValue(11);
    lineRects.append(labelRect);
}

void TestChartView::removeLineLabels()
{
    qDeleteAll(lineLabels);
    qDeleteAll(lineRects);
    lineLabels.clear();
    lineRects.clear();
}

void TestChartView::recountLabelPositions()
{
    QList<LineLabelTextItem*>::iterator it = lineLabels.begin();
 //   (*it)->setPos(this->chart()->mapToPosition((*it)->getCoordinates()));
    for ( ; it!=lineLabels.end();++it)
    {
        qDebug() << (*it)->getCoordinates();
        qDebug()<<this->chart()->mapToPosition((*it)->getCoordinates());
        (*it)->setPos(this->chart()->mapToPosition((*it)->getCoordinates()));
    }
}


LineLabelTextItem::LineLabelTextItem(QGraphicsItem *parent):QGraphicsSimpleTextItem(parent)
{
    coordinates = QPointF(0.0,0.0);
}

LineLabelTextItem::LineLabelTextItem(const QString &text, QGraphicsItem *parent):QGraphicsSimpleTextItem(text,parent)
{
    coordinates = QPointF(0.0,0.0);
}

LineLabelTextItem::LineLabelTextItem(const QPointF &point, const QString &text, QGraphicsItem *parent):QGraphicsSimpleTextItem(text,parent)
{
    coordinates = point;
}

QPointF LineLabelTextItem::getCoordinates()
{
    return coordinates;
}


LineLabelRect::LineLabelRect(QGraphicsItem *parent):
    QGraphicsItem(parent)
{

}

void LineLabelRect::setAnchor(QPointF point)
{
    m_anchor = point;
}

void LineLabelRect::setText(const QString &text)
{
   m_text=text;
   QFontMetrics metrics(m_font);
   m_textRect = metrics.boundingRect(QRect(0,0,20,20),Qt::AlignLeft,m_text);
   m_textRect.translate(0,0);
   prepareGeometryChange();
   m_rect=m_textRect.adjusted(-5,-5,5,5);
}

QRectF LineLabelRect::boundingRect() const
{
    QPointF anchor = mapFromParent(m_anchor);
    qDebug()<<"Anchor"<<anchor;
    qDebug()<<"m_anchor"<<m_anchor;
    QRectF rect;
    rect.setLeft(qMin(m_rect.left(), anchor.x()));
    rect.setRight(qMax(m_rect.right(), anchor.x()));
    rect.setTop(qMin(m_rect.top(), anchor.y()));
    rect.setBottom(qMax(m_rect.bottom(), anchor.y()));
    qDebug()<<"m_rect"<<m_rect;
    qDebug()<<"rect"<<rect;
    return rect;
}

void LineLabelRect::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    QPainterPath path;
    path.addRoundedRect(m_rect, 5, 5);

    QPointF anchor = mapFromParent(m_anchor);
    if (!m_rect.contains(anchor)) {
        QPointF point1, point2;

        // establish the position of the anchor point in relation to m_rect
        bool above = anchor.y() <= m_rect.top();
        bool aboveCenter = anchor.y() > m_rect.top() && anchor.y() <= m_rect.center().y();
        bool belowCenter = anchor.y() > m_rect.center().y() && anchor.y() <= m_rect.bottom();
        bool below = anchor.y() > m_rect.bottom();

        bool onLeft = anchor.x() <= m_rect.left();
        bool leftOfCenter = anchor.x() > m_rect.left() && anchor.x() <= m_rect.center().x();
        bool rightOfCenter = anchor.x() > m_rect.center().x() && anchor.x() <= m_rect.right();
        bool onRight = anchor.x() > m_rect.right();

        // get the nearest m_rect corner.
        qreal x = (onRight + rightOfCenter) * m_rect.width();
        qreal y = (below + belowCenter) * m_rect.height();
        bool cornerCase = (above && onLeft) || (above && onRight) || (below && onLeft) || (below && onRight);
        bool vertical = qAbs(anchor.x() - x) > qAbs(anchor.y() - y);

        qreal x1 = x + leftOfCenter * 10 - rightOfCenter * 20 + cornerCase * !vertical * (onLeft * 10 - onRight * 20);
        qreal y1 = y + aboveCenter * 10 - belowCenter * 20 + cornerCase * vertical * (above * 10 - below * 20);;
        point1.setX(x1);
        point1.setY(y1);

        qreal x2 = x + leftOfCenter * 20 - rightOfCenter * 10 + cornerCase * !vertical * (onLeft * 20 - onRight * 10);;
        qreal y2 = y + aboveCenter * 20 - belowCenter * 10 + cornerCase * vertical * (above * 20 - below * 10);;
        point2.setX(x2);
        point2.setY(y2);

        path.moveTo(point1);
        path.lineTo(mapFromParent(m_anchor));
        path.lineTo(point2);
        path = path.simplified();
    }
    painter->setBrush(QColor(255, 255, 255));
    painter->drawPath(path);
    painter->drawText(m_textRect, m_text);
}


