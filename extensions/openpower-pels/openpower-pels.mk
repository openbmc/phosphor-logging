phosphor_log_manager_SOURCES += \
	extensions/openpower-pels/bcd_time.cpp \
	extensions/openpower-pels/entry_points.cpp \
	extensions/openpower-pels/log_id.cpp \
	extensions/openpower-pels/manager.cpp \
	extensions/openpower-pels/paths.cpp \
	extensions/openpower-pels/pel.cpp \
	extensions/openpower-pels/pel_values.cpp \
	extensions/openpower-pels/private_header.cpp \
	extensions/openpower-pels/repository.cpp \
	extensions/openpower-pels/user_header.cpp

bin_PROGRAMS += peltool

peltool_SOURCES = extensions/openpower-pels/tools/peltool.cpp \
       extensions/openpower-pels/tools/peltool_utility.cpp \
       extensions/openpower-pels/tools/pelparser.cpp \
       test/openpower-pels/pel_utils.cpp

peltool_LDADD = \
    extensions/openpower-pels/pel.o \
    extensions/openpower-pels/pel_values.o \
    extensions/openpower-pels/bcd_time.o \
    extensions/openpower-pels/private_header.o \
    extensions/openpower-pels/user_header.o \
    extensions/openpower-pels/log_id.o \
    extensions/openpower-pels/paths.o

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
