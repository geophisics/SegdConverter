#ifndef ATTRIBUTESMODEL_H
#define ATTRIBUTESMODEL_H

#include <QAbstractTableModel>

class AttributesModel : public QAbstractTableModel
{
public:
    AttributesModel();
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
private:
    int rows;
    int columns;
};

#endif // ATTRIBUTESMODEL_H
