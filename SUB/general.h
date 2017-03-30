#ifndef GENERAL_H
#define GENERAL_H
#include "SUB/threadsafevector.h"
#include <QVariant>

typedef QVector<QPair<QVariant,bool> > AttributesFromFile; // атрибуты одного файла
typedef ThreadSaveVector<AttributesFromFile> CountedAttributes; //все подсчитанные атрибуты


#endif // GENERAL_H
