#ifndef TESTMODEL_H
#define TESTMODEL_H

#include <QObject>
#include <QAbstractTableModel>
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
private:

    QStringList headers;
    int rows;
    int columns;
    TestMap tMap;
    QVector<TestPoint> testData;

};

#endif // TESTMODEL_H
