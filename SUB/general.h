#ifndef GENERAL_H
#define GENERAL_H
#include "SUB/threadsafevector.h"
#include <QVariant>

typedef QVector<QPair<QVariant,bool> > AttributesFromFile; // атрибуты одного файла
typedef ThreadSaveVector<AttributesFromFile> CountedAttributes; //все подсчитанные атрибуты
typedef QPair<float,float> AttributesRange;

#endif // GENERAL_H
