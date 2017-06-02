#ifndef TESTVIEWDIALOG_H
#define TESTVIEWDIALOG_H

#include <QDialog>
#include <SUB/general.h>
#include <Models/testmodel.h>
#include <QtCharts/QChartView>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QVXYModelMapper>
#include <QSignalMapper>
#include <QGraphicsSimpleTextItem>

QT_CHARTS_USE_NAMESPACE
class LineLabelTextItem;
class LineLabelRect;

namespace Ui {
class TestViewDialog;
}

class TestViewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TestViewDialog(QWidget *parent = 0);
    ~TestViewDialog();
    TestMap* getTestMap();
    XFileMap* getXMap();

private:
    Ui::TestViewDialog *ui;
    TestMap testMap;
    XFileMap xMap;

    TestModel *p_TestModel;
    TestSortFilterModel *p_badFilterTestModel;
    TestSortFilterModel *p_goodFilterTestModel;
    QtCharts::QScatterSeries *p_badScatterSeries;
    QtCharts::QScatterSeries *p_goodScatterSeries;

    QtCharts::QVXYModelMapper *p_badModelMappper;
    QtCharts::QVXYModelMapper *p_goodModelMapper;


    QVector<uint> pointsInTemplate;


    QtCharts::QScatterSeries *p_lastPointScatterSeries;

private:
    void setBadScatterSeries();
    void setGoodScatterSeries();

public slots:
    void newTestReceived();
    void showAuxesByFfid(const uint &f);

    void setGoodScatterVisible(const bool &b);
    void setBadScatterVisible(const bool &b);
    void setAllScatterVisible(const bool &b);


    void setGoodScatterSize(const int &size);
    void setBadScatterSize(const int &size);

signals:
    void sendLineLabel(QPointF,QString);

};

class TestChartView : public QChartView
{
    Q_OBJECT
public:
    TestChartView(QWidget *parent =0);
    TestChartView(QChart *chart, QWidget *parent = 0);
public slots:

    void addLineLabel(const QPointF coordinates, const QString &txt);
    void recountLabelPositions();
    void removeLineLabels();

private:
    QList <LineLabelTextItem*> lineLabels;
    QList <LineLabelRect*> lineRects;


    //QList<QGraphicsSimpleTextItem*> lineNumbers;
    //QList<QPair<QPointF,QGraphicsSimpleTextItem*> > lineLabels;
};

class LineLabelTextItem : public QGraphicsSimpleTextItem
{
public:
    LineLabelTextItem(QGraphicsItem *parent = Q_NULLPTR);
    LineLabelTextItem(const QString &text, QGraphicsItem *parent = Q_NULLPTR);
    LineLabelTextItem(const QPointF &point, const QString &text, QGraphicsItem *parent = Q_NULLPTR);
    QPointF getCoordinates();

private:
    QPointF coordinates;

};

class LineLabelRect: public QGraphicsItem
{
public:
    LineLabelRect(QGraphicsItem *parent = Q_NULLPTR);
    void setAnchor(QPointF point);
    void setText(const QString &text);
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
private:
    QRectF m_rect;
    QPointF m_anchor;
    QLine m_line;
    QString m_text;
    QRectF m_textRect;
    QFont m_font;
};

#endif // TESTVIEWDIALOG_H
