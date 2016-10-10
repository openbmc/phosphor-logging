# export these headers
include_HEADERS = log.hpp elog.hpp

# Application test which runs in obmc env (qemu, real hardware)
bin_PROGRAMS = logging-test
logging_test_SOURCES = logging_test.cpp

# systemd required for journal interfaces
logging_test_LDFLAGS = $(SYSTEMD_LIBS)

SUBDIRS = test