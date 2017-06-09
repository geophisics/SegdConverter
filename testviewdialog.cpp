#include "testviewdialog.h"
#include "ui_testviewdialog.h"
#include "QValueAxis"
#include <QDebug>
#include <math.h>
#include <QScreen>
QT_CHARTS_USE_NAMESPACE


TestViewDialog::TestViewDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TestViewDialog)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Window);
    QChart *chart = new QChart;
    chart->setAnimationOptions(QChart::NoAnimation);
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->legend()->setZValue(50);
    QMargins margins = chart->margins();
    margins.setTop(40);
    margins.setRight(60);
    chart->setMargins(margins);
    //ui->pointsChartView->setRubberBand(QChartView::RectangleRubberBand);
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
    chart->addSeries(p_goodScatterSeries);
    chart->addSeries(p_badScatterSeries);
    chart->createDefaultAxes();
    ui->pointsChartView->setRenderHint(QPainter::Antialiasing);
    ui->pointsChartView->setChart(chart);
    ui->pointsChartView->addPointLabel();
    connect(ui->viewBadPointsCheckBox,SIGNAL(toggled(bool)),this,SLOT(setBadScatterVisible(bool)));
    connect(ui->viewGoodPointsCheckBox,SIGNAL(toggled(bool)),this,SLOT(setGoodScatterVisible(bool)));
    connect(ui->goodPointsSizeSpinBox,SIGNAL(valueChanged(int)),this,SLOT(setGoodScatterSize(int)));
    connect(ui->badPointsSizeSpinBox,SIGNAL(valueChanged(int)),this,SLOT(setBadScatterSize(int)));
    QValueAxis *axis = qobject_cast<QValueAxis*>(chart->axisX());
    axis = qobject_cast<QValueAxis*>(chart->axisY());
    connect(this,SIGNAL(sendLineLabel(QPointF,QString,bool)),ui->pointsChartView,SLOT(addLineLabel(QPointF,QString,bool)));
    connect(p_badScatterSeries,SIGNAL(hovered(QPointF,bool)),this,SLOT(mouseUnderPoint(QPointF,bool)));
    connect(p_goodScatterSeries,SIGNAL(hovered(QPointF,bool)),this,SLOT(mouseUnderPoint(QPointF,bool)));
    connect(ui->pointsChartView,SIGNAL(mousePressedWithCtrl(QPointF)),this,SLOT(mousePressedOnChart(QPointF)));
    connect(ui->deletePointLabelsPushButton,SIGNAL(clicked(bool)),ui->pointsChartView,SLOT(deletePointLabels()));
}

void TestViewDialog::setSettings(QSettings *set)
{
    settings = set;
    readSettings();
}

void TestViewDialog::readSettings()
{
    settings->beginGroup("TestViewSettings");
    this->restoreGeometry(settings->value("Geometry").toByteArray());
    ui->viewBadPointsCheckBox->setChecked(settings->value("ViewBadPoints",true).toBool());
    ui->viewGoodPointsCheckBox->setChecked(settings->value("ViewGoodPoints",true).toBool());
    ui->badPointsSizeSpinBox->setValue(settings->value("BadPointsSize",5).toUInt());
    ui->goodPointsSizeSpinBox->setValue(settings->value("GoodPointsSize",5).toUInt());
    p_badScatterSeries->setVisible(ui->viewBadPointsCheckBox->isChecked());
    p_goodScatterSeries->setVisible(ui->viewGoodPointsCheckBox->isChecked());
    settings->endGroup();
}

void TestViewDialog::saveSettings()
{
    settings->beginGroup("TestViewSettings");
    settings->setValue("ViewBadPoints",ui->viewBadPointsCheckBox->isChecked());
    settings->setValue("ViewGoodPoints",ui->viewGoodPointsCheckBox->isChecked());
    settings->setValue("BadPointsSize",ui->badPointsSizeSpinBox->value());
    settings->setValue("GoodPointsSize",ui->goodPointsSizeSpinBox->value());
    settings->setValue("Geometry",this->saveGeometry());
    settings->endGroup();
}


