
QT       += core gui sql webkit

TARGET = AIS
TEMPLATE = app

SOURCES += \
    sources/workStatement.cpp \
    sources/table.cpp \
    sources/settings.cpp \
    sources/reportHTML.cpp \
    sources/report.cpp \
    sources/mainWindow.cpp \
    sources/main.cpp \
    sources/ganttChart.cpp \
    sources/error.cpp \
    sources/card.cpp \
    sources/auth.cpp

HEADERS += \
    headers/workStatement.h \
    headers/table.h \
    headers/settings.h \
    headers/reportHTML.h \
    headers/report.h \
    headers/mainWindow.h \
    headers/ganttChart.h \
    headers/error.h \
    headers/card.h \
    headers/auth.h

RESOURCES += \
    images.qrc \
    html.qrc
