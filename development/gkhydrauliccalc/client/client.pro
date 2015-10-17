TEMPLATE = app
TARGET = GKHydraulicCalc

QT += core gui widgets

DESTDIR = $$PWD/../../bin

INCLUDEPATH += $$PWD/../3rdparty/python2.6.6/include
LIBS += -L$$PWD/../3rdparty/python2.6.6/libs -lpython26

HEADERS += \
    MainWindow.h \
    ScriptParser.h \
    CylinderParam.h \
    CurvePlotter.h

SOURCES += \
    Main.cc \
    MainWindow.cc \
    ScriptParser.cc \
    CurvePlotter.cc

FORMS += \
    MainWindow.ui

RESOURCES += \
    resources.qrc