TestViewDialog::~TestViewDialog()
{
    saveSettings();
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




//устанавливаем видимость для "хороших" пикетов
void TestViewDialog::setGoodScatterVisible(const bool &b)
{
        p_goodScatterSeries->setVisible(b);
}

void TestViewDialog::setBadScatterVisible(const bool &b)
{
    p_badScatterSeries->setVisible(b);
}

void TestViewDialog::setBadScatterSize(const int &size)
{
    p_badScatterSeries->setMarkerSize(size);
}

void TestViewDialog::setGoodScatterSize(const int &size)
{
    p_goodScatterSeries->setMarkerSize(size);
}

void TestViewDialog::showTemplatesByFfid(const uint &f)
{
    XFile xfile = xMap.value(f);
    QMap<uint, QPair<uint,bool> > linesMap;
    pointsInTemplate.clear();
    uint counter;
    foreach (Template t, xfile.getTemplates()) {
        counter =0;
        for (uint i = t.firstChannel; i<=t.lastChannel;++i)
        {
            pointsInTemplate.append(testMap.value(qMakePair(t.receiverLine,t.firstReceiver+counter)));
            counter++;
        }
        linesMap.insert(t.receiverLine,qMakePair(t.badChannels,t.lineStatus));
    }
    setBadScatterSeries(linesMap);
}

void TestViewDialog::mouseUnderPoint(const QPointF &coordinates, const bool &b)
{
    if (b)
    {
    QVector<TestPoint>::iterator vecIt = pointsInTemplate.begin();
    float minDistance = sqrt(powf(coordinates.x()-vecIt->getX(),2.0)+powf(coordinates.y()-vecIt->getY(),2.0));
    float distance;
    TestPoint tPoint = *vecIt;
    while (vecIt!=pointsInTemplate.end())
    {
        vecIt++;
        distance = sqrt(powf(coordinates.x()-vecIt->getX(),2.0)+powf(coordinates.y()-vecIt->getY(),2.0));
        if (distance<minDistance)
        {
            minDistance = distance;
            tPoint = *vecIt;
        }
    }
    ui->pointsChartView->setLastPointLabel(tPoint);
    }
    else
    {
        ui->pointsChartView->hideLastPointLabel();
    }
}

void TestViewDialog::mousePressedOnChart(const QPointF &coordinates)
{
    float minDistance;
    float distance;
    QPointF nearestPoint;
    if (p_badScatterSeries->isVisible())
    {
        minDistance = sqrt(powf(coordinates.x()-p_badScatterSeries->at(0).x(),2.0)+powf(coordinates.y()-p_badScatterSeries->at(0).y(),2.0));
    }
    else if (p_goodScatterSeries->isVisible())
    {
        minDistance = sqrt(powf(coordinates.x()-p_goodScatterSeries->at(0).x(),2.0)+powf(coordinates.y()-p_goodScatterSeries->at(0).y(),2.0));
    }
    else
    {
        return;
    }
    if (p_badScatterSeries->isVisible())
    {
        for (int i=0; i<p_badScatterSeries->count();++i)
        {
            distance =  sqrt(powf(coordinates.x()-p_badScatterSeries->at(i).x(),2.0)+powf(coordinates.y()-p_badScatterSeries->at(i).y(),2.0));
            if (distance<minDistance)
            {
                minDistance = distance;
                nearestPoint = p_badScatterSeries->at(i);
            }
        }
    }
    if (p_goodScatterSeries->isVisible())
    {
        for (int i=0; i<p_goodScatterSeries->count();++i)
        {
            distance =  sqrt(powf(coordinates.x()-p_goodScatterSeries->at(i).x(),2.0)+powf(coordinates.y()-p_goodScatterSeries->at(i).y(),2.0));
            if (distance<minDistance)
            {
                minDistance = distance;
                nearestPoint = p_goodScatterSeries->at(i);
            }
        }
    }
    mouseUnderPoint(nearestPoint,true);
}


void TestViewDialog::setBadScatterSeries(QMap<uint, QPair<uint,bool> > lines)
{

    ui->pointsChartView->removeLineLabels();
    QVector<QPointF> badPoints;
    QVector<QPointF> goodPoints;
    uint currentLine;
    uint pointsInLine;
    QVector<TestPoint>::iterator vecIt = pointsInTemplate.begin();
    float minX = vecIt->getX();
    float maxX = 0.0;
    float minY = vecIt->getY();
    float maxY = 0.0;
    QPointF lineLabelPoint;
    QPointF firstPoint;
    float angle=0.0;
    uint numOfLines=0;

    while (vecIt!=pointsInTemplate.end())
    {
        currentLine = vecIt->getLine();
        pointsInLine = 0;
        firstPoint = vecIt->getPointF();
        while (currentLine == vecIt->getLine() && vecIt!=pointsInTemplate.end())
        {
            if (vecIt->getTestError())
            {
                badPoints.append(QPointF(vecIt->getX(),vecIt->getY()));
            }
            else
            {
                goodPoints.append(QPointF(vecIt->getX(),vecIt->getY()));
            }
            lineLabelPoint = QPointF(vecIt->getX(),vecIt->getY());
            minX = vecIt->getX() > minX ? minX : vecIt->getX();
            maxX = vecIt->getX() > maxX ? vecIt->getX() : maxX;
            minY = vecIt->getY() > minY ? minY : vecIt->getY();
            maxY = vecIt->getY() > maxY ? vecIt->getY() : maxY;
            vecIt++;
            pointsInLine++;
        }

        emit sendLineLabel(lineLabelPoint,QString("ЛП %1\n%2(%3%)").arg(currentLine).arg(lines.value(currentLine).first)
                           .arg(QString::number(lines.value(currentLine).first*100.0/pointsInLine,'g',2)),lines.value(currentLine).second);
        angle+=fabs(atan((lineLabelPoint.y()-firstPoint.y())/(lineLabelPoint.x()-firstPoint.x())));

        numOfLines++;
    }
    ui->pointsChartView->setLineAngle(angle/numOfLines);
    p_badScatterSeries->replace(badPoints);
    p_goodScatterSeries->replace(goodPoints);
    ui->pointsChartView->setAxisLimits(minX*0.9999,maxX*1.0001,minY*0.9999,maxY*1.0001);
    ui->pointsChartView->setAxisRanges();


}




TestChartView::TestChartView(QWidget *parent):QChartView(parent)
{
    xEqualY = true;
    zoomRect = Q_NULLPTR;
    chartKoef = 1;

}

TestChartView::TestChartView(QChart *chart, QWidget *parent):QChartView(chart,parent)
{
    xEqualY = true;
    zoomRect = Q_NULLPTR;
    chartKoef = 1;
}



void TestChartView::setAxisRanges()
{
    QValueAxis *axis;
    if (xEqualY)
    {
        QRectF plotAreaRect = this->chart()->plotArea();
        chartKoef = plotAreaRect.width()/plotAreaRect.height();
        if (chartKoef>=1.0)
        {
            axis = qobject_cast<QValueAxis*>(this->chart()->axisY());
            axis->setRange(minY,maxY);
            qreal xLength = (maxX - minX)*chartKoef/2.0;
            qreal xcenter = (maxX+minX)/2.0;
            axis = qobject_cast<QValueAxis*>(this->chart()->axisX());
            axis->setRange(xcenter-xLength,xcenter+xLength);
        }
        else
        {
            axis = qobject_cast<QValueAxis*>(this->chart()->axisX());
            axis->setRange(minX,maxX);
            qreal yLength = (maxY - minY)/chartKoef/2.0;
            qreal ycenter = (maxY+minY)/2.0;
            axis = qobject_cast<QValueAxis*>(this->chart()->axisY());
            axis->setRange(ycenter-yLength,ycenter+yLength);
        }
    }
    repositionLabels();
}

//устанавливаем пределы осей
void TestChartView::setAxisLimits(const qreal &xmin, const qreal &xmax, const qreal &ymin, const qreal &ymax)
{
    minX=xmin;
    maxX=xmax;
    minY=ymin;
    maxY=ymax;
}

void TestChartView::addPointLabel()
{
    pointRects.append(new PointLabelRect(this->chart()));
}

void TestChartView::deletePointLabels()
{
    QList<PointLabelRect*>::iterator pointIt = pointRects.begin();
    while (*pointIt!=pointRects.last()) {
        delete (*pointIt);
        pointIt = pointRects.erase(pointIt);
    }
    pointRects.last()->hide();
}
void TestChartView::hideLastPointLabel()
{
    pointRects.last()->hide();
}
void TestChartView::setLastPointLabel(const TestPoint &point)
{
    pointRects.last()->setTestPoint(point);
    pointRects.last()->setZValue(99);
    QPointF pos = this->chart()->mapToPosition(point.getPointF()) + QPointF(20,-30);
    if (pos.x()+pointRects.last()->getRect().width() > this->width())
    {
        pos.setX(pos.x()-40.0-pointRects.last()->getRect().width());
    }
    pointRects.last()->setAnchor(this->chart()->mapToPosition(point.getPointF()));
    pointRects.last()->setPos(pos);
    pointRects.last()->show();
}


void TestChartView::setLineAngle(const float &a)
{
    lineAngle = a;
}

void TestChartView::addLineLabel(const QPointF coordinates, const QString &txt, const bool &status)
{
    LineLabelRect *labelRect = new LineLabelRect(this->chart());
    //labelRect->hide();
    labelRect->setAnchor(coordinates);
    labelRect->setText(txt);
    labelRect->setLineStatus(status);
//    labelRect->setPos(this->chart()->mapToPosition(coordinates) + QPoint(-20, -50));
    labelRect->setZValue(0);
    lineRects.append(labelRect);
}

void TestChartView::removeLineLabels()
{
    qDeleteAll(lineRects);
    lineRects.clear();
}

void TestChartView::resizeEvent(QResizeEvent *event)
{
    QChartView::resizeEvent(event);
    setAxisRanges();
    this->update();
}

void TestChartView::mousePressEvent(QMouseEvent *event)
{
    if (event->modifiers()==Qt::ControlModifier)
    {
        emit mousePressedWithCtrl(chart()->mapToValue(event->pos()));
        QChartView::mousePressEvent(event);
        return;
    }
    if (event->button()==Qt::LeftButton)
    {
        zoomPoint = event->pos();
        zoomRect = new QGraphicsRectItem(event->x(),event->y(), 30 , 50,this->chart());
        zoomRect->show();
        QChartView::mousePressEvent(event);
        return;
    }
}

void TestChartView::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (pointRects.last()->isVisible()) {
        addPointLabel();
    }
    QChartView::mouseDoubleClickEvent(event);
}

