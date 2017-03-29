#ifndef GENERAL_H
#define GENERAL_H
#include "SUB/threadsafevector.h"

typedef QVector<QPair<QVariant,bool> AttributesFromFile;
typedef ThreadSaveVector<AttributesFromFile> CountedAttributes;


#endif // GENERAL_H
