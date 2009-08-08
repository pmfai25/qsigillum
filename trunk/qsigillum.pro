TARGET = qsigillum
DESTDIR = bin
TEMPLATE = app
QT += network
SOURCES += src/main.cpp \
    src/userform.cpp \
    src/editorform.cpp \
    src/logiccore.cpp \
    src/preprocessor.cpp \
    src/segmentator.cpp \
    src/segmentationtemplate.cpp \
    src/outputdata.cpp
HEADERS += src/userform.h \
    src/editorform.h \
    src/logiccore.h \
    src/preprocessor.h \
    src/segmentator.h \
    src/segmentationtemplate.h \
    src/outputdata.h \
    src/ImageLoader.h \
    src/Classificator.h \
    src/OutputExporter.h
FORMS += src/userform.ui \
    src/editorform.ui
RESOURCES += res/qsigillum.qrc
RC_FILE = res/oxytest.rc
TRANSLATIONS += res/qsigillum_ru.ts
