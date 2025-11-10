#!/bin/bash

# Быстрая сборка с qmake для macOS с Homebrew Qt

echo "🚀 Быстрая сборка с qmake..."

# Определение пути Qt - адаптируйте под вашу систему
if [ -d "/opt/homebrew/Cellar/qt/6.9.3" ]; then
    QT_PATH="/opt/homebrew/Cellar/qt/6.9.3"
    QMAKE_CMD="$QT_PATH/bin/qmake"
elif [ -d "/usr/local/Cellar/qt/6.9.3" ]; then
    QT_PATH="/usr/local/Cellar/qt/6.9.3"
    QMAKE_CMD="$QT_PATH/bin/qmake"
elif command -v qmake6 &> /dev/null; then
    QMAKE_CMD="qmake6"
    QT_PATH=$(dirname $(dirname $(which qmake6)))
elif command -v qmake &> /dev/null; then
    QMAKE_CMD="qmake"
    QT_PATH=$(dirname $(dirname $(which qmake)))
else
    echo "❌ Qt не найден. Укажите путь вручную:"
    echo "Редактируйте этот скрипт и установите QT_PATH"
    echo "Пример для macOS с Homebrew:"
    echo "export QT_PATH=\"/opt/homebrew/Cellar/qt/6.9.3\""
    exit 1
fi

echo "🔨 Используем Qt: $QT_PATH"
echo "🔨 qmake: $QMAKE_CMD"

# Установка переменных окружения
export LDFLAGS="-L/opt/homebrew/opt/openssl/lib"
export CPPFLAGS="-I/opt/homebrew/opt/openssl/include"
export PKG_CONFIG_PATH="/opt/homebrew/opt/openssl/lib/pkgconfig"

# Сборка сервера
echo "📦 Сборка сервера..."
cd server
$QMAKE_CMD server.pro
make

if [ $? -eq 0 ]; then
    echo "✅ Сервер собран успешно"
else
    echo "❌ Ошибка сборки сервера"
    exit 1
fi

# Сборка клиента
echo "📦 Сборка клиента..."
cd ../client
$QMAKE_CMD client.pro
make

if [ $? -eq 0 ]; then
    echo "✅ Клиент собран успешно"
else
    echo "❌ Ошибка сборки клиента"
    exit 1
fi

cd ..

echo "🎉 Сборка завершена!"
echo ""
echo "🚀 Для запуска:"
echo "1. Сервер: ./server/fiat_shamir_server"
echo "2. Клиент: ./client/fiat_shamir_client"