void TestChartView::mouseMoveEvent(QMouseEvent *event)
{
    if (zoomRect)
    {

        //QRectF rect;
        //rect.setTopLeft(QPointF(std::min(zoomPoint.x(),event->x()),std::min(zoomPoint.y(),event->y())));

        /*QRectF rect = QRectF(QPointF(std::min(zoomPoint.x(),event->x()),std::min(zoomPoint.y(),event->y())),
                             QPointF(std::max(zoomPoint.x(),event->x()),std::max(zoomPoint.y(),event->y())));*/
        qreal w,h;
        w = event->x()-zoomPoint.x();

        h = w/chartKoef;
        if (event->y()-zoomPoint.y()<0)
        {
            h=h*-1;
        }
        //h= zoomPoint.y()-event->y();
        //QRectF rect(zoomPoint,event->pos());
        QRectF rect(zoomPoint.x(),zoomPoint.y(),w,h);
        rect = rect.normalized();
        //rect=rect.normalized();
        //rect.setHeight(rect.width()/chartKoef);
        qDebug()<<"w"<<w<<"h"<<h;
        qDebug()<<"ZoomRect"<<rect;
        zoomRect->setRect(rect);
        /*QRectF rect=zoomRect->rect();
        if (rect.bottom()<event->pos().y())
        {
            rect.setBottom(event->pos().y());
        }
        else
        {
            rect.setTopRight(event->pos());
        }
        zoomRect->setRect(rect.normalized());*/
    }
}

