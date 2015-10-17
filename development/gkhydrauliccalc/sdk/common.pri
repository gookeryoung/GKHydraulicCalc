include(../Portable.pri)

TEMPLATE = lib
TARGET = GKGE$$TARGET

CONFIG -= qt
CONFIG += warn_on

CONFIG(debug, debug|release) {
    CONFIG += console
}

CONFIG(__MSVC__) {
    CONFIG += staticlib
}

INCLUDEPATH += $$PWD

DESTDIR = $$PWD/../Lib
DLLDESTDIR = $$PWD/../Bin


