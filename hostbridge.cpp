#include "hostbridge.h"

#include <QCoreApplication>
#include <QLocalServer>
#include <QLocalSocket>

HostBridge::HostBridge(QObject *parent)
    : QObject(parent)
    , m_text1(QStringLiteral("subapp-a not started yet."))
    , m_text2(QStringLiteral("subapp-b not started yet."))
{
    m_serverName = QStringLiteral("gui-counter-host-%1")
                       .arg(QCoreApplication::applicationPid());

    m_server = new QLocalServer(this);
    QLocalServer::removeServer(m_serverName);
    if (!m_server->listen(m_serverName)) {
        m_text1 = QStringLiteral("IPC sunucu hatası: %1").arg(m_server->errorString());
        m_text2 = m_text1;
        emit channel1Changed();
        emit channel2Changed();
        return;
    }

    connect(m_server, &QLocalServer::newConnection, this, &HostBridge::onNewConnection);
}

HostBridge::~HostBridge()
{
    stopWorker1();
    stopWorker2();
    if (m_server) {
        m_server->close();
        QLocalServer::removeServer(m_serverName);
    }
}

QString HostBridge::subAppExecutable(int id) const
{
    const QString name = (id == 1)
                             ? QStringLiteral("/subapp-a")
                             : QStringLiteral("/subapp-b");
    QString path = QCoreApplication::applicationDirPath() + name;
#ifdef Q_OS_WIN
    path += QStringLiteral(".exe");
#endif
    return path;
}

void HostBridge::startWorker1()
{
    startWorker(1);
}

void HostBridge::stopWorker1()
{
    stopWorker(1);
}

void HostBridge::startWorker2()
{
    startWorker(2);
}

void HostBridge::stopWorker2()
{
    stopWorker(2);
}

void HostBridge::startWorker(int id)
{
    const int index = id - 1;
    if (index < 0 || index > 1)
        return;
    if ((id == 1 && m_running1) || (id == 2 && m_running2))
        return;

    stopWorker(id);

    auto *process = new QProcess(this);
    m_processes[index] = process;

    connect(process, &QProcess::finished, this,
            [this, id](int exitCode, QProcess::ExitStatus status) {
                onProcessFinished(id, exitCode, status);
            });

    const QString program = subAppExecutable(id);
    const QStringList args{QStringLiteral("--server"), m_serverName};

    if (id == 1) {
        m_count1 = 0;
        m_text1 = QStringLiteral("subapp-a starting...\n");
        emit channel1Changed();
        m_running1 = true;
        emit running1Changed();
    } else {
        m_count2 = 0;
        m_text2 = QStringLiteral("subapp-b starting...\n");
        emit channel2Changed();
        m_running2 = true;
        emit running2Changed();
    }

    process->start(program, args);
    if (!process->waitForStarted(3000)) {
        const QString err = QStringLiteral("%1 failed to start:\n%2\n%3")
                                .arg(id == 1 ? QStringLiteral("subapp-a")
                                             : QStringLiteral("subapp-b"))
                                .arg(program)
                                .arg(process->errorString());
        if (id == 1) {
            m_text1 = err;
            emit channel1Changed();
            m_running1 = false;
            emit running1Changed();
        } else {
            m_text2 = err;
            emit channel2Changed();
            m_running2 = false;
            emit running2Changed();
        }
        process->deleteLater();
        m_processes[index] = nullptr;
    }
}

void HostBridge::stopWorker(int id)
{
    const int index = id - 1;
    if (index < 0 || index > 1)
        return;

    if (m_sockets[index]) {
        m_sockets[index]->write("STOP\n");
        m_sockets[index]->flush();
        m_sockets[index]->disconnectFromServer();
        m_sockets[index]->deleteLater();
        m_sockets[index] = nullptr;
    }
    m_buffers[index].clear();

    if (m_processes[index]) {
        QProcess *process = m_processes[index];
        m_processes[index] = nullptr;
        process->disconnect(this);
        if (process->state() != QProcess::NotRunning) {
            process->terminate();
            if (!process->waitForFinished(1500))
                process->kill();
        }
        process->deleteLater();
    }

    if (id == 1 && m_running1) {
        m_running1 = false;
        emit running1Changed();
    } else if (id == 2 && m_running2) {
        m_running2 = false;
        emit running2Changed();
    }
}

