#ifndef COUNTERMANAGER_H
#define COUNTERMANAGER_H

#include <QObject>
#include <atomic>

// Mantık katmanı — kendi QThread'inde çalışır
class CounterManager : public QObject
{
    Q_OBJECT

public:
    explicit CounterManager(QObject *parent = nullptr);

public slots:
    void run();
    void requestStop();

signals:
    void valueReady(int value);
    void finished();

private:
    std::atomic<bool> m_stopRequested{false};
};

#endif // COUNTERMANAGER_H
