#ifndef SINGLETONCOUNTER_H
#define SINGLETONCOUNTER_H

#include <QObject>
#include <QMutex>

class SingletonCounter : public QObject
{
    Q_OBJECT
public:
    // Get the singleton instance
    static SingletonCounter& getInstance();

    // Delete copy constructor and assignment operator
    SingletonCounter(const SingletonCounter&) = delete;
    SingletonCounter& operator=(const SingletonCounter&) = delete;

    // Increment the counter by 0.1
    double increment();

    // Get the current counter value
    double getValue() const;

    // Set the counter to a specific value
    void setValue(double newValue);

signals:
    // Signal emitted when value changes
    void valueChanged(double newValue);

private:
    // Private constructor
    explicit SingletonCounter(QObject* parent = nullptr);

    static QMutex mutex;          // Mutex for thread safety
    double counterValue = 0.0;     // The counter value
};

#endif // SINGLETONCOUNTER_H