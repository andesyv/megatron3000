QT       += core gui widgets

CONFIG += c++17

SOURCES += \
    src/histogramwidget.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/transferfunctionwidget.cpp \
    src/viewport2d.cpp \
    src/viewport3d.cpp

HEADERS += \
    src/histogramwidget.h \
    src/mainwindow.h \
    src/transferfunctionwidget.h \
    src/viewport2d.h \
    src/viewport3d.h

FORMS += \
    src/histogramwidget.ui \
    src/mainwindow.ui \
    src/transferfunctionwidget.ui \
    src/viewport2d.ui \
    src/viewport3d.ui