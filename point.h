#ifndef POINT_H
#define POINT_H

#include <QObject>
#include <QVector>
#include <QQueue>
struct Template;
class Point : public QObject
{
    Q_OBJECT
public:
    explicit Point(QObject *parent = 0);
    explicit Point(QString &str, QObject *parent = 0);
    void setLine(const int &l);
    void setPoint(const int &p);
    QString getLinePoint();
    uint getX();
    uint getY();
    uint getZ();
private:
    int line;
    int point;
    float x;
    float y;
    float z;
    QString linePoint;
signals:

public slots:
};

class XFile
{

public:
    explicit XFile();
    explicit XFile(QQueue<QString> strList);
    uint getLine();
    uint getPoint();
    QQueue<Template>* getTemplates();
    bool checkTemplates();
private:
    uint ffid;
    uint sourceLine;
    uint sourcePoint;
    QQueue<Template> *templates;

    //QVector<Template> templates;
};

struct Template
{
    Template();
    Template(const QString &str);
    uint increment;
    uint firstChannel;
    uint lastChannel;
    uint receiverLine;
    uint firstReceiver;
    uint lastReceiver;
};

#endif // POINT_H
