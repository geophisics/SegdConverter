#include "point.h"
#include "math.h"
#include <QQueue>
#include <QVector>
Point::Point(QObject *parent) : QObject(parent)
{

}

Point::Point(QString &str, QObject *parent):QObject(parent)
{
    QString tmp = str.mid(1,16);
    line = tmp.toInt();
    tmp = str.mid(17,8);
    point = tmp.toInt();
    tmp = str.mid(46,9);
    x=tmp.toFloat();
    tmp = str.mid(55,10);
    y=tmp.toFloat();
    tmp = str.mid(65,6);
    z=tmp.toFloat();
    linePoint = QString::number(line)+QString::number(point);
}

QString Point::getLinePoint()
{
    return linePoint;
}
void Point::setLine(const int &l)
{
    line = l;
}
void Point::setPoint(const int &p)
{
    point = p;
}
uint Point::getX()
{
    return round(x);
}
uint Point::getY()
{
    return round(y);
}
uint Point::getZ()
{
    return round(z);
}

XFile::XFile()
{

}
XFile::XFile(QQueue<QString> strList)
{
    QString tmp = strList.dequeue();
    templates = new QQueue<Template>();
    ffid = tmp.mid(7,4).toUInt();
    sourceLine = tmp.mid(13,16).toUInt();
    sourcePoint = tmp.mid(29,8).toUInt();
    templates->enqueue(Template(tmp));
    while (!strList.isEmpty()) {
        templates->enqueue(Template(strList.dequeue()));
    }
}

uint XFile::getLine()
{
    return sourceLine;
}
uint XFile::getPoint()
{
    return sourcePoint;
}
QQueue<Template>* XFile::getTemplates()
{
    return templates;
}
bool XFile::checkTemplates()
{
    uint lastChannel=0;
    foreach (Template t, *templates) {
        if (t.firstReceiver+(t.lastChannel-t.firstChannel)*t.increment!=t.lastReceiver || lastChannel >= t.firstChannel)
        {
            return false;
        }
        lastChannel = t.lastChannel;
    }
    return true;
}
Template::Template(const QString &str)
{
    firstChannel = str.mid(38,4).toInt();
    lastChannel = str.mid(42,4).toInt();
    increment = str.mid(46,1).toInt();
    if (increment==0)
    {
        increment=1;
    }
    receiverLine = str.mid(47,16).toInt();
    firstReceiver = str.mid(63,8).toInt();
    lastReceiver = str.mid(71,8).toInt();
}
Template::Template()
{
    firstChannel = 0;
    lastChannel = 0;
    receiverLine = 0;
    firstReceiver = 0;
    lastReceiver = 0;
}
