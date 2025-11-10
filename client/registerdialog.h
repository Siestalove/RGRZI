#pragma once
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QDialogButtonBox>

using namespace std;

class RegisterDialog : public QDialog {
    Q_OBJECT

private:
    QLineEdit* loginEdit;
    
    void setupUI();

public:
    RegisterDialog(QWidget* parent = nullptr);
    QString getLogin() const;
};