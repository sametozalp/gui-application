#include "countermanager.h"

#include <QThread>

CounterManager::CounterManager(QObject *parent)
    : QObject(parent)
{
}

void CounterManager::run()
{
    m_stopRequested = false;
    int value = 1;

    while (!m_stopRequested.load()) {
        emit valueReady(value++);
        QThread::msleep(1000);
    }

    emit finished();
}

void CounterManager::requestStop()
{
    m_stopRequested = true;
}
