#TEMPLATE = app
TEMPLATE = lib
CONFIG += console c++11
CONFIG -= app_bundle qt
CONFIG += staticlib


Release:DESTDIR     = $$PWD/bin/release
Release:OBJECTS_DIR = $$PWD/bin/release/.obj
Release:MOC_DIR     = $$PWD/bin/release/.moc
Release:RCC_DIR     = $$PWD/bin/release/.rcc
Release:UI_DIR      = $$PWD/bin/release/.ui

Debug:DESTDIR       = $$PWD/bin/debug
Debug:OBJECTS_DIR   = $$PWD/bin/debug/.obj
Debug:MOC_DIR       = $$PWD/bin/debug/.moc
Debug:RCC_DIR       = $$PWD/bin/debug/.rcc
Debug:UI_DIR        = $$PWD/bin/debug/.ui


INCLUDEPATH += ./include

#LIBS +=


SOURCES += main.cpp \
    src/config.cpp \
    test/test_config.hpp

HEADERS += \
    include/pt/config.h \
    include/pt/std_extension.hpp \
    include/pt/event.hpp
