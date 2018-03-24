#-------------------------------------------------
#
# Project created by QtCreator 2017-11-04T18:38:20
#
#-------------------------------------------------

QT       += core gui
QT       += uitools
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT       +=designer

TARGET = hellow
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
    finddialog.cpp \
    gotocelldialog.cpp \
    sortdialog.cpp \
    mainwindow.cpp \
    spreadsheet.cpp \
    cell.cpp \
    user-defined-component/hexspinbox.cpp \
    user-defined-component/iconEditor/iconeditor.cpp \
    user-defined-component/plotter/plotter.cpp \
    user-defined-component/mdiEditor/editor.cpp \
    user-defined-component/mdiEditor/mdiwindow.cpp \
    user-defined-component/timeTicker/ticker.cpp \
    user-defined-component/ovenTimer/oventimer.cpp

HEADERS  += \
    finddialog.h \
    gotocelldialog.h \
    sortdialog.h \
    mainwindow.h \
    spreadsheet.h \
    cell.h \
    user-defined-component/hexspinbox.h \
    user-defined-component/iconEditor/iconeditor.h \
    user-defined-component/plotter/plotter.h \
    user-defined-component/mdiEditor/editor.h \
    user-defined-component/mdiEditor/mdiwindow.h \
    user-defined-component/timeTicker/ticker.h \
    user-defined-component/ovenTimer/oventimer.h

FORMS    += \
    gotocelldialog.ui \
    sortdialog.ui

RESOURCES += \
    spreadsheet.qrc \
    res/plotter.qrc
