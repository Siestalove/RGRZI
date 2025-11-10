#include "server.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QTextStream>
#include <QHostAddress>
#include <QDebug>

Server::Server(QObject* parent) : QTcpServer(parent), usersFile("users.json") {
    loadUsers();
    
    // Генерируем системные параметры, если их нет
    if (n.isEmpty()) {
        auto keys = FiatShamir::generateKeys("system");
        n = QString::fromStdString(keys.n);
    }
}

void Server::loadUsers() {
    QFile file(usersFile);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Файл пользователей не найден, будет создан новый";
        return;
    }
    
    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonArray usersArray = doc.array();
    
    for (const auto& value : usersArray) {
        QJsonObject userObj = value.toObject();
        FiatShamir::KeyPair keys;
        keys.login = userObj["login"].toString().toStdString();
        keys.public_key = userObj["public_key"].toString().toStdString();
        keys.n = userObj["n"].toString().toStdString();
        
        users[QString::fromStdString(keys.login)] = keys;
    }
    
    qDebug() << "Загружено" << users.size() << "пользователей";
}

void Server::saveUsers() {
    QJsonArray usersArray;
    
    for (auto it = users.begin(); it != users.end(); ++it) {
        QJsonObject userObj;
        userObj["login"] = QString::fromStdString(it.value().login);
        userObj["public_key"] = QString::fromStdString(it.value().public_key);
        userObj["n"] = QString::fromStdString(it.value().n);
        usersArray.append(userObj);
    }
    
    QJsonDocument doc(usersArray);
    QFile file(usersFile);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        qDebug() << "Пользователи сохранены";
    }
}

QString Server::generateSystemParameters() {
    return n;
}

bool Server::startServer(quint16 port) {
    if (!listen(QHostAddress::Any, port)) {
        qDebug() << "Ошибка запуска сервера:" << errorString();
        return false;
    }
    
    qDebug() << "Сервер запущен на порту" << port;
    qDebug() << "Системный параметр n:" << n;
    return true;
}

void Server::incomingConnection(qintptr socketDescriptor) {
    QTcpSocket* socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);
    
    connect(socket, &QTcpSocket::readyRead, this, &Server::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &Server::onDisconnected);
    
    qDebug() << "Новое подключение:" << socket->peerAddress().toString();
    
    // Отправляем системные параметры
    NetworkMessage handshake(NetworkMessage::HANDSHAKE, generateSystemParameters().toStdString());
    sendResponse(socket, handshake);
}

void Server::onReadyRead() {
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;
    
    QByteArray data = socket->readAll();
    QString dataStr(data);
    
    // Обрабатываем каждое сообщение
    QStringList messages = dataStr.split('\n', Qt::SkipEmptyParts);
    for (const QString& msgStr : messages) {
        NetworkMessage msg = NetworkMessage::deserialize(msgStr.toStdString());
        handleClientMessage(socket, msg);
    }
}

void Server::onDisconnected() {
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;
    
    qDebug() << "Клиент отключен:" << socket->peerAddress().toString();
    
    socketUsernames.remove(socket);
    pendingAuths.remove(socket);
    socket->deleteLater();
}

void Server::handleClientMessage(QTcpSocket* socket, const NetworkMessage& msg) {
    switch (msg.type) {
        case NetworkMessage::REGISTER_USER: {
            QString login = QString::fromStdString(msg.data);
            
            if (users.contains(login)) {
                NetworkMessage response(NetworkMessage::USER_EXISTS, "Пользователь уже существует");
                sendResponse(socket, response);
            } else {
                // Генерируем ключи для нового пользователя
                auto keys = FiatShamir::generateKeys(msg.data);
                keys.n = n.toStdString(); // Используем общий модуль
                
                users[login] = keys;
                saveUsers();
                
                NetworkMessage response(NetworkMessage::USER_REGISTERED, 
                    "Пользователь зарегистрирован. Ваш секретный ключ: " + keys.secret);
                sendResponse(socket, response);
                
                qDebug() << "Зарегистрирован новый пользователь:" << login;
            }
            break;
        }
        
        case NetworkMessage::AUTH_REQUEST: {
            QString login = QString::fromStdString(msg.data);
            
            if (!users.contains(login)) {
                NetworkMessage response(NetworkMessage::AUTH_FAILED, "Пользователь не найден");
                sendResponse(socket, response);
                return;
            }
            
            // Сохраняем данные аутентификации
            FiatShamir::AuthData auth;
            auth.login = msg.data;
            auth.x = msg.params[0];
            auth.e = FiatShamir::generateChallenge();
            
            pendingAuths[socket] = auth;
            socketUsernames[socket] = login;
            
            // Отправляем вызов
            NetworkMessage response(NetworkMessage::AUTH_CHALLENGE, to_string(auth.e));
            sendResponse(socket, response);
            
            qDebug() << "Запрос аутентификации от:" << login << "вызов:" << auth.e;
            break;
        }
        
        case NetworkMessage::AUTH_RESPONSE: {
            if (!pendingAuths.contains(socket)) {
                NetworkMessage response(NetworkMessage::AUTH_FAILED, "Нет ожидающей аутентификации");
                sendResponse(socket, response);
                return;
            }
            
            FiatShamir::AuthData auth = pendingAuths[socket];
            auth.y = msg.data;
            
            QString login = socketUsernames[socket];
            auto keys = users[login];
            
            if (FiatShamir::verifyAuthentication(auth, keys.public_key, keys.n)) {
                NetworkMessage response(NetworkMessage::AUTH_SUCCESS, "Аутентификация успешна");
                sendResponse(socket, response);
                qDebug() << "Аутентификация успешна для:" << login;
            } else {
                NetworkMessage response(NetworkMessage::AUTH_FAILED, "Аутентификация не удалась");
                sendResponse(socket, response);
                qDebug() << "Аутентификация не удалась для:" << login;
            }
            
            pendingAuths.remove(socket);
            break;
        }
        
        default:
            qDebug() << "Неизвестный тип сообщения:" << msg.type;
            break;
    }
}

void Server::sendResponse(QTcpSocket* socket, const NetworkMessage& response) {
    string data = response.serialize();
    socket->write(data.c_str(), data.length());
    socket->flush();
}