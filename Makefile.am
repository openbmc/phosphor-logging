# export these headers
include_HEADERS = \
	phosphor-logging/log.hpp \
	phosphor-logging/elog.hpp \
	phosphor-logging/elog-errors-HostEvent.hpp

nobase_include_HEADERS = \
	phosphor-logging/log.hpp \
	phosphor-logging/elog.hpp \
	phosphor-logging/elog-errors-HostEvent.hpp

# Generate this header, don't export in a distro
nodist_include_HEADERS = elog-gen.hpp

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
BUILT_SOURCES = elog-gen.hpp elog-lookup.cpp

CLEANFILES = \
	elog-gen.hpp \
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
ELOG_YAML_DIR ?= tools/example/xyz/openbmc_project/Example/
ELOG_TEMPLATE_DIR ?= tools/phosphor-logging/templates/
REQ_FILES_TO_GEN ?= ${abs_srcdir}/tools/elog-gen.py\
	${abs_srcdir}/$(ELOG_YAML_DIR)/$(ELOG_YAML)\
	${abs_srcdir}/${ELOG_TEMPLATE_DIR}/$(ELOG_MAKO)

# Be sure to package up the required script, yaml, and mako template to
# generate the header file
EXTRA_DIST = $(REQ_FILES_TO_GEN)

elog-gen.hpp: $(REQ_FILES_TO_GEN)
	$(AM_V_at)$(PYTHON) ${abs_srcdir}/tools/elog-gen.py -y ${abs_srcdir}/${ELOG_YAML_DIR} -t ${abs_srcdir}/${ELOG_TEMPLATE_DIR} -m $(ELOG_MAKO) -o ${abs_srcdir}/elog-gen.hpp
elog-lookup.cpp: $(REQ_FILES_TO_GEN)
	$(AM_V_at)$(PYTHON) ${abs_srcdir}/tools/elog-gen.py -y ${abs_srcdir}/${ELOG_YAML_DIR} -t ${abs_srcdir}/${ELOG_TEMPLATE_DIR} -m $(LOOKUP_MAKO) -o ${abs_srcdir}/elog-lookup.cpp

pkgconfiglibdir = ${libdir}/pkgconfig
pkgconfiglib_DATA = phosphor-logging.pc

SUBDIRS = test
