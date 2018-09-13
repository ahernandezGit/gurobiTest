#-------------------------------------------------
#
# Project created by QtCreator 2018-09-13T10:15:57
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TestGraphGurobi
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

GUROBI_PATH= /opt/gurobi801/linux64
EIGEN_PATH =  /usr/include/eigen3
BUILDDIR = $$PWD/build/

INCLUDEPATH += $$GUROBI_PATH/include
LIBS += -L$$GUROBI_PATH/lib
LIBS += -lgurobi_g++5.2 -lgurobi80 -lpthread
# Directories
OBJECTS_DIR =   $$BUILDDIR/obj
MOC_DIR =       $$BUILDDIR/moc
RCC_DIR =       $$BUILDDIR/rccFind
UI_DIR =        $$BUILDDIR/ui
DESTDIR =       $$PWD/bin
# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        mainwindow.h

FORMS += \
        mainwindow.ui
