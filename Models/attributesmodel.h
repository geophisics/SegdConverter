#ifndef ATTRIBUTESMODEL_H
#define ATTRIBUTESMODEL_H

#include <QAbstractTableModel>
//#include <QSettings>

typedef QVector<QVariant> segdAttributes;
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


private:
    //QSettings *settings;
    int rows;
    int columns;
    QStringList headers;
    QVector<segdAttributes> attributes;
public slots:
    void receiveFfidData();
};

#endif // ATTRIBUTESMODEL_H
