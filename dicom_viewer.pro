#-------------------------------------------------
#
# Project created by QtCreator 2020-09-04T09:30:51
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = dicom_viewer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += /net/travail/lhofer/public_teaching/dcmtk-3.6.1/usr/local/include

SOURCES += \
        main.cpp \
        dicom_viewer.cpp


HEADERS += \
        dicom_viewer.h

QMAKE_LFLAGS += -Wl,--rpath=/net/travail/lhofer/public_teaching/dcmtk-3.6.1/usr/local/lib

LIBS += -L/net/travail/lhofer/public_teaching/dcmtk-3.6.1/usr/local/lib \
        -ldcmimage \
        -ldcmimgle \
        -ldcmjpeg \
        -lijg8 \
        -lijg12 \
        -lijg16 \
        -ldcmdata \
        -loflog \
        -lofstd
