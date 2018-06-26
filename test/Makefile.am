AM_CPPFLAGS = -I${top_srcdir}

TESTS = $(check_PROGRAMS)

check_PROGRAMS = \
	elog_unittest \
	serialization_test_path \
	serialization_test_properties

test_cppflags = \
	-Igtest \
	$(GTEST_CPPFLAGS) \
	$(AM_CPPFLAGS) \
	$(PHOSPHOR_DBUS_INTERFACES_CFLAGS) \
	$(SDBUSPLUS_CFLAGS)

test_cxxflags = \
	$(PTHREAD_CFLAGS)

test_ldflags = \
	-lgtest_main -lgtest \
	-lgmock_main -lgmock \
	$(PTHREAD_LIBS) \
	$(OESDK_TESTCASE_FLAGS) \
	$(PHOSPHOR_DBUS_INTERFACES_LIBS) \
	$(SDBUSPLUS_LIBS)

test_ldadd = \
	$(top_builddir)/elog_serialize.o \
	$(top_builddir)/elog_entry.o \
	$(top_builddir)/log_manager.o \
	$(top_builddir)/org.openbmc.Associations.o \
	$(top_builddir)/xyz/openbmc_project/Logging/Internal/Manager/server.o \
	$(top_builddir)/elog_meta.o \
	$(top_builddir)/elog-lookup.o \
	$(top_builddir)/elog-process-metadata.o


elog_unittest_CPPFLAGS = $(test_cppflags)
elog_unittest_CXXFLAGS = $(test_cxxflags)
elog_unittest_SOURCES = elog_unittest.cpp
elog_unittest_LDADD = $(test_ldadd)
elog_unittest_LDFLAGS = \
        $(test_ldflags) \
        -lstdc++fs

serialization_test_path_CPPFLAGS = $(test_cppflags)
serialization_test_path_CXXFLAGS = $(test_cxxflags)
serialization_test_path_SOURCES = serialization_test_path.cpp
serialization_test_path_LDADD = $(test_ldadd)
serialization_test_path_LDFLAGS = \
	$(test_ldflags) \
	-lstdc++fs

serialization_test_properties_CPPFLAGS = $(test_cppflags)
serialization_test_properties_CXXFLAGS = $(test_cxxflags)
serialization_test_properties_SOURCES = serialization_test_properties.cpp
serialization_test_properties_LDADD = $(test_ldadd)
serialization_test_properties_LDFLAGS = \
	$(test_ldflags) \
	-lstdc++fs
