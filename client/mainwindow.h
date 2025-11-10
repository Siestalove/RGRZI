#pragma once
#include <QMainWindow>
#include <QTcpSocket>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QGroupBox>
#include <memory>
#include "../common/fiat_shamir.h"
#include "../common/network_message.h"

using namespace std;

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    QTcpSocket* socket;
    QString systemN;
    QString currentLogin;
    string currentSecret;
    
    // UI элементы
    QWidget* centralWidget;
    QVBoxLayout* mainLayout;
    
    QGroupBox* connectionGroup;
    QHBoxLayout* connectionLayout;
    QLineEdit* serverAddressEdit;
    QLineEdit* portEdit;
    QPushButton* connectButton;
    QLabel* statusLabel;
    
    QGroupBox* authGroup;
    QVBoxLayout* authLayout;
    QPushButton* loginButton;
    QPushButton* registerButton;
    
    QTextEdit* logTextEdit;
    
    void setupUI();
    void connectToServer();
    void disconnectFromServer();
    void addLogMessage(const QString& message);
    void handleServerMessage(const NetworkMessage& msg);
    void startAuthentication(const QString& login, const string& secret);

private slots:
    void onConnectClicked();
    void onLoginClicked();
    void onRegisterClicked();
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onSocketError();

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
};