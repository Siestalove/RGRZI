#pragma once
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QString>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <memory>
#include "../common/fiat_shamir.h"
#include "../common/network_message.h"

using namespace std;

class Server : public QTcpServer {
    Q_OBJECT

private:
    QMap<QString, FiatShamir::KeyPair> users;
    QMap<QTcpSocket*, QString> socketUsernames;
    QMap<QTcpSocket*, FiatShamir::AuthData> pendingAuths;
    QString usersFile;
    QString n; // Общий модуль системы
    
    void loadUsers();
    void saveUsers();
    void handleClientMessage(QTcpSocket* socket, const NetworkMessage& msg);
    void sendResponse(QTcpSocket* socket, const NetworkMessage& response);
    QString generateSystemParameters();

protected:
    void incomingConnection(qintptr socketDescriptor) override;

public slots:
    void onReadyRead();
    void onDisconnected();

public:
    Server(QObject* parent = nullptr);
    bool startServer(quint16 port = 12345);
};