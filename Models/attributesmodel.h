#ifndef ATTRIBUTESMODEL_H
#define ATTRIBUTESMODEL_H

#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
#include "SUB/general.h"


//#include <QSettings>

//typedef QVector<QVariant> segdAttributes;
class AttributesModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    AttributesModel(QObject *parent=Q_NULLPTR);
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    void setHeaders();
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    CountedAttributes* getAttributes();

private:
    //QSettings *settings;
    int rows;
    int columns;
    QStringList headers;
    CountedAttributes attributes;

    //QVector<segdAttributes> attributes;
public slots:
    void receiveFfidData();
    void saveDataInXlsx(const QString &path);
};



#endif // ATTRIBUTESMODEL_H
