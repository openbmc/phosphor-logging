bin_PROGRAMS = logging-test
logging_test_SOURCES = test/logging_test.cpp

# systemd required for journal interfaces
logging_test_LDFLAGS = $(SYSTEMD_LIBS)

# export these headers
include_HEADERS = log.hpp elog.hpp