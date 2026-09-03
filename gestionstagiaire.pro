QT       += core gui sql charts printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    capacitedialog.cpp \
    coursdao.cpp \
    databasemanager.cpp \
    fichesuividialog.cpp \
    main.cpp \
    mainwindow.cpp \
    pdfgenerator.cpp \
    stagiaire.cpp \
    stagiairedao.cpp \
    statistiquedialog.cpp

HEADERS += \
    capacitedialog.h \
    coursdao.h \
    databasemanager.h \
    fichesuividialog.h \
    mainwindow.h \
    pdfgenerator.h \
    stagiaire.h \
    stagiairedao.h \
    statistiquedialog.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
