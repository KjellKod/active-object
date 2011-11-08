# CMakeLists.txt
# 2010 @author Kjell Hedström, hedstrom@kjellkod.cc */
#
# == README: Example how to setup environment + running tests ===============
# 1. Install gtest
#    cmake
#    make
#    make install (possibly as root)
#
#
#
# 2. update path to libraries
#    sudo /sbin/ldconfig -v   | grep gtest
#
#    the grep is only to verify that it works. It should give something like
#       ... other stuff ...
#      	libgtest.so.0 -> libgtest.so.0.0.0
#	libgtest_main.so.0 -> libgtest_main.so.0.0.0
#
#
#
# 3. To test this:
#    cd lock_free/build
#    cmake ..    # create makefiles in lock_free/build directory
#    make        # link gtest and library CircularFifo with test executable
#                  ./testCircularFifo
#   
# should provide output like
# [==========] Running 1 test from 1 test case.
# [----------] Global test environment set-up.
# [----------] 1 test from CircularFifoTest
# [ RUN      ] CircularFifoTest.IsEmptyInitially
# [       OK ] CircularFifoTest.IsEmptyInitially (0 ms)
# [----------] 1 test from CircularFifoTest (1 ms total)
#
# [----------] Global test environment tear-down
# [==========] 1 test from 1 test case ran. (1 ms total)
# [  PASSED  ] 1 test.
#
# ============================================================================

cmake_minimum_required (VERSION 2.6)
project (TestLockFreeFifo) 

# --coverage is a gcc synonum for "-fprofile-arcs -ftest-coverage"
# -- and for those flags + "-lgcov" when linking"
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} -pthread -D_THREAD_SAFE -g -O0 -Wall -Wshadow -Wunused-variable --coverage") 

# add a src library, since it's a template library and only '.h' file
# it has to have the language defined as well 'set_target_properties'
add_library(CircularFifo src/circularfifo.h ../active_object/src/activecallback.h ../active_object/src/active.h ../active_object/src/active.cpp)
set_target_properties(CircularFifo PROPERTIES LINKER_LANGUAGE CXX)

# make the src directory available for test classes
include_directories(src) 
include_directories(../active_object/src) 
include_directories(../active_object/test) 

# create the test executable (main + test class)
add_executable(testCircularFifo test/test_main.cpp test/test_circularfifo.cpp test/backgrounder.h)

set(CMAKE_LDFLAGS "--coverage")

# SETUP for GTEST
set(GTEST_DIR ../../3rdParty/gtest/gtest-1.6.0__stripped)
set(GTEST_INCLUDE_DIRECTORIES ${GTEST_DIR}/include ${GTEST_DIR} ${GTEST_DIR}/src)
include_directories(${GTEST_INCLUDE_DIRECTORIES})
add_library(gtest_160_lib ${GTEST_DIR}/src/gtest-all.cc ${GTEST_DIR}/src/gtest_main.cc)
enable_testing(true) 


# link test executable with the src library
target_link_libraries(testCircularFifo CircularFifo gtest_160_lib)

