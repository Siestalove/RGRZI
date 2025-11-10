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

# Установка переменных окружения для OpenSSL и Qt
if [ -d "/opt/homebrew" ]; then
    # Apple Silicon (M1/M2)
    export LDFLAGS="-L/opt/homebrew/opt/openssl/lib"
    export CPPFLAGS="-I/opt/homebrew/opt/openssl/include"
    export PKG_CONFIG_PATH="/opt/homebrew/opt/openssl/lib/pkgconfig"
    export CMAKE_PREFIX_PATH="/opt/homebrew"
    export PATH="/opt/homebrew/bin:/opt/homebrew/opt/qt/bin:$PATH"
    QT_PATH="/opt/homebrew/Cellar/qt/6.9.3"
else
    # Intel Mac
    export LDFLAGS="-L/usr/local/opt/openssl/lib"
    export CPPFLAGS="-I/usr/local/opt/openssl/include"
    export PKG_CONFIG_PATH="/usr/local/opt/openssl/lib/pkgconfig"
    export CMAKE_PREFIX_PATH="/usr/local"
    export PATH="/usr/local/bin:/usr/local/opt/qt/bin:$PATH"
    QT_PATH="/usr/local/Cellar/qt/6.9.3"
fi

# Поиск Qt
if command -v qmake6 &> /dev/null; then
    QMAKE_CMD="qmake6"
elif command -v qmake &> /dev/null; then
    QMAKE_CMD="qmake"
else
    # Пробуем прямой путь
    if [ -f "$QT_PATH/bin/qmake" ]; then
        QMAKE_CMD="$QT_PATH/bin/qmake"
    else
        echo "❌ qmake не найден"
        exit 1
    fi
fi

echo "✅ Зависимости найдены"
echo "🔨 Используем qmake: $QMAKE_CMD"
echo "🔨 Qt путь: $QT_PATH"

# Создание директории сборки
mkdir -p build
cd build

# Сборка с помощью CMake (предпочтительный способ)
echo "🏗️ Сборка с помощью CMake..."
cmake .. -DCMAKE_PREFIX_PATH="$QT_PATH" -DQt6_ROOT="$QT_PATH"
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