void HostBridge::onNewConnection()
{
    while (m_server->hasPendingConnections()) {
        QLocalSocket *socket = m_server->nextPendingConnection();
        m_socketIds.insert(socket, 0);
        m_pendingRx.insert(socket, QByteArray());
        connect(socket, &QLocalSocket::readyRead, this, &HostBridge::onSocketReadyRead);
        connect(socket, &QLocalSocket::disconnected, this, &HostBridge::onSocketDisconnected);
    }
}

void HostBridge::onSocketReadyRead()
{
    auto *socket = qobject_cast<QLocalSocket *>(sender());
    if (!socket)
        return;

    int id = m_socketIds.value(socket, 0);
    m_pendingRx[socket].append(socket->readAll());
    QByteArray &buffer = m_pendingRx[socket];

    while (true) {
        const int nl = buffer.indexOf('\n');
        if (nl < 0)
            break;

        const QByteArray raw = buffer.left(nl).trimmed();
        buffer.remove(0, nl + 1);
        if (raw.isEmpty())
            continue;

        const QString line = QString::fromUtf8(raw);

        if (id == 0 && line.startsWith(QLatin1String("HELLO|"))) {
            const int workerId = line.section('|', 1, 1).toInt();
            if (workerId == 1 || workerId == 2) {
                id = workerId;
                m_socketIds[socket] = workerId;
                if (m_sockets[workerId - 1] && m_sockets[workerId - 1] != socket)
                    m_sockets[workerId - 1]->deleteLater();
                m_sockets[workerId - 1] = socket;
            }
            continue;
        }

        handleLine(line);
    }
}

void HostBridge::onSocketDisconnected()
{
    auto *socket = qobject_cast<QLocalSocket *>(sender());
    if (!socket)
        return;

    const int id = m_socketIds.value(socket, 0);
    m_socketIds.remove(socket);
    m_pendingRx.remove(socket);

    if (id == 1 || id == 2) {
        if (m_sockets[id - 1] == socket)
            m_sockets[id - 1] = nullptr;
        m_buffers[id - 1].clear();
    }
    socket->deleteLater();
}

void HostBridge::handleLine(const QString &line)
{
    // RESULT|<id>|<count>|<text>
    if (!line.startsWith(QLatin1String("RESULT|")))
        return;

    const QStringList parts = line.split('|');
    if (parts.size() < 4)
        return;

    const int id = parts.at(1).toInt();
    const int count = parts.at(2).toInt();
    const QString text = parts.mid(3).join('|');

    if (id == 1) {
        m_count1 = count;
        m_text1 = text + QLatin1Char('\n') + m_text1;
        if (m_text1.size() > 4000)
            m_text1.truncate(4000);
        emit channel1Changed();
    } else if (id == 2) {
        m_count2 = count;
        m_text2 = text + QLatin1Char('\n') + m_text2;
        if (m_text2.size() > 4000)
            m_text2.truncate(4000);
        emit channel2Changed();
    }
}

void HostBridge::onProcessFinished(int id, int exitCode, QProcess::ExitStatus status)
{
    Q_UNUSED(exitCode);
    Q_UNUSED(status);

    const int index = id - 1;
    if (index < 0 || index > 1)
        return;

    if (m_processes[index]) {
        m_processes[index]->deleteLater();
        m_processes[index] = nullptr;
    }
    if (m_sockets[index]) {
        m_sockets[index]->deleteLater();
        m_sockets[index] = nullptr;
    }
    m_buffers[index].clear();

    if (id == 1) {
        m_running1 = false;
        emit running1Changed();
        m_text1 = QStringLiteral("subapp-a stopped.\n") + m_text1;
        emit channel1Changed();
    } else if (id == 2) {
        m_running2 = false;
        emit running2Changed();
        m_text2 = QStringLiteral("subapp-b stopped.\n") + m_text2;
        emit channel2Changed();
    }
}
