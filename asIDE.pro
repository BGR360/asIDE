#-------------------------------------------------
#
# Project created by QtCreator 2016-04-03T17:39:01
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = subdirs

SUBDIRS = \
    Intellisense \
    App \
    Tests

Intellisense.subdir = src/intellisense
App.subdir = src/app
Tests.subdir = src/tests

App.depends = Intellisense
Tests.depends = Intellisense
