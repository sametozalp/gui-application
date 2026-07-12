#ifndef COUNTERPRESENTER_H
#define COUNTERPRESENTER_H

#include <QObject>
#include <QString>
#include <QThread>
#include <atomic>

class CounterManager;

// Sunum katmanı — main (GUI) thread'de kalır; Manager'dan gelen veriyi QML'e yazar
class CounterPresenter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString displayText READ displayText NOTIFY displayTextChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(bool running READ running NOTIFY runningChanged)
    Q_PROPERTY(QString title READ title CONSTANT)

public:
    explicit CounterPresenter(const QString &title, QObject *parent = nullptr);
    ~CounterPresenter() override;

    QString displayText() const { return m_displayText; }
    int count() const { return m_count; }
    bool running() const { return m_running; }
    QString title() const { return m_title; }

    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();

signals:
    void displayTextChanged();
    void countChanged();
    void runningChanged();

private slots:
    void onValueReady(int value);
    void onManagerFinished();

private:
    void writeToUi(int value);

    QString m_title;
    QString m_displayText;
    int m_count = 0;
    bool m_running = false;
    std::atomic<bool> m_stopping{false};

    QThread *m_thread = nullptr;
    CounterManager *m_manager = nullptr;
};

#endif // COUNTERPRESENTER_H
