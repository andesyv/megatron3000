QT       += core gui widgets opengl

CONFIG += c++17

!versionAtLeast(QT_VERSION, 5.4.0) {
    message("Cannot use Qt $${QT_VERSION}")
    error("Use Qt 5.4 or newer")
}

SHADER_PATH = $${PWD}/src/shaders
DEFINES += "SHADERPATH=\\\"$$SHADER_PATH\\\""

SOURCES += \
    src/datawidget.cpp \
    src/histogramwidget.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/renderer.cpp \
    src/transferfunctionwidget.cpp \
    src/viewport2d.cpp \
    src/viewport3d.cpp \
    src/volume.cpp \
    src/menuinterface.cpp \
    src/shaders/shadermanager.cpp

HEADERS += \
    src/datawidget.h \
    src/histogramwidget.h \
    src/mainwindow.h \
    src/renderer.h \
    src/transferfunctionwidget.h \
    src/viewport2d.h \
    src/viewport3d.h \
    src/volume.h \
    src/menuinterface.h \
    src/shaders/shadermanager.h

FORMS += \
    src/datawidget.ui \
    src/histogramwidget.ui \
    src/mainwindow.ui \
    src/transferfunctionwidget.ui

DISTFILES += \
    src/shaders/default.vs \
    src/shaders/default.fs \
    src/shaders/screen.vs \
    src/shaders/screen.fs
