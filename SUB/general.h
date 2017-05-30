#ifndef GENERAL_H
#define GENERAL_H
#include "SUB/threadsafevector.h"
#include "SUB/threadsavemap.h"
#include "SUB/threadsavehash.h"
#include <QVariant>
#include <SUB/point.h>
typedef QVector<QPair<QVariant,bool> > AttributesFromFile; // атрибуты одного файла
typedef ThreadSaveVector<AttributesFromFile> CountedAttributes; //все подсчитанные атрибуты
typedef QPair<float,float> AttributesRange;

typedef QPair<quint32,quint32> PointId;

//typedef ThreadSaveMap<PointId,TestPoint> TestMap;
//typedef ThreadSaveHash<PointId,TestPoint> TestMap;
//typedef ThreadSaveHash<uint,TestPoint> TestMap;
typedef ThreadSaveMap<uint,TestPoint> TestMap;
typedef ThreadSaveMap<uint,XFile> XFileMap;




#endif // GENERAL_H
