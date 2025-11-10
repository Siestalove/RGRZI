QT -= gui
TARGET = fiat_shamir_server
TEMPLATE = app
CONFIG += console c++17

SOURCES += \
    main.cpp \
    server.cpp \
    ../common/fiat_shamir.cpp \
    ../common/network_message.cpp

HEADERS += \
    server.h \
    ../common/fiat_shamir.h \
    ../common/network_message.h

LIBS += -lssl -lcrypto

INCLUDEPATH += ../common