void TestChartView::mouseReleaseEvent(QMouseEvent *event)
{
    if (zoomRect)
    {
        delete zoomRect;
        zoomRect=Q_NULLPTR;
    }
    QChartView::mouseReleaseEvent(event);
}
void TestChartView::repositionLabels()
{
    QList<LineLabelRect*>::iterator it = lineRects.begin();
    QPointF newPos;
    QValueAxis *axis = qobject_cast<QValueAxis*>(this->chart()->axisX());
    for ( ; it!=lineRects.end();++it)
    {
//        if ((*it)->getAnchor().x()<axis->min() || (*it)->getAnchor().x()>axis->max())
//        {
//            (*it)->hide();
//        }
//        else
//        {
            QPointF p(20*(cosf(lineAngle)-sinf(lineAngle)),-18-36*sinf(lineAngle)); //18 - половина высоты этикетки, 36 -высота этикетки
           // qDebug()<<p;
            newPos = this->chart()->mapToPosition((*it)->getAnchor())+p;// QPointF(20*(cosf(lineAngle)-sinf(lineAngle)),0);
            if (newPos.y()<20)
            {
                newPos.setY(20);
            }
            if (newPos.x()>this->width())
            {
                newPos.setX(this->width()-55);
            }
            (*it)->setPos(newPos);
            (*it)->show();
//        }
    }


    QList<PointLabelRect*>::iterator pointIt = pointRects.begin();
    QRectF rect;
    rect.setLeft(axis->min());
    rect.setRight(axis->max());
    axis = qobject_cast<QValueAxis*>(this->chart()->axisY());
    rect.setTop(axis->min());
    rect.setBottom(axis->max());
    while (*pointIt!=pointRects.last()) {
        if (rect.contains((*pointIt)->getChartAnchor()))
        {

            (*pointIt)->setAnchor(this->chart()->mapToPosition((*pointIt)->getChartAnchor()));
            newPos = this->chart()->mapToPosition((*pointIt)->getChartAnchor())+QPointF(20,-30);
            if (newPos.x()+(*pointIt)->getRect().width() > this->width())
            {
                newPos.setX(newPos.x()-40.0-(*pointIt)->getRect().width());
            }
            (*pointIt)->setPos(newPos);
            (*pointIt)->show();
        }
        else
        {
            (*pointIt)->hide();
        }
        pointIt++;
    }
    pointRects.last()->hide();
}

