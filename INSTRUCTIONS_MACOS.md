# 🍎 Инструкция по сборке на macOS с Homebrew Qt

## Ваша ситуация
У вас установлен Qt 6.9.3 через Homebrew в `/opt/homebrew/Cellar/qt/6.9.3/`

## 🚀 Быстрый старт

### 1. Сборка на вашем MacBook

Скопируйте проект на ваш MacBook и выполните:

```bash
# Клонируйте или скопируйте проект
cd fiat-shamir-project

# Установка переменных окружения
export PATH="/opt/homebrew/Cellar/qt/6.9.3/bin:$PATH"
export LDFLAGS="-L/opt/homebrew/opt/openssl/lib"
export CPPFLAGS="-I/opt/homebrew/opt/openssl/include"
export PKG_CONFIG_PATH="/opt/homebrew/opt/openssl/lib/pkgconfig"

# Сборка сервера
cd server
/opt/homebrew/Cellar/qt/6.9.3/bin/qmake server.pro
make

# Сборка клиента  
cd ../client
/opt/homebrew/Cellar/qt/6.9.3/bin/qmake client.pro
make

cd ..
```

### 2. Запуск

```bash
# Запуск сервера (в первом терминале)
./server/fiat_shamir_server

# Запуск клиента (во втором терминале)
./client/fiat_shamir_client
```

## 🔧 Альтернативный способ (через CMake)

```bash
# Установка путей
export CMAKE_PREFIX_PATH="/opt/homebrew/Cellar/qt/6.9.3"
export Qt6_ROOT="/opt/homebrew/Cellar/qt/6.9.3"

# Сборка
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH="/opt/homebrew/Cellar/qt/6.9.3"
make
```

## 🛠️ Если возникнут проблемы

### Проблема: "Qt6 not found"

Решение: Укажите путь явно:
```bash
export Qt6_ROOT="/opt/homebrew/Cellar/qt/6.9.3"
```

### Проблема: "OpenSSL not found"

Установите OpenSSL:
```bash
brew install openssl
export OPENSSL_ROOT_DIR="/opt/homebrew/opt/openssl"
```

### Проблема: "QTcpServer: No such file or directory"

Убедитесь, что используете правильный qmake:
```bash
/opt/homebrew/Cellar/qt/6.9.3/bin/qmake -version
```

## 📱 Проверка установки

Проверьте, что Qt установлен правильно:
```bash
/opt/homebrew/Cellar/qt/6.9.3/bin/qmake -query QT_INSTALL_HEADERS
/opt/homebrew/Cellar/qt/6.9.3/bin/qmake -query QT_INSTALL_LIBS
```

## 🎯 Готовые команды для копирования

```bash
# Одна строка для сборки всего проекта
export PATH="/opt/homebrew/Cellar/qt/6.9.3/bin:$PATH" && \
export LDFLAGS="-L/opt/homebrew/opt/openssl/lib" && \
cd server && /opt/homebrew/Cellar/qt/6.9.3/bin/qmake && make && \
cd ../client && /opt/homebrew/Cellar/qt/6.9.3/bin/qmake && make && \
echo "✅ Сборка завершена!"
```

## 🎮 Использование приложения

1. **Запустите сервер**: `./server/fiat_shamir_server`
2. **Запустите клиент**: `./client/fiat_shamir_client`
3. **Подключитесь** к серверу (127.0.0.1:12345)
4. **Зарегистрируйтесь** - получите секретный ключ
5. **Войдите** с полученным ключом

Проект полностью готов к использованию на вашем MacBook!