TARGET = qsigillum
DESTDIR = bin
TEMPLATE = app
QT += xml
SOURCES += src/main.cpp \
    src/userform.cpp \
    src/editorform.cpp \
    src/logiccore.cpp \
    src/preprocessor.cpp \
    src/segmentator.cpp \
    src/segmentationtemplate.cpp \
    src/templatecontainer.cpp \
    src/templatefield.cpp
HEADERS += src/userform.h \
    src/editorform.h \
    src/logiccore.h \
    src/preprocessor.h \
    src/segmentator.h \
    src/segmentationtemplate.h \
    src/imageloader.h \
    src/classifier.h \
    src/outputexporter.h \
    src/templatecontainer.h \
    src/templatefield.h \
    FORMS \
    += \
    src/userform.ui \
    src/editorform.ui
RESOURCES += res/qsigillum.qrc
RC_FILE = res/qsigillum.rc
TRANSLATIONS += res/qsigillum_ru.ts
