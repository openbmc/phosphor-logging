phosphor_log_manager_SOURCES += \
	extensions/openpower-pels/bcd_time.cpp \
	extensions/openpower-pels/entry_points.cpp \
	extensions/openpower-pels/log_id.cpp \
	extensions/openpower-pels/manager.cpp \
	extensions/openpower-pels/paths.cpp \
	extensions/openpower-pels/pel.cpp \
	extensions/openpower-pels/private_header.cpp \
	extensions/openpower-pels/repository.cpp \
	extensions/openpower-pels/user_header.cpp \
        extensions/openpower-pels/tools/peltool.cpp \
        extensions/openpower-pels/tools/peltool_utility.cpp \
        test/openpower-pels/pel_utils.cpp

peltool_SOURCES += \
        extensions/openpower-pels/bcd_time.cpp \
        extensions/openpower-pels/entry_points.cpp \
        extensions/openpower-pels/log_id.cpp \
        extensions/openpower-pels/manager.cpp \
        extensions/openpower-pels/paths.cpp \
        extensions/openpower-pels/pel.cpp \
        extensions/openpower-pels/private_header.cpp \
        extensions/openpower-pels/repository.cpp \
        extensions/openpower-pels/user_header.cpp \
        extensions/openpower-pels/tools/peltool.cpp \
        extensions/openpower-pels/tools/peltool_utility.cpp \
        test/openpower-pels/pel_utils.cpp 

