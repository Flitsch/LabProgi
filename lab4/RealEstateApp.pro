QT       += core widgets testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
CONFIG += testlib

TARGET = RealEstateApp

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    fileloaderdialog.cpp \
    realestate.cpp \
    propertymanager.cpp \
    propertyparser.cpp \
    commandprocessor.cpp

HEADERS += \
    mainwindow.h \
    fileloaderdialog.h \
    realestate.h \
    propertymanager.h \
    propertyparser.h \
    commandprocessor.h

FORMS += \
    mainwindow.ui

# Тестовая конфигурация
test {
    message("=== ТЕСТОВАЯ СБОРКА ===")

    QT += testlib
    CONFIG += console
    CONFIG -= app_bundle

    TARGET = RealEstateApp_tests

    SOURCES -= main.cpp mainwindow.cpp fileloaderdialog.cpp
    HEADERS -= mainwindow.h fileloaderdialog.h
    FORMS -= mainwindow.ui

    # Только тесты файловых операций
    SOURCES += tests/test_fileoperations.cpp
}

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target