phosphor_log_manager_SOURCES += \
	extensions/openpower-pels/bcd_time.cpp \
	extensions/openpower-pels/entry_points.cpp \
	extensions/openpower-pels/log_id.cpp \
	extensions/openpower-pels/manager.cpp \
	extensions/openpower-pels/paths.cpp \
	extensions/openpower-pels/pel.cpp \
	extensions/openpower-pels/private_header.cpp \
	extensions/openpower-pels/repository.cpp \
	extensions/openpower-pels/user_header.cpp

registrydir = $(datadir)/phosphor-logging/pels/
registry_DATA = extensions/openpower-pels/registry/message_registry.json
