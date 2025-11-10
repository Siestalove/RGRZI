QT += widgets network
TARGET = fiat_shamir_client
TEMPLATE = app
CONFIG += c++17

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    authdialog.cpp \
    registerdialog.cpp \
    ../common/fiat_shamir.cpp \
    ../common/network_message.cpp

HEADERS += \
    mainwindow.h \
    authdialog.h \
    registerdialog.h \
    ../common/fiat_shamir.h \
    ../common/network_message.h

LIBS += -lssl -lcrypto

INCLUDEPATH += ../common