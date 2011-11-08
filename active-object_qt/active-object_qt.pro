

TEMPLATE = subdirs
CONFIG += ordered

# specify builddir BEFORE  include make-test-settings.pri
builddir = ../build
DESTDIR = $$builddir
# specify builddir BEFORE  include make-settings.pri
! include( ../make-test-settings.pri ) {
    error( Couldn't find the make-test-settings.pri file! )
}


#3rd party subdirs
SUBDIRS = $$DESTDIR/../3rdParty/gtest/3rdparty_gtest.pro
SUBDIRS +=  test_active-object_qt.pro
