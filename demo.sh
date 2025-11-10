#!/bin/bash

# Демонстрационный скрипт запуска приложения Фиата-Шамира

echo "🔐 Демонстрация протокола Фиата-Шамира"
echo "======================================"

# Проверка наличия исполняемых файлов
SERVER_CMD="./build/fiat_shamir_server"
CLIENT_CMD="./build/fiat_shamir_client"

if [ ! -f "$SERVER_CMD" ]; then
    SERVER_CMD="./server/fiat_shamir_server"
fi

if [ ! -f "$CLIENT_CMD" ]; then
    CLIENT_CMD="./client/fiat_shamir_client"
fi

if [ ! -f "$SERVER_CMD" ]; then
    echo "❌ Сервер не найден. Сначала соберите проект:"
    echo "./build_macos.sh"
    exit 1
fi

if [ ! -f "$CLIENT_CMD" ]; then
    echo "❌ Клиент не найден. Сначала соберите проект:"
    echo "./build_macos.sh"
    exit 1
fi

echo "🚀 Запуск сервера..."
$SERVER_CMD &
SERVER_PID=$!

# Ждем запуск сервера
sleep 2

echo "📱 Запуск клиента..."
echo ""
echo "📋 Инструкция по использованию:"
echo "1. В клиенте нажмите 'Подключиться'"
echo "2. Нажмите 'Зарегистрироваться' и введите логин"
echo "3. Сохраните выданный секретный ключ"
echo "4. Нажмите 'Войти в систему' и введите логин и секретный ключ"
echo "5. Наблюдайте за процессом аутентификации"
echo ""
echo "🛑 Для остановки нажмите Ctrl+C в терминале сервера"
echo ""

$CLIENT_CMD

# Остановка сервера при выходе клиента
kill $SERVER_PID 2>/dev/null