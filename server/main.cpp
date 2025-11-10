#include <QCoreApplication>
#include "server.h"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    
    Server server;
    if (!server.startServer(12345)) {
        return 1;
    }
    
    qDebug() << "Сервер Фиата-Шамира запущен...";
    qDebug() << "Нажмите Ctrl+C для остановки";
    
    return app.exec();
}