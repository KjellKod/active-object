#-------------------------------------------------
#
# $Id: $
#
#-------------------------------------------------

# remove unsigned, signed comparison warnings for gtests "check" macros
# --- this doesn't seem to work --- KjellKod
QMAKE_CXXFLAGS += -Wno-sign-compare
GTEST_INSTALL_HEADERS  = $$builddir/../3rdParty/gtest/gtest-1.6.0__stripped/include


OBJECTS_DIR  =$$builddir/.obj
MOC_DIR      =$$builddir/.moc
UI_DIR       =$$builddir/.ui
RCC_DIR      =$$builddir/.rcc
