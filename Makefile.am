AM_DEFAULT_SOURCE_EXT = .cpp

bin_PROGRAMS = logging-test
logging_test_SOURCES = test/logging_test.cpp

# systemd required for journal interfaces
logging_test_LDFLAGS = $(SYSTEMD_LIBS) $(LIBADD_DLOPEN)

# export these headers
include_HEADERS = log.hpp elog.hpp