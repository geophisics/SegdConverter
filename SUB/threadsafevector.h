#ifndef THREADSAFEVECTOR_H
#define THREADSAFEVECTOR_H

#include <QReadWriteLock>
#include <QReadLocker>
#include <QWriteLocker>
#include <QVector>

template <typename T>

class ThreadSaveVector
{
public:
    explicit ThreadSaveVector() {}
    int count() const
    {
        QReadLocker locker(&lock);
        return vec.count();
    }

    bool isEmpty() const
    {
        QReadLocker locker(&lock);
        return vec.isEmpty();
    }

    void clear()
    {
        QWriteLocker(&lock);
        vec.clear();
    }

    void append(const T &value)
    {
        QWriteLocker(&lock);
        vec.append(value);
    }

    T value(const int &i) const
    {
        QReadLocker(&lock);
        return vec.value(i);
    }
private:
    mutable QReadWriteLock lock;
    QVector<T> vec;
};


#endif // THREADSAFEVECTOR_H