LineLabelRect::LineLabelRect(QGraphicsItem *parent):
    QGraphicsItem(parent)
{

}

void LineLabelRect::setAnchor(QPointF point)
{
    m_anchor = point;
}

QPointF LineLabelRect::getAnchor()
{
    return m_anchor;
}

void LineLabelRect::setText(const QString &text)
{
   m_text=text;
   QFontMetrics metrics(m_font);
   m_textRect = metrics.boundingRect(QRect(0,0,20,20),Qt::AlignLeft,m_text);
   m_textRect.translate(0,0);
   prepareGeometryChange();
   m_rect=m_textRect.adjusted(-5,-5,5,5);
   //qDebug()<<"Mrect"<<m_rect;
}

void LineLabelRect::setLineStatus(const bool &stat)
{
    lineStatus = stat;
}

QRectF LineLabelRect::boundingRect() const
{
    return m_rect;
}

void LineLabelRect::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    if (lineStatus)
    {
        painter->setPen(QPen(QBrush(Qt::green),2.0,Qt::SolidLine));
        painter->setBrush(QBrush(QColor(193, 255, 186),Qt::SolidPattern));

    }
    else
    {
        painter->setPen(QPen(QBrush(Qt::red),2.0,Qt::SolidLine));
        painter->setBrush(QBrush(QColor(255, 173, 170),Qt::SolidPattern));
    }
    painter->drawRoundedRect(m_rect, 5, 5);
    painter->setPen(QPen(QBrush(QColor(0,0,0)),2.0,Qt::SolidLine));
    painter->drawText(m_textRect, m_text);
}




