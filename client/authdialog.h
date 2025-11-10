#pragma once
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QDialogButtonBox>

using namespace std;

class AuthDialog : public QDialog {
    Q_OBJECT

private:
    QLineEdit* loginEdit;
    QLineEdit* secretEdit;
    
    void setupUI();

public:
    AuthDialog(QWidget* parent = nullptr);
    QString getLogin() const;
    string getSecret() const;
};