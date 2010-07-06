TARGET = $$qtLibraryTarget(saneimageloader)
TEMPLATE = lib
CONFIG += plugin
DESTDIR = ../
HEADERS += ../../src/imageloader.h \
    saneimageloader.h
LIBS += -L/usr/lib -lsane
SOURCES += saneimageloader.cpp
RESOURCES += saneimageloader.qrc
TRANSLATIONS += saneimageloader_ru.ts
