#include "mainwindow.h"
#include "authdialog.h"
#include "registerdialog.h"
#include <QApplication>
#include <QMessageBox>
#include <QHostAddress>
#include <QDateTime>
#include <ctime>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    socket = new QTcpSocket(this);
    setupUI();
    
    connect(socket, &QTcpSocket::connected, this, &MainWindow::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &MainWindow::onDisconnected);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::onReadyRead);
    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
            this, &MainWindow::onSocketError);
}

MainWindow::~MainWindow() {
    if (socket->state() == QAbstractSocket::ConnectedState) {
        socket->disconnectFromHost();
    }
}

void MainWindow::setupUI() {
    setWindowTitle("Клиент Фиата-Шамира");
    setMinimumSize(600, 500);
    
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    mainLayout = new QVBoxLayout(centralWidget);
    
    // Группа подключения
    connectionGroup = new QGroupBox("Подключение к серверу", this);
    connectionLayout = new QHBoxLayout(connectionGroup);
    
    serverAddressEdit = new QLineEdit("127.0.0.1", this);
    serverAddressEdit->setPlaceholderText("Адрес сервера");
    
    portEdit = new QLineEdit("12345", this);
    portEdit->setPlaceholderText("Порт");
    portEdit->setMaximumWidth(80);
    
    connectButton = new QPushButton("Подключиться", this);
    connectButton->setMaximumWidth(120);
    
    connectionLayout->addWidget(new QLabel("Сервер:", this));
    connectionLayout->addWidget(serverAddressEdit);
    connectionLayout->addWidget(new QLabel("Порт:", this));
    connectionLayout->addWidget(portEdit);
    connectionLayout->addWidget(connectButton);
    connectionLayout->addStretch();
    
    // Группа аутентификации
    authGroup = new QGroupBox("Аутентификация", this);
    authLayout = new QVBoxLayout(authGroup);
    
    loginButton = new QPushButton("Войти в систему", this);
    registerButton = new QPushButton("Зарегистрироваться", this);
    
    authLayout->addWidget(loginButton);
    authLayout->addWidget(registerButton);
    
    // Лог
    logTextEdit = new QTextEdit(this);
    logTextEdit->setReadOnly(true);
    logTextEdit->setMaximumHeight(200);
    
    // Статус
    statusLabel = new QLabel("Статус: Не подключен", this);
    statusLabel->setStyleSheet("QLabel { color: red; font-weight: bold; }");
    
    // Добавляем все на главный layout
    mainLayout->addWidget(connectionGroup);
    mainLayout->addWidget(authGroup);
    mainLayout->addWidget(new QLabel("Лог событий:", this));
    mainLayout->addWidget(logTextEdit);
    mainLayout->addWidget(statusLabel);
    
    // Подключаем сигналы
    connect(connectButton, &QPushButton::clicked, this, &MainWindow::onConnectClicked);
    connect(loginButton, &QPushButton::clicked, this, &MainWindow::onLoginClicked);
    connect(registerButton, &QPushButton::clicked, this, &MainWindow::onRegisterClicked);
    
    // Изначально кнопки аутентификации неактивны
    loginButton->setEnabled(false);
    registerButton->setEnabled(false);
    
    addLogMessage("Приложение запущено. Выберите сервер и нажмите 'Подключиться'");
}

void MainWindow::onConnectClicked() {
    if (socket->state() == QAbstractSocket::ConnectedState) {
        disconnectFromServer();
    } else {
        connectToServer();
    }
}

void MainWindow::connectToServer() {
    QString address = serverAddressEdit->text();
    QString port = portEdit->text();
    
    if (address.isEmpty() || port.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите адрес и порт сервера");
        return;
    }
    
    addLogMessage(QString("Подключение к %1:%2...").arg(address).arg(port));
    socket->connectToHost(address, port.toUShort());
    
    connectButton->setText("Отключиться");
    connectButton->setEnabled(false);
}

void MainWindow::disconnectFromServer() {
    socket->disconnectFromHost();
    connectButton->setText("Подключиться");
    connectButton->setEnabled(true);
    loginButton->setEnabled(false);
    registerButton->setEnabled(false);
    statusLabel->setText("Статус: Не подключен");
    statusLabel->setStyleSheet("QLabel { color: red; font-weight: bold; }");
    addLogMessage("Отключено от сервера");
}

void MainWindow::onConnected() {
    connectButton->setEnabled(true);
    loginButton->setEnabled(true);
    registerButton->setEnabled(true);
    statusLabel->setText("Статус: Подключено");
    statusLabel->setStyleSheet("QLabel { color: green; font-weight: bold; }");
    addLogMessage("Успешное подключение к серверу");
}

