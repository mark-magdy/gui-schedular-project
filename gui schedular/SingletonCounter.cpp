#include "SingletonCounter.h"
#include <QMutexLocker>

// Initialize static mutex
QMutex SingletonCounter::mutex;

SingletonCounter::SingletonCounter(QObject* parent)
    : QObject(parent), counterValue(0.0)
{
}

SingletonCounter& SingletonCounter::getInstance()
{
    QMutexLocker locker(&mutex);
    static SingletonCounter instance;
    return instance;
}

double SingletonCounter::increment()
{
    QMutexLocker locker(&mutex);
    counterValue +=1;
	return counterValue;
    //emit valueChanged(counterValue);
}

double SingletonCounter::getValue() const
{
    QMutexLocker locker(&mutex);
    return counterValue;
}

void SingletonCounter::setValue(double newValue)
{
    QMutexLocker locker(&mutex);
    if (counterValue != newValue) {
        counterValue = newValue;
        emit valueChanged(counterValue);
    }
}