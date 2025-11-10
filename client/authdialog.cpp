#include "authdialog.h"

AuthDialog::AuthDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle("Аутентификация");
    setModal(true);
    setupUI();
}

void AuthDialog::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Логин
    QHBoxLayout* loginLayout = new QHBoxLayout();
    loginLayout->addWidget(new QLabel("Логин:", this));
    loginEdit = new QLineEdit(this);
    loginEdit->setPlaceholderText("Введите ваш логин");
    loginLayout->addWidget(loginEdit);
    
    // Секретный ключ
    QHBoxLayout* secretLayout = new QHBoxLayout();
    secretLayout->addWidget(new QLabel("Секретный ключ:", this));
    secretEdit = new QLineEdit(this);
    secretEdit->setPlaceholderText("Введите ваш секретный ключ");
    secretEdit->setEchoMode(QLineEdit::Password);
    secretLayout->addWidget(secretEdit);
    
    // Кнопки
    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    
    mainLayout->addLayout(loginLayout);
    mainLayout->addLayout(secretLayout);
    mainLayout->addWidget(buttonBox);
    
    resize(400, 150);
}

QString AuthDialog::getLogin() const {
    return loginEdit->text();
}

string AuthDialog::getSecret() const {
    return secretEdit->text().toStdString();
}

#include "authdialog.moc"