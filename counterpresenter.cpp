#include "counterpresenter.h"
#include "countermanager.h"

#include <QElapsedTimer>

CounterPresenter::CounterPresenter(const QString &title, QObject *parent)
    : QObject(parent)
    , m_title(title)
    , m_displayText(QStringLiteral("%1 henüz başlamadı.").arg(title))
{
}

CounterPresenter::~CounterPresenter()
{
    stop();
    if (m_thread) {
        m_thread->quit();
        m_thread->wait(2000);
        delete m_thread;
        m_thread = nullptr;
        m_manager = nullptr;
    }
}

void CounterPresenter::start()
{
    if (m_running)
        return;

    if (m_thread) {
        m_thread->quit();
        m_thread->wait(2000);
        delete m_thread;
        m_thread = nullptr;
        m_manager = nullptr;
    }

    m_stopping = false;
    m_count = 0;
    m_displayText = QStringLiteral("%1 başladı...\n").arg(m_title);
    emit countChanged();
    emit displayTextChanged();

    m_thread = new QThread(this);
    m_manager = new CounterManager();
    m_manager->moveToThread(m_thread);

    connect(m_thread, &QThread::started, m_manager, &CounterManager::run);
    connect(m_manager, &CounterManager::finished, m_thread, &QThread::quit);
    connect(m_manager, &CounterManager::finished, m_manager, &QObject::deleteLater);
    connect(m_thread, &QThread::finished, this, &CounterPresenter::onManagerFinished);

    // Cross-thread → QueuedConnection: onValueReady main thread'de çalışır
    connect(m_manager, &CounterManager::valueReady,
            this, &CounterPresenter::onValueReady);

    m_running = true;
    emit runningChanged();
    m_thread->start();
}

void CounterPresenter::stop()
{
    m_stopping = true;
    if (m_manager)
        m_manager->requestStop();
}

void CounterPresenter::onValueReady(int value)
{
    if (m_stopping.load())
        return;

    // Presenter main thread'de: for ile QML'e yazılacak metni üretir
    writeToUi(value);
}

void CounterPresenter::writeToUi(int value)
{
    // Kasıtlı ağır for: main thread'i ~200ms meşgul eder
    QString line;
    QElapsedTimer timer;
    timer.start();
    for (int i = 0; timer.elapsed() < 2000; ++i)
        line = QStringLiteral("[%1] sayaç = %2 (%3)\n")
                   .arg(m_title, QString::number(value), QString::number(i));

    m_count = value;
    m_displayText.prepend(line);
    if (m_displayText.size() > 3000)
        m_displayText.truncate(3000);

    emit countChanged();
    emit displayTextChanged();
}

void CounterPresenter::onManagerFinished()
{
    m_running = false;
    emit runningChanged();

    if (m_thread) {
        m_thread->deleteLater();
        m_thread = nullptr;
    }
    m_manager = nullptr;
}
