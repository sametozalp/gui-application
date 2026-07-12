#include "hostbridge.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    HostBridge host;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("host"), &host);

#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    engine.loadFromModule("GuiApplication", "Main");
#else
    const QUrl url(QStringLiteral("qrc:/qt/qml/GuiApplication/Main.qml"));
    engine.load(url);
#endif

    if (engine.rootObjects().isEmpty())
        return -1;

    return QGuiApplication::exec();
}
