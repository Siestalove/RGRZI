#include "registerdialog.h"

RegisterDialog::RegisterDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle("Регистрация");
    setModal(true);
    setupUI();
}

void RegisterDialog::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Информационная метка
    QLabel* infoLabel = new QLabel(
        "Введите логин для регистрации.\n"
        "Сервер сгенерирует для вас секретный ключ,\n"
        "который будет показан после регистрации.", this);
    infoLabel->setWordWrap(true);
    infoLabel->setStyleSheet("QLabel { color: gray; margin: 10px; }");
    
    // Логин
    QHBoxLayout* loginLayout = new QHBoxLayout();
    loginLayout->addWidget(new QLabel("Логин:", this));
    loginEdit = new QLineEdit(this);
    loginEdit->setPlaceholderText("Придумайте логин");
    loginLayout->addWidget(loginEdit);
    
    // Кнопки
    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    
    mainLayout->addWidget(infoLabel);
    mainLayout->addLayout(loginLayout);
    mainLayout->addWidget(buttonBox);
    
    resize(400, 180);
}

QString RegisterDialog::getLogin() const {
    return loginEdit->text();
}

#include "registerdialog.moc"