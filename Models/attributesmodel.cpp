#include "attributesmodel.h"
#include <QSettings>
#include <QCoreApplication>
#include <QDir>
#include <QDebug>
#include <QBrush>
#include <QtXlsx>
AttributesModel::AttributesModel(QObject *parent):QAbstractTableModel(parent)
{
    headers<<"FFID"<<"Line"<<"Point"<<"Source X"<<"Source Y"<<"Source Z";
    columns =headers.count();
    rows =0;
}

int AttributesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return attributes.count();
}

int AttributesModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return columns;
}

void AttributesModel::setHeaders(QSettings *settings)
{
 //   emit beginResetModel();
    attributes.clear();
    headers =headers.mid(0,6);
    columns =6;
   // QSettings *settings = new QSettings(QCoreApplication::applicationDirPath()+QDir::separator()+"config.ini",QSettings::IniFormat,this);
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
    endResetModel();
   // emit headerDataChanged(Qt::Horizontal,0,headers.count());
}

QVariant AttributesModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case 0:
        return attributes.value(index.row()).at(index.column()).first;
        break;
    case 8:
        return QBrush(attributes.value(index.row()).at(index.column()).second ? Qt::white : Qt::red,Qt::SolidPattern);
    default:
        return QVariant();
        break;
    }
    return QVariant();
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
   return QVariant();
}

CountedAttributes* AttributesModel::getAttributes()
{
    return &attributes;
}
QStringList* AttributesModel::getHeaders()
{
    return &headers;
}

bool AttributesModel::dataStatus()
{
    return dataSaved;
}
void AttributesModel::receiveFfidData(/*QVector<QVariant> *data*/)
{
    emit layoutChanged();
}

bool AttributesModel::saveDataInXlsx(const QString &path)
{
    QXlsx::Document xlsx(path);
    QXlsx::Format xlsxFormat;
    xlsxFormat.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    xlsxFormat.setVerticalAlignment(QXlsx::Format::AlignVCenter);
    xlsxFormat.setPatternBackgroundColor(Qt::white);
    xlsxFormat.setBorderStyle(QXlsx::Format::BorderThin);
    for(int i=0;i<headers.count();++i)
    {
        xlsx.write(1,i+1,headers.at(i));
    }
    for (int i=0;i<attributes.count();i++)
    {
        for(int j=0;j<headers.count();j++)
        {
            if (attributes.value(i).value(j).second)
            {
                xlsxFormat.setPatternBackgroundColor(Qt::white);
            }
            else
            {
                xlsxFormat.setPatternBackgroundColor(Qt::red);
            }
            xlsx.write(i+2,j+1,attributes.value(i).value(j).first,xlsxFormat);
        }
    }
    if (xlsx.save()) {
        dataSaved = true;
        return true;
    }
    else
    {
        dataSaved = false;
        return false;
    }
}




//--------------------------------------------------------------------------
AttributesSortFilterProxyModel::AttributesSortFilterProxyModel(QObject *parent) :QSortFilterProxyModel(parent)
{
  /*  for (int i=0;i<3;i++)
    {
        visibleColumns.append(i);
    }
    visibleColumns.append(7);*/
}

bool AttributesSortFilterProxyModel::filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const
{
    return visibleColumns.contains(source_column);
}

QSet<int>* AttributesSortFilterProxyModel::getVisibleColumns()
{
    return &visibleColumns;
}

void AttributesSortFilterProxyModel::setVisibleColumns(QSettings *settings)
{
    settings->beginGroup("/MainSettings");
    int size = settings->beginReadArray("/VisibleColumns");
    for (int i =0; i<size; ++i)
    {
        settings->setArrayIndex(i);
        visibleColumns.insert(settings->value("/columnNb",i).toInt());
    }
    settings->endArray();
    settings->endGroup();
}
void AttributesSortFilterProxyModel::resetVisibleColumns()
{
    visibleColumns.clear();
    for (int i=0; i<sourceModel()->columnCount();++i)
    {
        visibleColumns.insert(i);
    }
    invalidate();
}
