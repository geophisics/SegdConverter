#ifndef EXCLUSION_H
#define EXCLUSION_H

#include <QObject>
#include <QPointF>
#include <QVector>
struct Circle;
struct Poligon;

class Exclusion
{

public:
    Exclusion();
    void setName(const QString n);
    void setType(const int &t);
    void addCircle(const QString &rad, const QString &coordinats);
    void addLine(const QString &rad, const QStringList &coordinats);
    void addPoligon(const QStringList &coordinats);
    bool checkPoint(const float &x, const float &y);
    static QVector<QPointF> polygonFromLine(const float &r, const QString &begin,const QString &end);
private:
    QVector<Circle*> circles;
    QVector<QPolygonF*> polygons;
    int type;
    QString name;
};



struct Circle
{
    Circle();
    Circle(const float &r, const QString &coordinats);
    bool pointInCircle(const QPointF &p);
    QPointF center;
    float radius;
};


#endif // EXCLUSION_H
