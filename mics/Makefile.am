bin_PROGRAMS = lockfile cnsleep waitpid scheduler_admin scheduler_protocol_test

lockfile_SOURCES = lockfile.c 
cnsleep_SOURCES = cnsleep.c
waitpid_SOURCES = waitpid.c

scheduler_protocol_admin_CFLAGS = -I$(top_srcdir)/cores -I$(top_srcdir)/drivers
scheduler_protocol_admin_LDADD = ../cores/libaaoscore.la ../drivers/libaaosdriver.la
scheduler_protocol_admin_SOURCES = scheduler_admin.c

scheduler_protocol_test_CFLAGS = -I$(top_srcdir)/cores -I$(top_srcdir)/drivers
scheduler_protocol_test_LDADD = ../cores/libaaoscore.la ../drivers/libaaosdriver.la
scheduler_protocol_test_SOURCES = scheduler_protocol_test.c
