# export these headers
include_HEADERS = log.hpp elog.hpp

# Generate this header, don't export in a distro
nodist_include_HEADERS = elog-gen.hpp

# Application test which runs in obmc env (qemu, real hardware)
bin_PROGRAMS = logging-test
logging_test_SOURCES = logging_test.cpp

sbin_PROGRAMS = phosphor-log-manager
phosphor_log_manager_SOURCES = \
	xyz.openbmc_project.Logging.Internal.Manager.cpp \
	log_manager.cpp \
	log_manager_main.cpp

# Be sure to build needed files before compiling
BUILT_SOURCES = elog-gen.hpp elog-lookup.cpp

CLEANFILES = \
	elog-gen.hpp \
	elog-lookup.cpp

# systemd required for journal interfaces
logging_test_LDFLAGS = $(SYSTEMD_LIBS)
phosphor_log_manager_LDFLAGS = $(SYSTEMD_LIBS)

ELOG_YAML ?= xyz/openbmc_project/Example/Elog.errors.yaml
ELOG_MAKO ?= elog-gen-template.mako.hpp
LOOKUP_MAKO ?= elog-lookup-template.mako.cpp
ELOG_YAML_DIR ?= tools/example/
ELOG_TEMPLATE_DIR ?= tools/phosphor-logging/templates/
REQ_FILES_TO_GEN ?= ${abs_srcdir}/tools/elog-gen.py\
	${abs_srcdir}/$(ELOG_YAML_DIR)/$(ELOG_YAML)\
	${abs_srcdir}/${ELOG_TEMPLATE_DIR}/$(ELOG_MAKO)

# Be sure to package up the required script, yaml, and mako template to
# generate the header file
EXTRA_DIST = $(REQ_FILES_TO_GEN)

elog-gen.hpp: $(REQ_FILES_TO_GEN)
	$(AM_V_at)$(PYTHON) ${abs_srcdir}/tools/elog-gen.py -r ${abs_srcdir}/${ELOG_YAML_DIR} -t ${abs_srcdir}/${ELOG_TEMPLATE_DIR} -e $(ELOG_YAML) -m $(ELOG_MAKO) -o ${abs_srcdir}/elog-gen.hpp
elog-lookup.cpp: $(REQ_FILES_TO_GEN)
	$(AM_V_at)$(PYTHON) ${abs_srcdir}/tools/elog-gen.py -r ${abs_srcdir}/${ELOG_YAML_DIR} -t ${abs_srcdir}/${ELOG_TEMPLATE_DIR} -e $(ELOG_YAML) -m $(LOOKUP_MAKO) -o ${abs_srcdir}/elog-lookup.cpp

SUBDIRS = test
