# export these headers
include_HEADERS = log.hpp elog.hpp elog-gen.hpp

# Application test which runs in obmc env (qemu, real hardware)
bin_PROGRAMS = logging-test
logging_test_SOURCES = logging_test.cpp

# Be sure to build elog-gen.hpp before compiling
BUILT_SOURCES = elog-gen.hpp

CLEANFILES=elog-gen.hpp

# systemd required for journal interfaces
logging_test_LDFLAGS = $(SYSTEMD_LIBS)

ELOG_YAML ?= ${srcdir}/elog.yaml
ELOG_MAKO ?= ${srcdir}/elog-gen-template.mako.hpp

elog-gen.hpp: ${srcdir}/elog-gen.py $(ELOG_YAML) $(ELOG_MAKO)
	echo "Generating elog-gen.hpp"
	${srcdir}/elog-gen.py -e $(ELOG_YAML) -m $(ELOG_MAKO)

SUBDIRS = test