PointLabelRect::PointLabelRect(QGraphicsItem *parent):
    QGraphicsTextItem(parent)
{

}

QRectF PointLabelRect::boundingRect() const
{
    QPointF anchor = mapFromParent(m_anchor);
    QRectF rect = m_rect;
    rect.setLeft(qMin(m_rect.left(),anchor.x()));
    rect.setRight(qMax(m_rect.right(),anchor.x()));

    return rect;
}
void PointLabelRect::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    QPointF point1 = mapFromParent(m_anchor);
    QPointF point2 = m_rect.center();
    if (point1.x()<0)
    {
        point2.setX(m_rect.left());
    }
    else
    {
        point2.setX(m_rect.right());
    }
    QColor color;
    color = testError ? Qt::red : Qt::green;
    painter->setPen(QPen(QBrush(color),1.0,Qt::SolidLine));
    painter->setBrush(QBrush(QColor(255, 255, 255),Qt::SolidPattern));
    painter->drawLine(point1,point2);
    painter->setPen(QPen(QBrush(color),2.0,Qt::SolidLine));
    painter->drawRoundedRect(m_rect,5,5);
    painter->drawEllipse(point2,2,2);
    QGraphicsTextItem::paint(painter,option,widget);
}

void PointLabelRect::setTestPoint(const TestPoint &point)
{
    QString text;
    QColor color;
    text = QString ("<table align = center>"
                    "<tr> <td align = center> Линия </td> <td align = center>%1</td></tr>"
                    "<tr> <td align = center> Пикет </td> <td align = center>%2</td></tr>").arg(point.getLine()).arg(point.getPoint());
    if (point.getTestError())
    {
        color = Qt::red;
    }
    color = point.getTiltError()? Qt::red : Qt::green;
    text.append(QString("<tr> <td align = center> Tilt </td> <td align = center bgcolor=%1>%2</td></tr>").arg(color.name()).arg(point.getTilt()));
    color = point.getResistanceError()? Qt::red : Qt::green;
    text.append(QString("<tr> <td align = center> Resistance </td> <td align = center bgcolor=%1>%2</td></tr>").arg(color.name()).arg(point.getResistance()));
    color = point.getLeakageError()? Qt::red : Qt::green;
    text.append(QString("<tr> <td align = center> Leakage </td> <td align = center bgcolor=%1>%2</td></tr>").arg(color.name()).arg(point.getLeakage()));
    text.append("</table");
    this->setHtml(text);
    m_chartAnchor = point.getPointF();
    testError = point.getTestError();
    m_rect = QGraphicsTextItem::boundingRect().adjusted(-3,-3,3,3);
}


QRectF PointLabelRect::getRect() const
{
    return m_rect;
}
QPointF PointLabelRect::getChartAnchor() const
{
    return m_chartAnchor;
}
void PointLabelRect::setAnchor(const QPointF &point)
{
    m_anchor = point;
}
