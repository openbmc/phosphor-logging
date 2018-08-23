noinst_HEADERS = \
	utils.hpp \
	server-conf.hpp

sbin_PROGRAMS = phosphor-rsyslog-conf

phosphor_rsyslog_conf_SOURCES = \
	main.cpp \
	server-conf.cpp

phosphor_rsyslog_conf_LDFLAGS = \
	$(SDBUSPLUS_LIBS) \
	$(PHOSPHOR_LOGGING_LIBS) \
	$(PHOSPHOR_DBUS_INTERFACES_LIBS)

phosphor_rsyslog_conf_CXXFLAGS = \
	$(SDBUSPLUS_CFLAGS) \
	$(PHOSPHOR_LOGGING_CFLAGS) \
	$(PHOSPHOR_DBUS_INTERFACES_CFLAGS) \
	-flto
