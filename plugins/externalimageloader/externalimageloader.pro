TARGET = $$qtLibraryTarget(externalimageloader)
TEMPLATE = lib
CONFIG += plugin
DESTDIR = ../
HEADERS += ../../src/imageloader.h \
    externalimageloader.h
SOURCES += externalimageloader.cpp
TRANSLATIONS += externalimageloader_ru.ts
RESOURCES += externalimageloader.qrc
