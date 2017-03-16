#include "exclusion.h"
#include "math.h"
#include "float.h"
#include <QDebug>
#include <QPolygonF>
Exclusion::Exclusion()
{

}
void Exclusion::setName(const QString n)
{
    name = n;
}

void Exclusion::setType(const int &t)
{
    type = t;
}
void Exclusion::addCircle(const QString &rad, const QString &coordinats)
{
    Circle *p_circle = new Circle(rad.toFloat(),coordinats);
    circles.append(p_circle);
}
void Exclusion::addPoligon(const QStringList &coordinats)
{
    QVector<QPointF> coords;
    QStringList tmp;

    foreach (QString str, coordinats) {
        tmp = str.split(" ",QString::SkipEmptyParts);
        coords.append(QPointF(tmp.value(0).toFloat(),tmp.value(1).toFloat()));
    }
    polygons.append(new QPolygonF(coords));
}

void Exclusion::addLine(const QString &rad, const QStringList &coordinats)
{
    for (int i=0; i<coordinats.count();++i)
    {
        circles.append(new Circle(rad.toFloat(),coordinats.value(i)));
        if (i+1<coordinats.count())
        {
            polygons.append(new QPolygonF(polygonFromLine(rad.toFloat(),coordinats.value(i),coordinats.value(i+1))));
        }
    }
}

QVector<QPointF>  Exclusion::polygonFromLine(const float &r, const QString &begin, const QString &end)
{
    QVector<QPointF> result;
    QStringList coord = begin.split(" ",QString::SkipEmptyParts);
    QPointF beginCoordinats = QPointF(coord.value(0).toFloat(), coord.value(1).toFloat());
    coord = end.split(" ",QString::SkipEmptyParts);
    QPointF endCoordinats= QPointF(coord.value(0).toFloat(), coord.value(1).toFloat());
    float dX = endCoordinats.rx()-beginCoordinats.rx();
    float dY = endCoordinats.ry()-beginCoordinats.ry();
    float length = sqrt(powf(dX,2)+powf(dY,2));
    dX = dX*r/length;
    dY = dY*r/length;
    result.append(QPointF(beginCoordinats.rx()+dY,beginCoordinats.ry()-dX));
    result.append(QPointF(beginCoordinats.rx()-dY,beginCoordinats.ry()+dX));
    result.append(QPointF(endCoordinats.rx()-dY,endCoordinats.ry()+dX));
    result.append(QPointF(endCoordinats.rx()+dY,endCoordinats.ry()-dX));
    result.append(QPointF(dY+beginCoordinats.rx(),beginCoordinats.ry()-dX ));
    return result;
}
bool Exclusion::checkPoint(const float &x, const float &y)
{
    QPointF point = QPointF(x,y);
    foreach (Circle *circ, circles) {
        if (circ->pointInCircle(point))
        {
            return true;
        }
    }
    foreach (QPolygonF *pol, polygons) {
        if (pol->containsPoint(point,Qt::OddEvenFill))
        {
            return true;
        }
    }
    return false;
}

Circle::Circle()
{
    radius =0.0;

    center = QPointF(0.0,0.0);
}
Circle::Circle(const float &r, const QString &coordinats)
{
    radius = r;
    QStringList coord = coordinats.split(" ",QString::SkipEmptyParts);
    float x = coord.value(0).toFloat();
    float y = coord.value(1).toFloat();
    center = QPointF(x,y);

}

bool Circle::pointInCircle(const QPointF &p)
{
    if (sqrtf(powf(center.x()-p.x(),2)+powf(center.y()-p.y(),2)) <= radius )
    {
        return true;
    }
    else
    {
        return false;
    }
}
//Poligon::Poligon(QStringList coords)
//{
//    float x,y;
//    QStringList tmpList;
//    foreach (QString str, coords) {
//        tmpList = str.split(" ",QString::SkipEmptyParts);
//        x = tmpList.value(0).toFloat();
//        y= tmpList.value(1).toFloat();
//        coordinats.append(new QPointF(x,y));
//    }
//}


//bool Poligon::pointInPoligon(const QPointF &point)
//{
//    int numOfIntersection=0;
//    for (int i=0; i<coordinats.count()-1;++i)
//    {
//        if (isLinePartsIntersected(point,coordinats.value(i),coordinats.value(i+1)))
//        {
//            numOfIntersection++;
//        }
//    }
//    if (numOfIntersection % 2 ==0)
//    {
//        return true;
//    }
//    else
//    {
//        return false;
//    }
//}
//bool Poligon::isLinePartsIntersected(const QPointF p, const QPointF *begin, const QPointF *end)
//{
//    float common = (begin->y()-end->y())*(1000000000.0-p.x());
//    if (common ==0)
//    {
//        return false;
//    }
//    float rH = (begin->y()-p.y())*(FLT_MAX-p.x());
//    float sH = (begin->y()-p.y())*(end->x()-begin->x())-(begin->x()-p.x())*(end->y()-begin->y());

//    rH = rH/common;
//    sH = sH/common;

//    if (rH >=0 && rH<=1 && sH>=0 && sH<=1)
//    {
//        return true;
//    }
//    else
//    {
//        return false;
//    }
//}
