#include <QCommandLineParser>
#include <QCoreApplication>
#include <QElapsedTimer>
#include <QLocalSocket>
#include <QTimer>

// Sub-app A — own process / main
static constexpr int kAppId = 1;
static constexpr const char *kAppName = "subapp-a";

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(QString::fromUtf8(kAppName));

    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Sub-app A"));
    parser.addHelpOption();
    QCommandLineOption serverOption(QStringList{QStringLiteral("server")},
                                    QStringLiteral("Host QLocalServer name"),
                                    QStringLiteral("name"));
    parser.addOption(serverOption);
    parser.process(app);

    const QString serverName = parser.value(serverOption);
    if (serverName.isEmpty())
        return 1;

    QLocalSocket socket;
    socket.connectToServer(serverName);
    if (!socket.waitForConnected(3000))
        return 2;

    socket.write(QByteArray("HELLO|") + QByteArray::number(kAppId) + '\n');
    socket.flush();

    bool running = true;
    QObject::connect(&socket, &QLocalSocket::readyRead, &app, [&]() {
        if (socket.readAll().contains("STOP"))
            running = false;
    });
    QObject::connect(&socket, &QLocalSocket::disconnected, &app, [&]() {
        running = false;
        QCoreApplication::quit();
    });

    int value = 1;
    QTimer tick;
    tick.setInterval(1000);
    QObject::connect(&tick, &QTimer::timeout, &app, [&]() {
        if (!running) {
            tick.stop();
            QCoreApplication::quit();
            return;
        }

        QString prepared;
        QElapsedTimer burn;
        burn.start();
        for (int i = 0; burn.elapsed() < 200; ++i) {
            prepared = QStringLiteral("%1 count=%2 (step #%3)")
                           .arg(QString::fromUtf8(kAppName))
                           .arg(value)
                           .arg(i);
        }

        const QByteArray line =
            QByteArray("RESULT|")
            + QByteArray::number(kAppId) + '|'
            + QByteArray::number(value) + '|'
            + prepared.toUtf8() + '\n';

        if (socket.state() != QLocalSocket::ConnectedState) {
            running = false;
            tick.stop();
            QCoreApplication::quit();
            return;
        }

        socket.write(line);
        socket.flush();
        ++value;
    });
    tick.start();

    return QCoreApplication::exec();
}
