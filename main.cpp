#include "counterpresenter.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    CounterPresenter presenter1(QStringLiteral("Presenter 1"));
    CounterPresenter presenter2(QStringLiteral("Presenter 2"));

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("presenter1"), &presenter1);
    engine.rootContext()->setContextProperty(QStringLiteral("presenter2"), &presenter2);

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
