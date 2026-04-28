QT       += core widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = RealEstateApp

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    fileloaderdialog.cpp \
    realestate.cpp \
    propertymanager.cpp \
    propertyparser.cpp

HEADERS += \
    mainwindow.h \
    fileloaderdialog.h \
    realestate.h \
    propertymanager.h \
    propertyparser.h

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

    SOURCES += tests/test_propertyparser.cpp
}

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target