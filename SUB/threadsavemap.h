#ifndef THREADSAVEMAP_H
#define THREADSAVEMAP_H

#include <QReadWriteLock>
#include <QReadLocker>
#include <QWriteLocker>
#include <QMap>

template <typename T1,typename T2>
class ThreadSaveMap
{
public:
//    typedef ThreadSaveMapIterator<T1,T2> iterator;

    explicit ThreadSaveMap() {}
    int count() const
    {
        QReadLocker locker(&lock);
        return map.count();
    }
    bool isEmpty() const
    {
        QReadLocker locker(&lock);
        return map.isEmpty();
    }
    void clear()
    {
        QWriteLocker locker(&lock);
        map.clear();
    }
    void insert(const T1 &key,const T2 &value)
    {
        QWriteLocker locker(&lock);
        map.insert(key,value);
    }

    T2 value(const T1 &key) const
    {
        QReadLocker locker(&lock);
        return map.value(key);
    }
    QList<T2> values() const
    {
        QReadLocker locker(&lock);
        return map.values();
    }
//    iterator begin();
//    iterator end();

private:
    mutable QReadWriteLock lock;
    QMap<T1,T2> map;
};


#endif // THREADSAVEMAP_H
