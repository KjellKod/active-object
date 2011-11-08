#-------------------------------------------------
#
# A Qt creator generated project file
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = active-object_qt
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += \
    activeqthread.cpp \
    test/main.cpp \
    test/test_bg_worker.cpp

HEADERS += \
    activeqthread.h \
    macro_definitions.h \
    shared_queue.h

# specify builddir BEFORE  include make-test-settings.pri
builddir = ../build
DESTDIR = $$builddir

! include( ../make-test-settings.pri ) {
    error( Couldn't find the make-test-settings.pri file! )
}

POST_TARGETDEPS += $$DESTDIR/lib3rdparty_gtest.a
INCLUDEPATH += $${GTEST_INSTALL_HEADERS}
LIBS += -L$$DESTDIR -l3rdparty_gtest

OTHER_FILES += \
    looping_test.sh
