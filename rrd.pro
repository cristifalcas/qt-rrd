# -------------------------------------------------
# Project created by QtCreator 2009-03-05T15:05:08
# -------------------------------------------------
TARGET = qtrrd
TEMPLATE = app
SOURCES += main.cpp \
    statsgraph.cpp \
    pages.cpp \
    pagegeneralconfiguration.cpp \
    pagegenerategraph.cpp \
    pageextractstats.cpp \
    widgetconfigrrd.cpp \
    widgetconfigplot.cpp \
    parsingstats.cpp \
    commons.cpp \
    extractstatistics.cpp \
    rrdtool.cpp \
    rrdinfo.cpp \
    mythread.cpp \
    messages.cpp
HEADERS += statsgraph.h \
    pages.h \
    pagegeneralconfiguration.h \
    pagegenerategraph.h \
    pageextractstats.h \
    widgetconfigrrd.h \
    widgetconfigplot.h \
    parsingstats.h \
    commons.h \
    extractstatistics.h \
    rrdtool.h \
    rrdinfo.h \
    mythread.h \
    messages.h
CONFIG += thread
