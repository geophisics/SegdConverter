#include "testmodel.h"

TestModel::TestModel(QObject *parent)
{
    headers<<"Line"<<"Point"<<"X"<<"Y"<<"Test Status";
    columns = 5;
    rows = 0;
}

int TestModel::columnCount(const QModelIndex &parent) const
{
    return columns;
}

int TestModel::rowCount(const QModelIndex &parent) const
{
    return rows;
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

QVariant TestModel::data(const QModelIndex &index, int role) const
{
  /*  if (role == Qt::DisplayRole)
    {
        return testData.at(index.row()).at(index.column());
    }*/
    return QVariant();
}
