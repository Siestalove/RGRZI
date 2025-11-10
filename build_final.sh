#!/bin/bash

# Сборка проекта Фиата-Шамира для macOS с Qt 6.9.3
# Адаптируйте путь QT_PATH если у вас другая версия

# Укажите ваш путь к Qt
QT_PATH="/opt/homebrew/Cellar/qt/6.9.3"

# Если Qt установлен в другом месте, измените эту переменную:
# QT_PATH="/path/to/your/qt"

echo "🔧 Сборка проекта Фиата-Шамира"
echo "📍 Путь к Qt: $QT_PATH"

# Проверка наличия Qt
if [ ! -f "$QT_PATH/bin/qmake" ]; then
    echo "❌ qmake не найден в $QT_PATH/bin/qmake"
    echo "Пожалуйста, установите правильный путь в переменную QT_PATH"
    exit 1
fi

# Установка переменных окружения
export PATH="$QT_PATH/bin:$PATH"
export LDFLAGS="-L/opt/homebrew/opt/openssl/lib"
export CPPFLAGS="-I/opt/homebrew/opt/openssl/include"
export PKG_CONFIG_PATH="/opt/homebrew/opt/openssl/lib/pkgconfig"

echo "✅ Переменные окружения установлены"

# Сборка сервера
echo "📦 Сборка сервера..."
cd server
$QT_PATH/bin/qmake server.pro

if [ $? -ne 0 ]; then
    echo "❌ Ошибка выполнения qmake для сервера"
    exit 1
fi

make

if [ $? -ne 0 ]; then
    echo "❌ Ошибка сборки сервера"
    exit 1
fi

echo "✅ Сервер собран"

# Сборка клиента
echo "📦 Сборка клиента..."
cd ../client
$QT_PATH/bin/qmake client.pro

if [ $? -ne 0 ]; then
    echo "❌ Ошибка выполнения qmake для клиента"
    exit 1
fi

make

if [ $? -ne 0 ]; then
    echo "❌ Ошибка сборки клиента"
    exit 1
fi

echo "✅ Клиент собран"

cd ..

echo ""
echo "🎉 Сборка завершена успешно!"
echo ""
echo "🚀 Для запуска:"
echo "1. Откройте терминал и запустите сервер:"
echo "   ./server/fiat_shamir_server"
echo ""
echo "2. Откройте второй терминал и запустите клиент:"
echo "   ./client/fiat_shamir_client"
echo ""
echo "3. В клиенте:"
echo "   - Нажмите 'Подключиться'"
echo "   - Зарегистрируйте пользователя"
echo "   - Сохраните секретный ключ"
echo "   - Войдите в систему"
echo ""
echo "🔐 Наслаждайтесь безопасной аутентификацией по протоколу Фиата-Шамира!"