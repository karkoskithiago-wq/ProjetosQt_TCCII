QT       += core gui sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
    databasemanager.cpp \
    main.cpp \
    login.cpp \
    principal.cpp \
    produto.cpp

HEADERS += \
    databasemanager.h \
    databasemanager_copy.h \
    login.h \
    principal.h \
    produto.h

FORMS += \
    login.ui \
    principal.ui \
    produto.ui

TRANSLATIONS += \
    SistemaVendasTCC_pt_BR.ts

CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
