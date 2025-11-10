#!/bin/bash

# Скрипт сборки проекта Фиата-Шамира на macOS

echo "🔧 Сборка проекта Фиата-Шамира для macOS..."

# Проверка наличия зависимостей
echo "📦 Проверка зависимостей..."

# Проверка Qt6
if ! command -v qmake6 &> /dev/null && ! command -v qmake &> /dev/null; then
    echo "❌ Qt6 не найден. Установите Qt6:"
    echo "brew install qt@6"
    echo "Или добавьте Qt в PATH:"
    echo "export PATH=\"/usr/local/opt/qt@6/bin:\$PATH\""
    exit 1
fi

# Проверка OpenSSL
if ! pkg-config --exists openssl; then
    echo "❌ OpenSSL не найден. Установите OpenSSL:"
    echo "brew install openssl"
    echo "Или установите переменные окружения:"
    echo "export LDFLAGS=\"-L/usr/local/opt/openssl/lib\""
    echo "export CPPFLAGS=\"-I/usr/local/opt/openssl/include\""
    echo "export PKG_CONFIG_PATH=\"/usr/local/opt/openssl/lib/pkgconfig\""
    exit 1
fi

# Проверка CMake
if ! command -v cmake &> /dev/null; then
    echo "❌ CMake не найден. Установите CMake:"
    echo "brew install cmake"
    exit 1
fi

# Установка переменных окружения для OpenSSL
export LDFLAGS="-L/usr/local/opt/openssl/lib"
export CPPFLAGS="-I/usr/local/opt/openssl/include"
export PKG_CONFIG_PATH="/usr/local/opt/openssl/lib/pkgconfig"

# Поиск Qt
if command -v qmake6 &> /dev/null; then
    QMAKE_CMD="qmake6"
else
    QMAKE_CMD="qmake"
fi

echo "✅ Зависимости найдены"
echo "🔨 Используем qmake: $QMAKE_CMD"

# Создание директории сборки
mkdir -p build
cd build

# Сборка с помощью CMake (предпочтительный способ)
echo "🏗️ Сборка с помощью CMake..."
cmake .. -DCMAKE_PREFIX_PATH="$(dirname $(dirname $($QMAKE_CMD -query QT_INSTALL_HEADERS)))"
make

if [ $? -eq 0 ]; then
    echo "✅ Сборка завершена успешно!"
    echo ""
    echo "🚀 Для запуска:"
    echo "1. Сервер: ./fiat_shamir_server"
    echo "2. Клиент: ./fiat_shamir_client"
    echo ""
    echo "📝 Не забудьте, что сервер должен быть запущен первым!"
else
    echo "❌ Ошибка сборки. Пробуем альтернативный способ..."
    
    # Альтернативная сборка с qmake
    cd ..
    echo "🔨 Сборка с помощью qmake..."
    
    # Сборка сервера
    cd server
    $QMAKE_CMD server.pro
    make
    
    if [ $? -eq 0 ]; then
        echo "✅ Сервер собран"
    else
        echo "❌ Ошибка сборки сервера"
        exit 1
    fi
    
    cd ../client
    $QMAKE_CMD client.pro
    make
    
    if [ $? -eq 0 ]; then
        echo "✅ Клиент собран"
    else
        echo "❌ Ошибка сборки клиента"
        exit 1
    fi
    
    cd ..
    
    echo "✅ Сборка завершена!"
    echo "🚀 Исполняемые файлы:"
    echo "- Сервер: server/fiat_shamir_server"
    echo "- Клиент: client/fiat_shamir_client"
fi