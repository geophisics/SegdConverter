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
float AttributesModel::getMaxValueInColumn(const int &column)
{
    float max = attributes.value(0).value(column).first.toFloat();
    for (int i=1; i<attributes.count();++i)
    {
        if (max<attributes.value(i).value(column).first.toFloat())
        {
            max = attributes.value(i).value(column).first.toFloat();
        }
    }
    return max;
}
float AttributesModel::getMinValueInColumn(const int &column)
{
    float min = attributes.value(0).value(column).first.toFloat();
    for (int i=1; i<attributes.count();++i)
    {
        if (min > attributes.value(i).value(column).first.toFloat())
        {
            min = attributes.value(i).value(column).first.toFloat();
        }
    }
    return min;
}

int AttributesModel::getFirstColumnValue(QModelIndex &index)
{
    return attributes.value(index.row()).at(0).first.toInt();
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

void AttributesSortFilterProxyModel::setFirstFilterColumn(const int &i)
{
    firstFilterColumn =i-1;
}

void AttributesSortFilterProxyModel::setSecondFilterColumn(const int &i)
{
    secondFilterColumn =i-1;
}
void AttributesSortFilterProxyModel::setThirdFilterColumn(const int &i)
{
    thirdFilterColumn =i-1;
}

void AttributesSortFilterProxyModel::setFirstRange(const float &min, const float &max)
{
    firstRange = qMakePair(min,max);
    invalidateFilter();
}

void AttributesSortFilterProxyModel::setSecondRange(const float &min, const float &max)
{
    secondRange = qMakePair(min,max);
    invalidateFilter();
}
void AttributesSortFilterProxyModel::setThirdRange(const float &min, const float &max)
{
    thirdRange = qMakePair(min,max);
    invalidateFilter();
}



//--------------------------------------------------------------------------
AttributesSortFilterProxyModel::AttributesSortFilterProxyModel(QObject *parent) :QSortFilterProxyModel(parent)
{
    firstFilterColumn = -1;
    secondFilterColumn = -1;
    thirdFilterColumn = -1;
}

bool AttributesSortFilterProxyModel::filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const
{
    Q_UNUSED(source_parent);
    return visibleColumns.contains(source_column);
}
bool AttributesSortFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    bool firstCondition=true;
    bool secondCondition=true;
    bool thirdCondition=true;
    QModelIndex index;
   // qDebug()<<"First Filter"<<"Column "<<firstFilterColumn<<"Min "<<firstRange.first<<"Max "<<firstRange.second;
   // qDebug()<<"Second Filter"<<"Column "<<secondFilterColumn<<"Min "<<secondRange.first<<"Max "<<secondRange.second;
   // qDebug()<<"Third Filter"<<"Column "<<thirdFilterColumn<<"Min "<<thirdRange.first<<"Max "<<thirdRange.second;
    if (firstFilterColumn>=0)
    {

        index = sourceModel()->index(source_row, firstFilterColumn, source_parent);
        firstCondition = dataInRange(sourceModel()->data(index).toFloat(),firstRange);
    }
    if (secondFilterColumn>=0)
    {

        index = sourceModel()->index(source_row, secondFilterColumn, source_parent);
        secondCondition = dataInRange(sourceModel()->data(index).toFloat(),secondRange);
    }
    if (thirdFilterColumn>=0)
    {

        index = sourceModel()->index(source_row, thirdFilterColumn, source_parent);
        thirdCondition = dataInRange(sourceModel()->data(index).toFloat(),thirdRange);
    }
    return (firstCondition && secondCondition && thirdCondition);
}

bool AttributesSortFilterProxyModel::dataInRange(const float &f,const AttributesRange &range) const
{
    return (range.first-f <EPS && f-range.second < EPS);
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

AttributesRange AttributesSortFilterProxyModel::getFirstFilterRange()
{
    return firstRange;
}

AttributesRange AttributesSortFilterProxyModel::getSecondFilterRange()
{
    return secondRange;
}

AttributesRange AttributesSortFilterProxyModel::getThirdFilterRange()
{
    return thirdRange;
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
