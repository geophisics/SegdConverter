#ifndef TESTMODEL_H
#define TESTMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
#include <SUB/general.h>
class TestModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    TestModel(QObject *parent =Q_NULLPTR);
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;

    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    void setTestMap(TestMap *map);
private:

    QStringList headers;
    int rows;
    int columns;
    TestMap *tMap;
    QVector<TestPoint> testData;


};

class TestSortFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT


public:
    enum testType {badTest, okTest};
    TestSortFilterModel(QObject *parent=Q_NULLPTR);
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
    bool filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const;
    void setTestType(const testType &t);
    void setXFile(const XFile &x);
private:
    testType type;
    XFile xFile;

};

#endif // TESTMODEL_H
