TARGET = $$qtLibraryTarget(numberclassifier)
TEMPLATE = lib
CONFIG += plugin
DESTDIR = ../
HEADERS += ../../src/classifier.h \
	numberclassifier.h
SOURCES += numberclassifier.cpp
