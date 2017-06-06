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
#include <QSettings>

QT_CHARTS_USE_NAMESPACE
class LineLabelTextItem;
class LineLabelRect;
class PointLabelRect;

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
    void setSettings(QSettings *set);
    void readSettings();
    void saveSettings();

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

    QSettings *settings;

    //QVector<uint> pointsInTemplate;
    QVector<TestPoint> pointsInTemplate;


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
private slots:
    void mouseUnderPoint(const QPointF &coordinates, const bool &b);
    void mousePressedOnChart(const QPointF &coordinates);
};

class TestChartView : public QChartView
{
    Q_OBJECT
public:
    TestChartView(QWidget *parent =0);
    TestChartView(QChart *chart, QWidget *parent = 0);
    void addPointLabel();
    void hideLastPointLabel();
    void setLastPointLabel(const TestPoint &point);


public slots:

    void addLineLabel(const QPointF coordinates, const QString &txt);
    void recountLabelPositions();
    void removeLineLabels();
    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void chartChanged();

private:
    //QList <LineLabelTextItem*> lineLabels;
    QList <LineLabelRect*> lineRects;
    QList <PointLabelRect*> pointRects;
    void repositionLabels();

signals:
    void mousePressedWithCtrl(QPointF);

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
    QPointF getAnchor();
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

class PointLabelRect:public QGraphicsTextItem
{
public:
    PointLabelRect(QGraphicsItem *parent = Q_NULLPTR);
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setTestPoint(const TestPoint &point);
    QRectF getRect() const;
private:
    TestPoint m_tpoint;
    QString m_text;
    QRectF m_textRect;
    QRectF m_rect;
    QFont m_font;
};




#endif // TESTVIEWDIALOG_H
