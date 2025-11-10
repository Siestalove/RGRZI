cmake_minimum_required(VERSION 3.16)
project(FiatShamirAuth)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Находим необходимые пакеты
find_package(OpenSSL REQUIRED)
find_package(Qt6 COMPONENTS Core Widgets Network REQUIRED)

# Добавляем общие файлы
set(COMMON_SOURCES
    common/fiat_shamir.cpp
    common/network_message.cpp
)

set(COMMON_HEADERS
    common/fiat_shamir.h
    common/network_message.h
)

# Сервер
add_executable(fiat_shamir_server
    server/main.cpp
    server/server.cpp
    ${COMMON_SOURCES}
)

target_include_directories(fiat_shamir_server PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/common
)

target_link_libraries(fiat_shamir_server
    Qt6::Core
    OpenSSL::SSL
    OpenSSL::Crypto
)

# Клиент
add_executable(fiat_shamir_client
    client/main.cpp
    client/mainwindow.cpp
    client/authdialog.cpp
    client/registerdialog.cpp
    ${COMMON_SOURCES}
)

target_include_directories(fiat_shamir_client PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/common
)

target_link_libraries(fiat_shamir_client
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    OpenSSL::SSL
    OpenSSL::Crypto
)

# Установка
install(TARGETS fiat_shamir_server fiat_shamir_client
    RUNTIME DESTINATION bin
)