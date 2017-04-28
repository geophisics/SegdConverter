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
    float getMaxValueInColumn(const int &column);
    float getMinValueInColumn(const int &column);
    int getFirstColumnValue(QModelIndex &index);

private:
    int rows;
    int columns;
    QStringList headers;
    CountedAttributes attributes;
    bool dataSaved;

public slots:
    void receiveFfidData();
    bool saveDataInXlsx(const QString &path);

private slots:
    void setDataUnsaved();

};

class AttributesSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT


public:
    //friend void SegdConverterWindow::filtersEnabled(const bool &);
    const float EPS = 0.00000000001;
    AttributesSortFilterProxyModel(QObject *parent =0);
    bool filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const;
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
    QSet<int>* getVisibleColumns();
    void setVisibleColumns(QSettings *settings);
    AttributesRange getFirstFilterRange();
    AttributesRange getSecondFilterRange();
    AttributesRange getThirdFilterRange();
private:
    QSet<int> visibleColumns;
    int firstFilterColumn;
    int secondFilterColumn;
    int thirdFilterColumn;

    AttributesRange firstRange;
    AttributesRange secondRange;
    AttributesRange thirdRange;

    bool dataInRange(const float &f, const AttributesRange &range) const;
public slots:
    void resetVisibleColumns();
    void setFirstFilterColumn(const int &i);
    void setSecondFilterColumn(const int &i);
    void setThirdFilterColumn(const int &i);
    void setFirstRange(const float &min,const float &max);
    void setSecondRange(const float &min,const float &max);
    void setThirdRange (const float &min,const float &max);

};

#endif // ATTRIBUTESMODEL_H
