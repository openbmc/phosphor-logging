# export these headers
include_HEADERS = \
	phosphor-logging/log.hpp \
	phosphor-logging/elog.hpp \
	phosphor-logging/elog-errors-HostEvent.hpp

nobase_include_HEADERS = \
	phosphor-logging/log.hpp \
	phosphor-logging/elog.hpp \
	phosphor-logging/elog-errors-HostEvent.hpp

nodist_include_HEADERS = phosphor-logging/elog-errors.hpp

libphosphor_logging_FILES = \
	elog.cpp
libphosphor_loggingdir = ${libdir}
libphosphor_logging_LTLIBRARIES = libphosphor_logging.la
libphosphor_logging_la_SOURCES = $(libphosphor_logging_FILES)
libphosphor_logging_la_LDFLAGS = \
	$(SYSTEMD_LIBS) \
	$(SDBUSPLUS_LIBS) \
	-version-info 1:0:0 -shared
libphosphor_logging_la_CXXFLAGS = \
	$(SDBUSPLUS_CFLAGS)

# Application test which runs in obmc env (qemu, real hardware)
bin_PROGRAMS = logging-test
logging_test_SOURCES = logging_test.cpp

sbin_PROGRAMS = phosphor-log-manager

phosphor_log_manager_SOURCES = \
	xyz.openbmc_project.Logging.Internal.Manager.cpp \
	log_manager.cpp \
	log_manager_main.cpp \
	elog_entry.cpp

# Be sure to build needed files before compiling
BUILT_SOURCES = \
	phosphor-logging/elog-errors.hpp \
	elog-lookup.cpp

CLEANFILES = \
	phosphor-logging/elog-errors.hpp \
	elog-lookup.cpp

# systemd required for journal interfaces
logging_test_LDFLAGS = $(SYSTEMD_LIBS) $(SDBUSPLUS_LIBS)
logging_test_CXXFLAGS = $(SYSTEMD_CFLAGS) $(SDBUSPLUS_CFLAGS)
phosphor_log_manager_LDFLAGS = \
		$(SYSTEMD_LIBS) \
		$(SDBUSPLUS_LIBS) \
		$(PHOSPHOR_DBUS_INTERFACES_LIBS)
phosphor_log_manager_CXXFLAGS = \
		$(SYSTEMD_CFLAGS) \
		$(SDBUSPLUS_CFLAGS) \
		$(PHOSPHOR_DBUS_INTERFACES_CFLAGS)

ELOG_MAKO ?= elog-gen-template.mako.hpp
LOOKUP_MAKO ?= elog-lookup-template.mako.cpp
ELOG_TEMPLATE_DIR ?= ${abs_srcdir}/tools/phosphor-logging/templates/
REQ_FILES_TO_GEN ?= ${abs_srcdir}/tools/elog-gen.py\
	${ELOG_TEMPLATE_DIR}/$(ELOG_MAKO)

# Be sure to package up the required script, yaml, and mako template to
# generate the header file
EXTRA_DIST = $(REQ_FILES_TO_GEN)

phosphor-logging/elog-errors.hpp: $(REQ_FILES_TO_GEN)
	@mkdir -p `dirname $@`
	$(AM_V_at)$(PYTHON) ${abs_srcdir}/tools/elog-gen.py -y ${YAML_DIR} -u ${YAML_DIR_TEST} -t ${ELOG_TEMPLATE_DIR} -m $(ELOG_MAKO) -o $@
elog-lookup.cpp: $(REQ_FILES_TO_GEN)
	$(AM_V_at)$(PYTHON) ${abs_srcdir}/tools/elog-gen.py -y ${YAML_DIR} -u ${YAML_DIR_TEST} -t ${ELOG_TEMPLATE_DIR} -m $(LOOKUP_MAKO) -o $@

pkgconfiglibdir = ${libdir}/pkgconfig
pkgconfiglib_DATA = phosphor-logging.pc

SUBDIRS = test