void MainWindow::onDisconnected() {
    disconnectFromServer();
}

void MainWindow::onReadyRead() {
    QByteArray data = socket->readAll();
    QString dataStr(data);
    
    QStringList messages = dataStr.split('\n', Qt::SkipEmptyParts);
    for (const QString& msgStr : messages) {
        NetworkMessage msg = NetworkMessage::deserialize(msgStr.toStdString());
        handleServerMessage(msg);
    }
}

void MainWindow::onSocketError() {
    addLogMessage(QString("Ошибка сокета: %1").arg(socket->errorString()));
    connectButton->setEnabled(true);
    connectButton->setText("Подключиться");
    loginButton->setEnabled(false);
    registerButton->setEnabled(false);
}

void MainWindow::handleServerMessage(const NetworkMessage& msg) {
    switch (msg.type) {
        case NetworkMessage::HANDSHAKE: {
            systemN = QString::fromStdString(msg.data);
            addLogMessage("Получены системные параметры");
            addLogMessage(QString("Модуль n: %1...").arg(systemN.left(20)));
            break;
        }
        
        case NetworkMessage::USER_REGISTERED: {
            QMessageBox::information(this, "Регистрация", 
                QString::fromStdString("Пользователь успешно зарегистрирован!\n" + msg.data));
            addLogMessage("Пользователь успешно зарегистрирован");
            break;
        }
        
        case NetworkMessage::USER_EXISTS: {
            QMessageBox::warning(this, "Ошибка", 
                QString::fromStdString(msg.data));
            addLogMessage("Ошибка регистрации: пользователь уже существует");
            break;
        }
        
        case NetworkMessage::AUTH_CHALLENGE: {
            int e = stoi(msg.data);
            addLogMessage(QString("Получен вызов от сервера: e = %1").arg(e));
            
            // Вычисляем ответ
            string r_str = FiatShamir::stringToNumber("random_" + to_string(time(nullptr)));
            string r = FiatShamir::modExp(r_str, "1", systemN.toStdString());
            
            // y = r * s^e mod n
            string s_exp_e = FiatShamir::modExp(currentSecret, to_string(e), systemN.toStdString());
            string y = FiatShamir::modMul(r, s_exp_e, systemN.toStdString());
            
            // Отправляем ответ
            NetworkMessage response(NetworkMessage::AUTH_RESPONSE, y);
            string data = response.serialize();
            socket->write(data.c_str(), data.length());
            socket->flush();
            
            addLogMessage("Отправлен ответ на вызов сервера");
            break;
        }
        
        case NetworkMessage::AUTH_SUCCESS: {
            QMessageBox::information(this, "Успех", 
                QString::fromStdString(msg.data));
            addLogMessage("Аутентификация успешна!");
            break;
        }
        
        case NetworkMessage::AUTH_FAILED: {
            QMessageBox::critical(this, "Ошибка", 
                QString::fromStdString(msg.data));
            addLogMessage("Аутентификация не удалась!");
            break;
        }
        
        default:
            addLogMessage(QString("Получено неизвестное сообщение: %1").arg(msg.type));
            break;
    }
}

void MainWindow::onLoginClicked() {
    AuthDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        currentLogin = dialog.getLogin();
        currentSecret = dialog.getSecret();
        startAuthentication(currentLogin, currentSecret);
    }
}

void MainWindow::onRegisterClicked() {
    RegisterDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QString login = dialog.getLogin();
        
        NetworkMessage msg(NetworkMessage::REGISTER_USER, login.toStdString());
        string data = msg.serialize();
        socket->write(data.c_str(), data.length());
        socket->flush();
        
        addLogMessage(QString("Отправлен запрос на регистрацию пользователя: %1").arg(login));
    }
}

void MainWindow::startAuthentication(const QString& login, const string& secret) {
    // Генерируем случайное r
    string r_str = FiatShamir::stringToNumber("auth_" + to_string(time(nullptr)));
    string r = FiatShamir::modExp(r_str, "1", systemN.toStdString());
    
    // Вычисляем x = r^2 mod n
    string x = FiatShamir::modExp(r, "2", systemN.toStdString());
    
    // Отправляем запрос аутентификации
    NetworkMessage msg(NetworkMessage::AUTH_REQUEST, login.toStdString());
    msg.params.push_back(x);
    
    string data = msg.serialize();
    socket->write(data.c_str(), data.length());
    socket->flush();
    
    addLogMessage(QString("Отправлен запрос аутентификации для пользователя: %1").arg(login));
}

void MainWindow::addLogMessage(const QString& message) {
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    logTextEdit->append(QString("[%1] %2").arg(timestamp).arg(message));
}

#include "mainwindow.moc"