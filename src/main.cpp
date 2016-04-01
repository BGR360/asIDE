#include <QApplication>
#include <QQmlApplicationEngine>

#include "documenthandler.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;

    qmlRegisterType<DocumentHandler>("com.bgreeves.aside", 1, 0, "DocumentHandler");

    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    return app.exec();
}
