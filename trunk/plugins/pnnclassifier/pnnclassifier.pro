TARGET = $$qtLibraryTarget(pnnclassifier)
TEMPLATE = lib
CONFIG += plugin
DESTDIR = ../
HEADERS += ../../src/classifier.h \
	pnnclassifier.h
SOURCES += pnnclassifier.cpp
