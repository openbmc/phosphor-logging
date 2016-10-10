# gtest unit tests which run during a 'make check'
check_PROGRAMS =

# Run all 'check' test programs
TESTS = $(check_PROGRAMS)

# Basic test suite for elog interfaces
check_PROGRAMS += elog_unittest
elog_unittest_CPPFLAGS = -Igtest $(GTEST_CPPFLAGS)
elog_unittest_CXXFLAGS = $(PTHREAD_CFLAGS)
elog_unittest_LDFLAGS = -lgtest_main -lgtest $(PTHREAD_LIBS) $(OESDK_TESTCASE_FLAGS)
elog_unittest_SOURCES = elog_unittest.cpp