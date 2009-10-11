TARGET = $$qtLibraryTarget(fileimageloader)
TEMPLATE = lib
CONFIG += plugin
DESTDIR = ../
HEADERS += ../../src/imageloader.h \
    fileimageloader.h
SOURCES += fileimageloader.cpp
RESOURCES += fileimageloader.qrc
TRANSLATIONS += fileimageloader_ru.ts
