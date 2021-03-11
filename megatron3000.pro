QT       += core gui widgets opengl

CONFIG += c++17

!versionAtLeast(QT_VERSION, 5.4.0) {
    message("Cannot use Qt $${QT_VERSION}")
    error("Use Qt 5.4 or newer")
}

SOURCES += \
    src/datawidget.cpp \
    src/histogramwidget.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/renderer.cpp \
    src/transferfunctionwidget.cpp \
    src/viewport2d.cpp \
    src/viewport3d.cpp \
    src/volume.cpp

HEADERS += \
    src/datawidget.h \
    src/histogramwidget.h \
    src/mainwindow.h \
    src/renderer.h \
    src/transferfunctionwidget.h \
    src/viewport2d.h \
    src/viewport3d.h \
    src/volume.h

FORMS += \
    src/datawidget.ui \
    src/histogramwidget.ui \
    src/mainwindow.ui \
    src/transferfunctionwidget.ui \
    src/viewport2d.ui \
    src/viewport3d.ui
