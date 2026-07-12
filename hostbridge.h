#ifndef HOSTBRIDGE_H
#define HOSTBRIDGE_H

#include <QByteArray>
#include <QHash>
#include <QObject>
#include <QProcess>
#include <QString>

class QLocalServer;
class QLocalSocket;

// Host UI process: launches sub-app processes and mirrors IPC results into QML
class HostBridge : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int count1 READ count1 NOTIFY channel1Changed)
    Q_PROPERTY(int count2 READ count2 NOTIFY channel2Changed)
    Q_PROPERTY(QString text1 READ text1 NOTIFY channel1Changed)
    Q_PROPERTY(QString text2 READ text2 NOTIFY channel2Changed)
    Q_PROPERTY(bool running1 READ running1 NOTIFY running1Changed)
    Q_PROPERTY(bool running2 READ running2 NOTIFY running2Changed)
    Q_PROPERTY(QString serverName READ serverName CONSTANT)

public:
    explicit HostBridge(QObject *parent = nullptr);
    ~HostBridge() override;

    int count1() const { return m_count1; }
    int count2() const { return m_count2; }
    QString text1() const { return m_text1; }
    QString text2() const { return m_text2; }
    bool running1() const { return m_running1; }
    bool running2() const { return m_running2; }
    QString serverName() const { return m_serverName; }

    Q_INVOKABLE void startWorker1();
    Q_INVOKABLE void stopWorker1();
    Q_INVOKABLE void startWorker2();
    Q_INVOKABLE void stopWorker2();

signals:
    void channel1Changed();
    void channel2Changed();
    void running1Changed();
    void running2Changed();

private:
    void startWorker(int id);
    void stopWorker(int id);
    void onNewConnection();
    void onSocketReadyRead();
    void onSocketDisconnected();
    void handleLine(const QString &line);
    void onProcessFinished(int id, int exitCode, QProcess::ExitStatus status);

    QString subAppExecutable(int id) const;

    QLocalServer *m_server = nullptr;
    QString m_serverName;

    QProcess *m_processes[2] = {nullptr, nullptr};
    QLocalSocket *m_sockets[2] = {nullptr, nullptr};
    QByteArray m_buffers[2];
    QHash<QLocalSocket *, int> m_socketIds;
    QHash<QLocalSocket *, QByteArray> m_pendingRx;

    int m_count1 = 0;
    int m_count2 = 0;
    QString m_text1;
    QString m_text2;
    bool m_running1 = false;
    bool m_running2 = false;
};

#endif // HOSTBRIDGE_H
