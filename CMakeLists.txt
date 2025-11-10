cmake_minimum_required(VERSION 3.16)
project(FiatShamirAuth)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Установка путей для Homebrew на macOS
if(APPLE)
    # Для Apple Silicon (M1/M2)
    if(EXISTS "/opt/homebrew")
        set(CMAKE_PREFIX_PATH "/opt/homebrew")
        set(OPENSSL_ROOT_DIR "/opt/homebrew/opt/openssl")
        set(CMAKE_FRAMEWORK_PATH "/opt/homebrew/lib")
    # Для Intel Mac
    elseif(EXISTS "/usr/local")
        set(CMAKE_PREFIX_PATH "/usr/local")
        set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl")
    endif()
endif()

# Находим необходимые пакеты
find_package(OpenSSL REQUIRED)
find_package(Qt6 COMPONENTS Core Widgets Network REQUIRED)

# Если Qt6 не найден, пробуем альтернативные пути
if(NOT Qt6_FOUND)
    message(STATUS "Qt6 не найден, пробуем альтернативные пути...")
    
    # Пробуем стандартные пути Homebrew
    set(Qt6_ROOT "/opt/homebrew/Cellar/qt/6.9.3")
    find_package(Qt6 COMPONENTS Core Widgets Network REQUIRED)
    
    if(NOT Qt6_FOUND)
        set(Qt6_ROOT "/opt/homebrew/opt/qt")
        find_package(Qt6 COMPONENTS Core Widgets Network REQUIRED)
    endif()
endif()

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