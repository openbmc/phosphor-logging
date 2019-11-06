phosphor_log_manager_SOURCES += \
	extensions/openpower-pels/ascii_string.cpp \
	extensions/openpower-pels/bcd_time.cpp \
	extensions/openpower-pels/callout.cpp \
	extensions/openpower-pels/callouts.cpp \
	extensions/openpower-pels/data_interface.cpp \
	extensions/openpower-pels/entry_points.cpp \
	extensions/openpower-pels/failing_mtms.cpp \
	extensions/openpower-pels/fru_identity.cpp \
	extensions/openpower-pels/generic.cpp \
        extensions/openpower-pels/hexdump.cpp \
	extensions/openpower-pels/log_id.cpp \
	extensions/openpower-pels/manager.cpp \
	extensions/openpower-pels/mru.cpp \
	extensions/openpower-pels/mtms.cpp \
	extensions/openpower-pels/paths.cpp \
	extensions/openpower-pels/pce_identity.cpp \
	extensions/openpower-pels/pel.cpp \
	extensions/openpower-pels/pel_rules.cpp \
	extensions/openpower-pels/pel_values.cpp \
	extensions/openpower-pels/private_header.cpp \
	extensions/openpower-pels/registry.cpp \
	extensions/openpower-pels/repository.cpp \
	extensions/openpower-pels/src.cpp \
	extensions/openpower-pels/section_factory.cpp \
	extensions/openpower-pels/severity.cpp \
	extensions/openpower-pels/user_data.cpp \
	extensions/openpower-pels/user_header.cpp

registrydir = $(datadir)/phosphor-logging/pels/
registry_DATA = extensions/openpower-pels/registry/message_registry.json

bin_PROGRAMS += peltool

peltool_SOURCES = extensions/openpower-pels/tools/peltool.cpp

peltool_LDADD = \
        extensions/openpower-pels/ascii_string.o \
        extensions/openpower-pels/bcd_time.o \
        extensions/openpower-pels/callout.o \
        extensions/openpower-pels/callouts.o \
        extensions/openpower-pels/failing_mtms.o \
        extensions/openpower-pels/fru_identity.o \
        extensions/openpower-pels/generic.o \
        extensions/openpower-pels/hexdump.o \
        extensions/openpower-pels/log_id.o \
        extensions/openpower-pels/mru.o \
        extensions/openpower-pels/mtms.o \
        extensions/openpower-pels/paths.o \
        extensions/openpower-pels/pce_identity.o \
        extensions/openpower-pels/pel.o \
        extensions/openpower-pels/pel_rules.o \
        extensions/openpower-pels/pel_values.o \
        extensions/openpower-pels/private_header.o \
        extensions/openpower-pels/registry.o \
        extensions/openpower-pels/repository.o \
        extensions/openpower-pels/src.o \
        extensions/openpower-pels/section_factory.o \
        extensions/openpower-pels/severity.o \
        extensions/openpower-pels/user_data.o \
        extensions/openpower-pels/user_header.o


peltool_LDFLAGS = \
        $(SYSTEMD_LIBS) \
        $(PHOSPHOR_LOGGING_LIBS) \
        $(SDBUSPLUS_LIBS) \
        $(PHOSPHOR_DBUS_INTERFACES_LIBS) \
        $(SDEVENTPLUS_LIBS) \
        -lstdc++fs

peltool_CXXFLAGS = \
        $(SYSTEMD_CFLAGS) \
        $(SDBUSPLUS_CFLAGS) \
        $(SDEVENTPLUS_CFLAGS) \
        $(PHOSPHOR_DBUS_INTERFACES_CFLAGS)
