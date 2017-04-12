#ifndef ATTRIBUTESMODEL_H
#define ATTRIBUTESMODEL_H

#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
#include <QSet>
#include "SUB/general.h"


#include <QSettings>

//typedef QVector<QVariant> segdAttributes;
class AttributesModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    AttributesModel(QObject *parent=Q_NULLPTR);
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    void setHeaders(QSettings *settings);
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QStringList* getHeaders();
    CountedAttributes* getAttributes();
    bool dataStatus();

private:
    //QSettings *settings;
    int rows;
    int columns;
    QStringList headers;
    CountedAttributes attributes;
    bool dataSaved;
    //QVector<segdAttributes> attributes;
public slots:
    void receiveFfidData();
    bool saveDataInXlsx(const QString &path);
};

class AttributesSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    AttributesSortFilterProxyModel(QObject *parent =0);
    bool filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const;
    QSet<int>* getVisibleColumns();
    void setVisibleColumns(QSettings *settings);
private:
    QSet<int> visibleColumns;
public slots:
    void resetVisibleColumns();

};

#endif // ATTRIBUTESMODEL_H
