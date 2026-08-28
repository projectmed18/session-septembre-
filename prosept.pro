QT       += core gui sql charts printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
    CoursWidget.cpp \
    InscriptionWidget.cpp \
    Stagiairewidget.cpp \
    alertescoursdialog.cpp \
    capacitedialog.cpp \
    classementcoursdialog.cpp \
    cours.cpp \
    coursdao.cpp \
    databasemanager.cpp \
    fichesuividialog.cpp \
    inscription.cpp \
    inscriptiondao.cpp \
    main.cpp \
    mainwindow.cpp \
    pdfgenerator.cpp \
    pdfgeneratorcours.cpp \
    stagiaire.cpp \
    stagiairedao.cpp \
    statistiquedialog.cpp \
    statistiquescoursdialog.cpp

HEADERS += \
    CoursWidget.h \
    InscriptionWidget.h \
    Stagiairewidget.h \
    alertescoursdialog.h \
    capacitedialog.h \
    classementcoursdialog.h \
    cours.h \
    coursdao.h \
    databasemanager.h \
    fichesuividialog.h \
    inscription.h \
    inscriptiondao.h \
    mainwindow.h \
    pdfgenerator.h \
    pdfgeneratorcours.h \
    stagiaire.h \
    stagiairedao.h \
    statistiquedialog.h \
    statistiquescoursdialog.h

FORMS += \
    mainwindow.ui

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
