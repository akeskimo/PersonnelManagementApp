#-------------------------------------------------
#
# Project created by QtCreator 2016-09-19T21:27:32
#
#-------------------------------------------------

QT       += core gui sql

CONFIG += console

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PersonnelManagementApp2
TEMPLATE = app

INCLUDEPATH += .

SOURCES += main.cpp \
        mainwindow.cpp \
    employeedelegate.cpp \
    employeeeditor.cpp

HEADERS  += mainwindow.h \
    employeedelegate.h \
    employeeeditor.h \
    globals.h \
    database.h

FORMS    += mainwindow.ui \
    dialog.ui

RESOURCES +=
