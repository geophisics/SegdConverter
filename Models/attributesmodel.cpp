#include "attributesmodel.h"

AttributesModel::AttributesModel()
{

}

int AttributesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return rows;
}

int AttributesModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return columns;
}
