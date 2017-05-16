#ifndef THREADSAVEHASH_H
#define THREADSAVEHASH_H

#include <QReadWriteLock>
#include <QReadLocker>
#include <QWriteLocker>
#include <QHash>

template <typename T1,typename T2>

class ThreadSaveHash
{
public:
    explicit ThreadSaveHash() {}
    int count() const
    {
        QReadLocker locker(&lock);
        return hash.count();
    }
    bool isEmpty() const
    {
        QReadLocker locker(&lock);
        return hash.isEmpty();
    }
    void clear()
    {
        QWriteLocker locker(&lock);
        hash.clear();
    }
    void insert(const T1 &key,const T2 &value)
    {
        QWriteLocker locker(&lock);
        hash.insert(key,value);
    }

    T2 value(const T1 &key) const
    {
        QReadLocker locker(&lock);
        return hash.value(key);
    }
    QList<T2> values() const
    {
        QReadLocker locker(&lock);
        return hash.values();
    }

private:
    mutable QReadWriteLock lock;
    QHash<T1,T2> hash;
};


#endif // THREADSAVEHASH_H
