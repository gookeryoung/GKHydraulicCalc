TEMPLATE = app
TARGET = GKHydraulicCalc

QT += widgets

DESTDIR = $$PWD/../../bin

win32: {
    INCLUDEPATH += $$PWD/../3rdparty/python2.6.6/include
    LIBS += -L$$PWD/../3rdparty/python2.6.6/libs -lpython26
}

unix: {
#    INCLUDEPATH += /usr/bin/python/include
    LIBS += -lpython
    INCLUDEPATH += /usr/local/Cellar/python/2.7.13/Frameworks/Python.framework/Versions/2.7/include/python2.7
}

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
