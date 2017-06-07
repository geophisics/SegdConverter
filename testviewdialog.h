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


private:
    Ui::TestViewDialog *ui;
    TestMap testMap;
    XFileMap xMap;



    QtCharts::QScatterSeries *p_badScatterSeries;
    QtCharts::QScatterSeries *p_goodScatterSeries;

    QSettings *settings;

    QVector<TestPoint> pointsInTemplate;

private:
    void setBadScatterSeries(QMap<uint, QPair<uint, bool> > lines);
    void setGoodScatterSeries();
    void readSettings();
    void saveSettings();

public slots:
    void showTemplatesByFfid(const uint &f);
    void setGoodScatterVisible(const bool &b);
    void setBadScatterVisible(const bool &b);
    void setGoodScatterSize(const int &size);
    void setBadScatterSize(const int &size);

signals:
    void sendLineLabel(QPointF,QString,bool);
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

    void addLineLabel(const QPointF coordinates, const QString &txt, const bool &status);
    void removeLineLabels();
    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void chartChanged();

private:
    QList <LineLabelRect*> lineRects;
    QList <PointLabelRect*> pointRects;
    void repositionLabels();

signals:
    void mousePressedWithCtrl(QPointF);

};


class LineLabelRect: public QGraphicsItem
{
public:
    LineLabelRect(QGraphicsItem *parent = Q_NULLPTR);
    void setAnchor(QPointF point);
    QPointF getAnchor();
    void setText(const QString &text);
    void setLineStatus(const bool &stat);
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
private:
    QRectF m_rect;
    QPointF m_anchor;
    QLine m_line;
    QString m_text;
    QRectF m_textRect;
    QFont m_font;
    bool lineStatus;
};

class PointLabelRect:public QGraphicsTextItem
{
public:
    PointLabelRect(QGraphicsItem *parent = Q_NULLPTR);
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setTestPoint(const TestPoint &point);
    void setAnchor(const QPointF &point);
    QPointF getChartAnchor() const;
    QRectF getRect() const;
private:
    TestPoint m_tpoint;
    QString m_text;
    QRectF m_rect;
    QFont m_font;
    QPointF m_chartAnchor;
    QPointF m_anchor;
    bool testError;
};




#endif // TESTVIEWDIALOG_H
