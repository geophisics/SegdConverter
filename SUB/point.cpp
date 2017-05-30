#include "point.h"
#include "math.h"
#include <QQueue>
#include <QVector>
Point::Point( )
{
    line = 0;
    point = 0;
    x = 0;
    y = 0;
    z = 0;
    linePoint ="NoPoint";// QString::number(line)+QString::number(point);
}

Point::Point(QString &str)
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

Point::Point(const Point &other)
{
    line = other.line;
    point = other.point;
    x = other.x;
    y = other.y;
    z = other.z;
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
uint Point::getX() const
{
    return round(x);
}
uint Point::getY() const
{
    return round(y);
}
uint Point::getZ() const
{
    return round(z);
}

TestPoint::TestPoint()
{
    line = 0;
    point = 0;
    x = 0;
    y = 0;
    z = 0;

    resistanceError = false;
    tiltError = false;
    leakageError = false;

    testError= false;

}
TestPoint::TestPoint(const TestPoint &other)
{
    line = other.line;
    point = other.point;
    x = other.x;
    y = other.y;
    z = other.z;
    resistanceValue = other.resistanceValue;
    resistanceError = other.resistanceError;
    tiltValue = other.tiltValue;
    tiltError = other.tiltError;
    leakageValue = other.leakageValue;
    leakageError = other.leakageError;
    testError = other.testError;
}

void TestPoint::setX(const float &f)
{
    x = f;
}
void TestPoint::setY(const float &f)
{
    y = f;
}
void TestPoint::setZ(const float &f)
{
    z = f;
}

void TestPoint::setResistance(const float &f)
{
    resistanceValue =f;
}

void TestPoint::setTilt(const float &f)
{
    tiltValue = f;
}
void TestPoint::setLeakage(const float &f)
{
    leakageValue = f;
}

void TestPoint::setResistanceError(const bool &b)
{
    resistanceError =b;
}
void TestPoint::setTiltError(const bool &b)
{
    tiltError = b;
}
void TestPoint::setLeakageError(const bool &b)
{
    leakageError =b;
}

void TestPoint::setTestStatus(const bool &b)
{
    testError = b;
}

void TestPoint::checkTestStatus()
{
    testError = (tiltError || resistanceError || leakageError);
}

int TestPoint::getLine() const
{
    return line;
}
int TestPoint::getPoint() const
{
    return point;
}
float TestPoint::getX() const
{
    return x;
}

float TestPoint::getY() const
{
    return y;
}

float TestPoint::getZ() const
{
    return z;
}
bool TestPoint::getTestError() const
{
    return testError;
}

float TestPoint::getLeakage() const
{
    return leakageValue;
}

bool TestPoint::getLeakageError() const
{
    return leakageError;
}

bool TestPoint::getTiltError() const
{
    return tiltError;
}

float TestPoint::getTilt() const
{
    return tiltValue;
}

float TestPoint::getResistance() const
{
    return resistanceValue;
}

bool TestPoint::getResistanceError() const
{
    return resistanceError;
}
XFile::XFile()
{

}
XFile::XFile(QQueue<QString> strList)
{
    QString tmp = strList.dequeue();
    //templates = new QQueue<Template>();
    ffid = tmp.mid(7,4).toUInt();
    sourceLine = tmp.mid(13,16).toUInt();
    sourcePoint = tmp.mid(29,8).toUInt();
    templates.enqueue(Template(tmp));
    while (!strList.isEmpty()) {
        templates.enqueue(Template(strList.dequeue()));
    }
}

XFile::XFile(const uint &fileNum, const uint &line, const uint &point)
{
    ffid = fileNum;
    sourceLine = line;
    sourcePoint = point;
    //templates = new QQueue<Template>();
}
uint XFile::getFfid()
{
    return ffid;
}

uint XFile::getLine()
{
    return sourceLine;
}
uint XFile::getPoint()
{
    return sourcePoint;
}


void XFile::setFfid(const uint &f)
{
    ffid = f;
}

void XFile::setSourceLine(const uint &line)
{
    sourceLine = line;
}

void XFile::setSourcePoint(const uint &point)
{
    sourcePoint = point;
}

bool XFile::pointInTemplate(const uint &line, const uint &point) const
{
    foreach (Template t, templates) {
        if (t.receiverLine == line && t.firstReceiver<=point && t.lastReceiver >= point) {
            return true;
        }
    }
    return false;
}
QQueue<Template> XFile::getTemplates()
{
    return templates;
}
bool XFile::checkTemplates()
{
    uint lastChannel=0;
    foreach (Template t, templates) {
        if (t.firstReceiver+(t.lastChannel-t.firstChannel)*t.increment!=t.lastReceiver || lastChannel >= t.firstChannel)
        {
            return false;
        }
        lastChannel = t.lastChannel;
    }
    return true;
}

void XFile::addLineToTemplate(Template templ)
{
    templates.enqueue(templ);
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

Template::Template(const Template &other)
{
    increment = other.increment;
    firstChannel = other.firstChannel;
    lastChannel = other.lastChannel;
    receiverLine = other.receiverLine;
    firstReceiver = other.firstReceiver;
    lastReceiver = other.lastReceiver;
}
Template::Template()
{
    increment = 1;
    firstChannel = 0;
    lastChannel = 0;
    receiverLine = 0;
    firstReceiver = 0;
    lastReceiver = 0;
}
