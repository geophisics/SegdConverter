#-------------------------------------------------
#
# Project created by QtCreator 2016-03-23T08:56:47
#
#-------------------------------------------------

QT       += core gui xlsx charts multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SegdConverter
TEMPLATE = app


SOURCES += main.cpp\
        segdconverterwindow.cpp \
    convertparametersdialog.cpp \
    binningparametersdialog.cpp \
    mydoublevalidator.cpp \
    windowsparametersdialog.cpp \
    attributewindow.cpp \
    Segd/channelsetheader.cpp \
    Segd/extendedheader.cpp \
    Segd/generalone.cpp \
    Segd/generalthree.cpp \
    Segd/generaltwo.cpp \
    Segd/segdtrace.cpp \
    Segd/traceextensionfive.cpp \
    Segd/traceextensionfour.cpp \
    Segd/traceextensionone.cpp \
    Segd/traceextensionseven.cpp \
    Segd/traceextensionsix.cpp \
    Segd/traceextensionthree.cpp \
    Segd/traceextensiontwo.cpp \
    Segd/traceheader.cpp \
    Segd/segdfile.cpp \
    Segy/segyfile.cpp \
    Segy/segytrace.cpp \
    Workers/segyworker.cpp \
    tableviewdialog.cpp \
    Workers/cstworker.cpp \
    Workers/baseworker.cpp \
    Cst/cstfile.cpp \
    Cst/csttrace.cpp \
    auxesdialog.cpp \
    auxviewdialog.cpp \
    Models/attributesmodel.cpp \
    SUB/exclusion.cpp \
    SUB/point.cpp \
    segdtestdialog.cpp \
    Models/testmodel.cpp \
    testviewdialog.cpp \
    testparametersdialog.cpp

HEADERS  += segdconverterwindow.h \
    convertparametersdialog.h \
    binningparametersdialog.h \
    mydoublevalidator.h \
    windowsparametersdialog.h \
    attributewindow.h \
    Segd/channelsetheader.h \
    Segd/extendedheader.h \
    Segd/generalone.h \
    Segd/generalthree.h \
    Segd/generaltwo.h \
    Segd/segdtrace.h \
    Segd/traceextensionfive.h \
    Segd/traceextensionfour.h \
    Segd/traceextensionone.h \
    Segd/traceextensionseven.h \
    Segd/traceextensionsix.h \
    Segd/traceextensionthree.h \
    Segd/traceextensiontwo.h \
    Segd/traceheader.h \
    Segd/segdfile.h \
    Segy/segyfile.h \
    Segy/segytrace.h \
    Workers/segyworker.h \
    tableviewdialog.h \
    Workers/cstworker.h \
    Workers/baseworker.h \
    Cst/cstfile.h \
    Cst/csttrace.h \
    auxesdialog.h \
    auxviewdialog.h \
    Models/attributesmodel.h \
    SUB/exclusion.h \
    SUB/point.h \
    SUB/threadsafevector.h \
    SUB/general.h \
    segdtestdialog.h \
    SUB/threadsavemap.h \
    Models/testmodel.h \
    testviewdialog.h \
    testparametersdialog.h



FORMS    += segdconverterwindow.ui \
    convertparametersdialog.ui \
    binningparametersdialog.ui \
    windowsparametersdialog.ui \
    tableviewdialog.ui \
    auxesdialog.ui \
    auxviewdialog.ui \
    segdtestdialog.ui \
    testviewdialog.ui \
    testparametersdialog.ui

RESOURCES += \
    icons.qrc



unix|win32: LIBS += -L$$PWD/../../AquilaLib/lib/ -lAquila
unix|win32: LIBS += -L$$PWD/../../AquilaLib/lib/ -lOoura_fft

INCLUDEPATH += $$PWD/../../AquilaLib/include
DEPENDPATH += $$PWD/../../AquilaLib/include


