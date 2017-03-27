#include "attributesmodel.h"
#include <QSettings>
#include <QCoreApplication>
#include <QDir>
#include <QDebug>

AttributesModel::AttributesModel(QObject *parent):QAbstractTableModel(parent)
{
    headers<<"FFID"<<"Line"<<"Point"<<"Source X"<<"Source Y"<<"Source Z";
    columns =headers.count();
    rows =0;
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

void AttributesModel::setHeaders()
{
    headers =headers.mid(0,6);
    columns =6;
    QSettings *settings = new QSettings(QCoreApplication::applicationDirPath()+QDir::separator()+"config.ini",QSettings::IniFormat,this);
    settings->beginGroup("/ConvertSettings");
    if (settings->value("/AnalisysAuxes",false).toBool())
    {
        headers<<"Check Aux";
        columns++;
    }
    if (settings->value("/CheckTests",false).toBool())
    {
        headers<<"Check Tests";
        columns++;
    }
    settings->endGroup();
    settings->beginGroup("/WindowsSettings");
    int size = settings->beginReadArray("/Windows");
    for (int i =0; i<size; ++i)
    {
        settings->setArrayIndex(i);
        if (settings->value("/CountAmpl",false).toBool())
        {
            headers<<QString("Abs A#%1").arg(i+1);
            columns++;
        }
        if (settings->value("/CountRms","0").toBool())
        {
            headers<<QString("RMS #%1").arg(i+1);
            columns++;
        }
        if (settings->value("/CountFreq","0").toBool())
        {
            headers<<QString("Frq #%1").arg(i+1);
            columns++;
        }
        if(settings->value("/CountEnergy","0").toBool())
        {
            headers<<QString("Energy #%1").arg(i+1);
            columns++;
        }
        if(settings->value("/CountDfr","0").toBool())
        {
            headers<<QString("DFR #%1").arg(i+1);
            columns++;
        }
    }
    settings->endArray();
    size = settings->beginReadArray("/Relations");
    for (int i =0; i<size; ++i)
    {
        settings->setArrayIndex(i);
        headers<< QString("%1/%2").arg(settings->value("/A1","A1").toString()).arg(settings->value("/A2","A1").toString());
        columns++;
    }
    settings->endArray();
    settings->endGroup();

    emit headerDataChanged(Qt::Horizontal,0,headers.count());
}


QVariant AttributesModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        return QString("%1.%2").arg(index.row()).arg(index.column());//attributes.at(index.row()).at(index.column());
    }
    else return QVariant();
}


QVariant AttributesModel::headerData(int section, Qt::Orientation orientation, int role) const
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
}

void AttributesModel::receiveFfidData(/*QVector<QVariant> *data*/)
{
   // beginInsertRows(QModelIndex(),1,1);
    //qDebug()<<data->count();
    beginInsertRows(QModelIndex(),rows,rows);//attributes.count(),attributes.count());
    //attributes.push_back(*data);
    rows++;
    endInsertRows();
    //qDebug()<<attributes.count();

    QModelIndex top = createIndex(attributes.count()-1,0);
    QModelIndex bottom = createIndex(attributes.count()-1,headers.count());
    emit dataChanged(top,bottom);
    //this->resetInternalData();
    //emit layoutChanged();
    //qDebug()<<rows;
    //endInsertRows();
    //
    //emit dataChanged(createIndex(0,0),createIndex(rows,columns));
   // this->resetInternalData();
    qDebug()<<rows;
    //emit dataChanged(createIndex(rowCount(),0),createIndex(rowCount(),columnCount()));
}
