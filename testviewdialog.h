#ifndef TESTVIEWDIALOG_H
#define TESTVIEWDIALOG_H

#include <QDialog>
#include <SUB/general.h>
#include <Models/testmodel.h>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QVXYModelMapper>
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

private:
    Ui::TestViewDialog *ui;
    TestMap testMap;
    TestModel *p_TestModel;
    TestSortFilterModel *p_filterTestModel;
    QtCharts::QScatterSeries *s_series;
    QtCharts::QVXYModelMapper *modelMappper;

public slots:
    void newTestReceived();

};

#endif // TESTVIEWDIALOG_H
