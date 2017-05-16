#include "testmodel.h"
#include <QBrush>
TestModel::TestModel(QObject *parent):QAbstractTableModel(parent)
{
    headers<<"Line"<<"Point"<<"X"<<"Y"<<"Resistance"<<"Tilt"<<"Leakage"<<"Test Status";
    columns = 8;
    rows = 0;
    tMap=NULL;
}

int TestModel::columnCount(const QModelIndex &parent) const
{
    return columns;
}

int TestModel::rowCount(const QModelIndex &parent) const
{
    if (tMap)
    {
        return tMap->count();
    }
    else
    {
        return 0;
    }
}

QVariant TestModel::headerData(int section, Qt::Orientation orientation, int role) const
{
       if (role == Qt::DisplayRole)
       {
           if (orientation == Qt::Horizontal)
           {
               return headers[section];
           }
           else
           {
               return section+1;
           }
       }
       return QVariant();
}

void TestModel::setTestMap(TestMap *map)
{
    tMap = map;
}

QVariant TestModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        switch (index.column()) {
        case 0:
            return tMap->values().at(index.row()).getLine();
            break;
        case 1:
            return tMap->values().at(index.row()).getPoint();
            break;
        case 2:
            return tMap->values().at(index.row()).getX();
            break;
        case 3:
            return tMap->values().at(index.row()).getY();
            break;
        case 4:
            return tMap->values().at(index.row()).getResistance();
            break;
        case 5:
            return tMap->values().at(index.row()).getTilt();
            break;
        case 6:
            return tMap->values().at(index.row()).getLeakage();
            break;
        case 7:
            return tMap->values().at(index.row()).getTestError() ? "Bad" : "Ok";
        default:
            break;
        }
    }
    if (role == Qt::BackgroundRole)
    {
        switch (index.column()) {
        case 4:
            return QBrush(tMap->values().at(index.row()).getResistanceError() ?  Qt::red : Qt::white,Qt::SolidPattern);
            break;
        case 5:
            return QBrush(tMap->values().at(index.row()).getTiltError() ?  Qt::red : Qt::white,Qt::SolidPattern);
            break;
        case 6:
            return QBrush(tMap->values().at(index.row()).getLeakageError() ?  Qt::red : Qt::white,Qt::SolidPattern);
            break;
        case 7: return QBrush(tMap->values().at(index.row()).getTestError() ?  Qt::red : Qt::white,Qt::SolidPattern);
        default:
            return (QBrush(Qt::white,Qt::SolidPattern));
            break;
        }
    }
    return QVariant();
}


TestSortFilterModel::TestSortFilterModel(QObject *parent):QSortFilterProxyModel(parent)
{

}
void TestSortFilterModel::setTestType(const testType &t)
{
    type = t;
}

bool TestSortFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    QModelIndex index = sourceModel()->index(source_row,7,source_parent);
    QString str = sourceModel()->data(index).toString();
    if (type==testType::badTest && str == "Bad")
    {
        return true;
    }
    if (type == testType::okTest && str == "Ok")
    {
        return true;
    }
    return false;
}


bool TestSortFilterModel::filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const
{
    return true;
}
