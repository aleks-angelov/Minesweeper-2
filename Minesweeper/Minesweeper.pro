#-------------------------------------------------
#
# Project created by QtCreator 2014-01-03T17:04:23
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Minesweeper
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    mypushbutton.cpp \
    difficultywindow.cpp

HEADERS  += widget.h \
    mypushbutton.h \
    difficultywindow.h

FORMS    += widget.ui \
    difficultywindow.ui

RESOURCES += \
    resources.qrc
