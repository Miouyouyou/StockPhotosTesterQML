#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "myywebsocket.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    app.setOrganizationName("Myy & Compagnie");
    app.setOrganizationDomain("miouyouyou.fr");
    app.setApplicationName("Stock Photo Selector");
    qmlRegisterType<MyyWebSocket>("myy.helpers.websocket", 1, 0, "MyyWebSocketServer");
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
