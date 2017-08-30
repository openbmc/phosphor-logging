# export these headers
nobase_include_HEADERS = \
	phosphor-logging/log.hpp \
	phosphor-logging/elog.hpp

nobase_nodist_include_HEADERS = \
	phosphor-logging/elog-errors.hpp \
	xyz/openbmc_project/Logging/Internal/Manager/server.hpp

libphosphor_logging_FILES = \
	elog.cpp elog_meta.cpp
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
bin_PROGRAMS = logging-test callout-test
logging_test_SOURCES = logging_test.cpp
logging_test_LDADD = libphosphor_logging.la
callout_test_SOURCES = callouts/callout_test.cpp

sbin_PROGRAMS = phosphor-log-manager

phosphor_log_manager_SOURCES = \
	xyz/openbmc_project/Logging/Internal/Manager/server.cpp \
	log_manager.cpp \
	log_manager_main.cpp \
	elog-lookup.cpp \
	elog_entry.cpp \
        log_collection.cpp \
	org.openbmc.Associations.cpp \
	elog-process-metadata.cpp \
	elog_meta.cpp \
	elog_serialize.cpp

# Be sure to build needed files before compiling
BUILT_SOURCES = \
	phosphor-logging/elog-errors.hpp \
	elog-lookup.cpp \
	elog-process-metadata.cpp \
	xyz/openbmc_project/Logging/Internal/Manager/server.cpp \
	xyz/openbmc_project/Logging/Internal/Manager/server.hpp \
	callouts-gen.hpp

CLEANFILES = \
	phosphor-logging/elog-errors.hpp \
	elog-lookup.cpp \
	elog-process-metadata.cpp \
	xyz/openbmc_project/Logging/Internal/Manager/server.cpp \
	xyz/openbmc_project/Logging/Internal/Manager/server.hpp \
	callouts-gen.hpp

# systemd required for journal interfaces
logging_test_LDFLAGS = $(SYSTEMD_LIBS) $(SDBUSPLUS_LIBS)
logging_test_CXXFLAGS = $(SYSTEMD_CFLAGS) $(SDBUSPLUS_CFLAGS)
callout_test_LDADD = libphosphor_logging.la
callout_test_LDFLAGS = $(SDBUSPLUS_LIBS) $(PHOSPHOR_DBUS_INTERFACES_LIBS)
callout_test_CXXFLAGS = $(SDBUSPLUS_CFLAGS) $(PHOSPHOR_DBUS_INTERFACES_CFLAGS)
phosphor_log_manager_LDFLAGS = \
		$(SYSTEMD_LIBS) \
		$(SDBUSPLUS_LIBS) \
		$(PHOSPHOR_DBUS_INTERFACES_LIBS) \
		-lstdc++fs
phosphor_log_manager_CXXFLAGS = \
		$(SYSTEMD_CFLAGS) \
		$(SDBUSPLUS_CFLAGS) \
		$(PHOSPHOR_DBUS_INTERFACES_CFLAGS)

xyz/openbmc_project/Logging/Internal/Manager/server.cpp: xyz/openbmc_project/Logging/Internal/Manager.interface.yaml xyz/openbmc_project/Logging/Internal/Manager/server.hpp
	@mkdir -p `dirname $@`
	$(SDBUSPLUSPLUS) -r $(srcdir) interface server-cpp xyz.openbmc_project.Logging.Internal.Manager > $@

xyz/openbmc_project/Logging/Internal/Manager/server.hpp: xyz/openbmc_project/Logging/Internal/Manager.interface.yaml
	@mkdir -p `dirname $@`
	$(SDBUSPLUSPLUS) -r $(srcdir) interface server-header xyz.openbmc_project.Logging.Internal.Manager > $@

ELOG_MAKO ?= elog-gen-template.mako.hpp
LOOKUP_MAKO ?= elog-lookup-template.mako.cpp
META_MAKO ?= elog-process-metadata.mako.cpp
CALLOUTS_MAKO ?= callouts-gen.mako.hpp
ELOG_TEMPLATE_DIR ?= ${abs_srcdir}/tools/phosphor-logging/templates/
REQ_FILES_TO_GEN ?= ${abs_srcdir}/tools/elog-gen.py\
	${ELOG_TEMPLATE_DIR}/$(ELOG_MAKO)\
	${ELOG_TEMPLATE_DIR}/$(META_MAKO)\
	${abs_srcdir}/callouts/callouts.py\
	${abs_srcdir}/callouts/$(CALLOUTS_MAKO)

# Be sure to package up the required script, yaml, and mako template to
# generate the header file
EXTRA_DIST = $(REQ_FILES_TO_GEN)

phosphor-logging/elog-errors.hpp: $(REQ_FILES_TO_GEN)
	@mkdir -p `dirname $@`
	$(AM_V_at)$(PYTHON) ${abs_srcdir}/tools/elog-gen.py -y ${YAML_DIR} -u ${YAML_DIR_TEST} -t ${ELOG_TEMPLATE_DIR} -m $(ELOG_MAKO) -o $@
elog-lookup.cpp: $(REQ_FILES_TO_GEN)
	$(AM_V_at)$(PYTHON) ${abs_srcdir}/tools/elog-gen.py -y ${YAML_DIR} -u ${YAML_DIR_TEST} -t ${ELOG_TEMPLATE_DIR} -m $(LOOKUP_MAKO) -o $@
elog-process-metadata.cpp: $(REQ_FILES_TO_GEN)
	$(AM_V_at)$(PYTHON) ${abs_srcdir}/tools/elog-gen.py -y ${YAML_DIR} -u ${YAML_DIR_TEST} -t ${ELOG_TEMPLATE_DIR} -m $(META_MAKO) -o $@
callouts-gen.hpp: $(REQ_FILES_TO_GEN)
	$(AM_V_at)$(PYTHON) ${abs_srcdir}/callouts/callouts.py -i ${CALLOUTS_YAML}

pkgconfiglibdir = ${libdir}/pkgconfig
pkgconfiglib_DATA = phosphor-logging.pc

SUBDIRS = test
