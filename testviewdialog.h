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

public slots:
    void newTestReceived();
    void showAuxesByFfid(const uint &f);

};

#endif // TESTVIEWDIALOG_H
