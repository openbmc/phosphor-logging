# export these headers
include_HEADERS = log.hpp elog.hpp

# Generate this header, don't export in a distro
nodist_include_HEADERS = elog-gen.hpp

# Application test which runs in obmc env (qemu, real hardware)
bin_PROGRAMS = logging-test
logging_test_SOURCES = logging_test.cpp

# Be sure to build elog-gen.hpp before compiling
BUILT_SOURCES = elog-gen.hpp

CLEANFILES=elog-gen.hpp

# systemd required for journal interfaces
logging_test_LDFLAGS = $(SYSTEMD_LIBS)

ELOG_YAML ?= ${abs_srcdir}/tools/example/xyz/openbmc_project/Example/Elog.errors.yaml
ELOG_MAKO ?= ${abs_srcdir}/tools/phosphor-logging/templates/elog-gen-template.mako.hpp
REQ_FILES_TO_GEN ?= ${abs_srcdir}/tools/elog-gen.py $(ELOG_YAML) $(ELOG_MAKO)

# Be sure to package up the required script, yaml, and mako template to
# generate the header file
EXTRA_DIST = $(REQ_FILES_TO_GEN)

elog-gen.hpp: $(REQ_FILES_TO_GEN)
	$(AM_V_at)${abs_srcdir}/tools/elog-gen.py -e $(ELOG_YAML) -m $(ELOG_MAKO) -o ${abs_srcdir}/elog-gen.hpp

SUBDIRS = test